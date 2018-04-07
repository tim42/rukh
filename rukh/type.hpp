//
// file : type.hpp
// in : file:///home/tim/projects/rukh/rukh/type.hpp
//
// created by : Timothée Feuillet
// date: sam. janv. 20 14:10:37 2018 GMT-0500
//
//
// Copyright (c) 2018 Timothée Feuillet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include <array>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "string.hpp"


namespace rukh
{
  class type_db;

  /// \brief Represent a type in the AST.
  /// This can be an explicit type (like 'int' or 'float2' can be) or an meta type
  /// (like 'number', 'vector' or 'any').
  ///
  /// Types also convey information about into what it is permitted to cast a given type into,
  /// member access, or swizzling.
  ///
  /// \note Some functions of the class are implemented in type_db.hpp (because of the dependency with type_db)
  class type
  {
    public:
      using ref = hash_t;

      enum class cast_type
      {
        none = 0,
        not_possible = 0,

        lossless = 1 << 1, // the cast does not loose information
        implicit = 1 << 2, // cast can be done implicitly

        constant = 1 << 3, // cast can be a constant operation (FIXME Do I really need that ????)
        generates_ir = 1 << 4, // cast will generate IR (FIXME Do I really need that ????)
      };

      /// \brief Holds the definition of a type
      /// \note It has been made so that most fields will uses a type::ref instead of a type
      /// \note It has been made so that IR can be used to generate meta-types
      ///       (hence having the capability to fully support meta-language)
      /// Quite a lot of helper functions are available for each function fields in this structure.
      /// It has some overhead (most importantly for arrays) but makes creating complex meta-types possible
      struct definition
      {
        type::ref type_id; // should very probably be something like rukh_str_hash("type-name")
        std::string debug_name = {};

        // Size of the type.
        // NOTE: this is the size without any members
        //       for structure / array (or vector) -like types, it should be 0 unless there is some storage required,
        //       whereas for primitives it should be the primitive size
        // NOTE: meta-types cannot have a size
        size_t size;

        // Array dimension (should not be 0, except for some special cases)
        size_t dim = 1;
        // Used instead of dim. Also indicates a meta-array-type
        std::function<bool(size_t)> is_dim_valid_for = {};

        // if false, only subtypes* is used, if true, members*, cast_into, {de,con}struct* are used
        bool concrete;

        // if true, allows to bypass construct_from to be called if every members in the members map is initialized
        bool can_default_construct = true;

        // members: (for concrete types)
        // NOTE: you can have meta-types as members

        std::map<hash_t, type::ref> members = {}; // hash -> type::ref for "static" members
        std::function<type::ref(hash_t)> members_getter = {}; // "dynamic" members (swizzling, ...). Return type::ref::zero when not existing.

        // sub-types: (for meta types)
        // NOTE: you should only use either subtypes or subtypes_getter

        std::set<type::ref> subtypes = {};
        std::function<bool(type::ref)> subtypes_getter = {};

        // casting info:
        std::map<type::ref, std::function<cast_type(/*TODO: IR-GEN + validate*/)>> cast_into = {};

        // construct from: (if not specified, default to construct members in the members map)
        std::function<bool(const std::vector<hash_t>& /*, TODO: IR-GEN + validate */)> construct_from = {};

        // destruct: (if not specified, will only destruct members. If there's not members, nothing will be done)
        // will be called to generate IR at the end of the lifecycle of the object
        std::function<bool(/*, TODO: IR-GEN + validate */)> destruct = {};
      };

  public:
      /// \brief Return the type::ref associated with the type
      type::ref get_ref() const { return def.type_id; }

      /// \brief Return whether or not a type can be implicitly casted into another
      bool can_implicit_cast(const type& other) const;

      /// \brief Return whether or not a type can be casted into another with precision loss
      bool can_lossless_cast(const type& other) const;

      /// \brief Check that the members are all of known types.
      /// A type can be invalid and become valid as new types are added.
      /// (this is not a constant state but more a current state).
      ///
      /// Invalid types cannot be resolved nor instanciated.
      /// Most functions will not work when dealing with non valid types.
      bool is_valid() const;

      /// \brief Whether or not a type is a primitive number (integer or floating point)
      /// Primitives are types with a non-zero size and no members (swizzling is fine)
      bool is_primitive() const
      {
        return def.members.empty() && is_fully_concrete();
      }

      /// \brief Whether or not a type is a concrete type ('explicit' type).
      bool is_concrete() const
      {
        return def.concrete;
      }

      /// \brief Whether or not a type is a concrete type ('explicit' type) and
      /// each one of its members is also a fully concrete type
      bool is_fully_concrete() const
      {
        if (!is_concrete())
          return false;
        if (def.is_dim_valid_for)
          return false;
        // TODO: check members
        return true;
      }

      /// \brief Return the size (in bytes) of the type (including its members).
      /// Return 0 if the type does not have a size (like a meta type)
      /// \note This size will not contain meaningful information if the type is not
      /// a fully concrete type
      size_t size() const;

      /// \brief Return whether or not the type has the specified member
      /// \tparam String must be a string as provided by rk_str("my-member")
      template<typename String>
      bool has_member() const
      {
        if (const auto it = def.members.find(String::hash); it != def.members.end())
          return true;
        if (def.members_getter)
          return def.members_getter(String::hash) != ref::zero;
        return false;
      }

      /// \brief Return whether or not the type has the specified member
      /// \warning must not be used for string literals. Only for runtime-strings.
      bool has_member(const std::string_view& sv) const
      {
        const hash_t hash = (hash_t)neam::ct::hash::fnv1a<64>((const uint8_t*)sv.data(), sv.size());
        if (const auto it = def.members.find(hash); it != def.members.end())
          return true;
        if (def.members_getter)
          return def.members_getter(hash) != ref::zero;
        return false;
      }

      /// \brief Return whether or not a given type is a valid resolution for the current type
      /// Non-meta-types only accept themselves as resolution
      /// Meta-types will accept matching types as resolution
      /// \note meta-types can also accept partial resolutions
      bool is_valid_resolution(const type& t) const;

      /// \brief Return the member type (or the special none type if none found)
      /// \tparam String must be a string as provided by rk_str("my-member")
      template<typename String>
      type get_member_type() const;

      /// \brief Return the member type (or the special none type if none found)
      /// \warning must not be used for string literals. Only for runtime-strings.
      type get_member_type(const std::string_view& sv) const;

      const type_db &tdb;
      const definition &def;
  };



  inline constexpr type::cast_type operator | (type::cast_type a, type::cast_type b)
  {
    using ut = typename std::underlying_type_t<type::cast_type>;
    return static_cast<type::cast_type>(static_cast<ut>(a) | static_cast<ut>(b));
  }
  inline constexpr type::cast_type& operator |= (type::cast_type& a, type::cast_type b)
  {
    using ut = typename std::underlying_type_t<type::cast_type>;
    a = static_cast<type::cast_type>(static_cast<ut>(a) | static_cast<ut>(b));
    return a;
  }
  inline constexpr type::cast_type operator & (type::cast_type a, type::cast_type b)
  {
    using ut = typename std::underlying_type_t<type::cast_type>;
    return static_cast<type::cast_type>(static_cast<ut>(a) & static_cast<ut>(b));
  }
  inline constexpr type::cast_type& operator &= (type::cast_type& a, type::cast_type b)
  {
    using ut = typename std::underlying_type_t<type::cast_type>;
    a = static_cast<type::cast_type>(static_cast<ut>(a) & static_cast<ut>(b));
    return a;
  }
} // namespace rukh
