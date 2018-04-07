// file : type_db.hpp
// in : file:///home/tim/projects/rukh/rukh/type_db.hpp
//
// created by : Timothée Feuillet
// date: dim. mars 25 15:38:37 2018 GMT-0400
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

#include <map>

#include "type.hpp"
#include "type_identity.hpp"

namespace rukh
{
  /// \brief Hold types definitions
  class type_db
  {
    public:
      /// \brief Add a new definition to the type DB
      /// \return whether or not the type has been added or not
      /// Reason for not adding a type definition is: conflict
      bool add_definition(type::definition&& def)
      {
        const auto [it, inserted] = definitions.emplace(def.type_id, std::move(def));
        (void)it;
        return inserted;
      }

      /// \brief Add a new definition to the type DB
      /// \return whether or not the type has been added or not
      /// Reason for not adding a type definition is: conflict
      bool add_definition(const type::definition& def)
      {
        const auto [it, inserted] = definitions.emplace(def.type_id, def);
        (void)it;
        return inserted;
      }

      /// \brief Return the type for a given type::ref or a spacial none type
      type get_type(type::ref id) const
      {
        if (const auto it = definitions.find(id); it != definitions.end())
          return {*this, it->second};
        return {*this, none};
      }

      /// \brief Return the special "none" type
      type get_none() const
      {
        return {*this, none};
      }

      // TODO: Some more utilities here

    private:
      const type::definition none
      {
        type::ref::zero,
        "none",
        0, // size
        0, // dim
        {}, // dim getter
        false, // concrete
      };
      std::map<type::ref, type::definition> definitions
      {
        {none.type_id, none},
        {rukh_str_hash("none"), none},
      };
  };







  // // // // //
  // // // // //
  // // // // //




  bool type::can_implicit_cast(const type& other) const
  {
    (void)other; // TODO
    return true;
  }

  bool type::can_lossless_cast(const type& other) const
  {
    (void)other; // TODO
    return true;
  }

  bool type::is_valid() const
  {
    if (def.dim == 0 || def.type_id == ref::zero)
      return false;
    for (auto&& it : def.members)
    {
      type t = tdb.get_type(it.second);
      if (!t.is_valid())
        return false;

      // TODO: make a contains() test for circular deps (with a bool flag as param)
    }
    return true;
  }

  size_t type::size() const
  {
    // can save us in case of a circular type
    if (!is_valid())
      return 0;

    size_t base_size = def.size;
    for (auto&& it : def.members)
      base_size += tdb.get_type(it.second).size();

    if (def.is_dim_valid_for)
      return base_size;

    return base_size * def.dim;
  }

  template<typename String>
  type type::get_member_type() const
  {
    if (const auto it = def.members.find(String::hash); it != def.members.end())
      return tdb.get_type(it->second);
    return tdb.get_none();
  }

  type type::get_member_type(const std::string_view &sv) const
  {
    const hash_t hash = (hash_t)neam::ct::hash::fnv1a<64>((const uint8_t*)sv.data(), sv.size());
    if (const auto it = def.members.find(hash); it != def.members.end())
      return tdb.get_type(it->second);
    return tdb.get_none();
  }

  bool type::is_valid_resolution(const type& t) const
  {
    // Test for self
    if (&t == this || &t.def == &def)
      return true;

#define return_false_if(x)  do{if (x) { return false; }}while(0)

    // fast exits:
    return_false_if(t.def.concrete != def.concrete);
    return_false_if(!is_valid() || !t.is_valid());

    // test the dim (ignore the test is t is a partial resolution)
    if (!t.def.is_dim_valid_for)
    {
      if (def.is_dim_valid_for)
        return_false_if(!def.is_dim_valid_for(t.def.dim));
      else
        return_false_if(def.dim != t.def.dim);
    }

    // for members (ignore the test for meta-types)
    if (t.def.concrete)
    {
      return_false_if(t.def.members.size() != def.members.size());
      for (auto&& member_res : t.def.members)
      {
        const auto member_it = def.members.find(member_res.first);

        // easy case: not found
        return_false_if(member_it == def.members.end());

        // easy case: the same type
        if (member_it->second == member_res.second)
          continue;

        // slow case: test for resolution
        const type member_type = tdb.get_type(member_it->second);
        const type member_res_type = tdb.get_type(member_res.second);
        return_false_if(!member_type.is_valid_resolution(member_res_type));
      }
    }
    // For sub-types (ignore the test for concrete types)
    else
    {
      // resolutions for meta types can only be more restrictives in the sense that
      // accepted sub-types should only be either resolution of types or types that are in the list
      for (auto&& subtype_id : t.def.subtypes)
      {
        // easy case: in the subtypes set or subtypes_getter returns true
        if (def.subtypes.count(subtype_id))
          continue;
        if (def.subtypes_getter && def.subtypes_getter(subtype_id))
          continue;

        // slow case: test for resolution:
        const type subtype = tdb.get_type(subtype_id);
        bool found = false;
        for (auto&& id : def.subtypes)
          found |= tdb.get_type(id).is_valid_resolution(subtype);

        return_false_if(!found);
      }
    }

#undef return_false_if

    return true;
  }
} // namespace rukh
