//
// file : string.h
// in : file:///home/tim/projects/rukh/rukh/string.h
//
// created by : Timothée Feuillet
// date: dim. févr. 11 19:36:25 2018 GMT-0500
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

#include <tools/hash/fnv1a.hpp>
#include <tools/ct_list.hpp>
#include <tools/embed.hpp>

namespace rukh
{
  enum class hash_t : uint64_t
  {
     zero = 0,
  };

  inline constexpr bool operator < (hash_t a, hash_t b) { return static_cast<uint64_t>(a) < static_cast<uint64_t>(b); }

  /// \brief Compile-time string
  template<typename Type, Type... Chs>
  struct ct_string
  {
    constexpr ct_string() noexcept = default;

    using type = Type;

    // length and size does not count the ending 0
    static constexpr size_t length = sizeof...(Chs);
    static constexpr size_t size = sizeof...(Chs) * sizeof(Type);

    template<size_t Index>
    using at = neam::ct::list::get_type<neam::ct::type_list<neam::embed<Chs, Type>...>, Index>;

    // null terminated array
    static constexpr Type array[] = {Chs..., 0};

    // string hash
    static constexpr hash_t hash = static_cast<hash_t>(neam::ct::hash::fnv1a<64>({Chs...}));
  };
} // namespace rukh


/// \brief SD
/// intended to be used with `using rukh_lit;`
namespace rukh_lit
{
  /// \brief Convert a string into a rukh::ct_string instance (to be used with the rukh_str macro)
  /// \warning gcc/clang only
  template<typename Type, Type... Ch>
  constexpr rukh::ct_string<Type, Ch...> operator "" _rukh_ctstr() {return {};}

  /// \brief Transform a string literal into a type (rukh::ct_string)
  /// \warning MUST take as input a string literal.
#define rukh_str(x)         decltype(x##_rukh_ctstr)

  /// \brief Hash a string at compile-time
  /// \warning MUST take as input a string literal.
#define rukh_str_hash(x)    rukh_str(x)::hash
} // namespace rukh_lit

namespace rukh
{
  // make sure rukh_str() is available
  using namespace rukh_lit;

#define rk_str(x)   rukh_str(x)
#define rk_pin_name(x)   rukh_str(x)
#define rk_type_name(x)  rukh_str(x)

} // namespace rukh

