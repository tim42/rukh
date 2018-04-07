//
// file : pin.hpp
// in : file:///home/tim/projects/rukh/rukh/pin.hpp
//
// created by : Timothée Feuillet
// date: dim. févr. 11 20:52:26 2018 GMT-0500
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

#include <cstddef>
#include <tools/ct_list.hpp>
#include "string.h"

namespace rukh
{
  namespace pin_def
  {
    /// \brief The pin is an array of multiple connections
    /// FIXME: ???
    template<size_t Min = 0, size_t Max = ~0ul>
    struct array
    {
      static_assert(Min <= Max, "Invalid array range");

      static constexpr size_t min = Min;
      static constexpr size_t max = Max;
    };

    

    /// \brief Add a custom validator to the pin. Can be used to check the type, the number of connections, the connections, ....
    /// \tparam ValidatorFnc Must be a function with the following signature: /* TODO */
    template<auto ValidatorFnc, typename Name = rk_str("unnamed-validator")>
    struct validator
    {
      constexpr static bool validate(/* TODO */)
      {
        return ValidatorFnc(/* TODO */);
      }
    };
  } // namespace pin_def

  /// \brief User definition of a pin
  /// \tparam Name must be a rk_str
  /// \tparam Type must be a rk_str
  /// \tparam Defs Additional definitions for the pin (see the pin_def namespace)
  template<typename Name, typename Type, typename... Defs>
  struct pin
  {
    using name = Name;
    using type = Type;
    using defs = neam::ct::type_list<Defs...>;
    static constexpr hash_t type_id = Type::hash;
  };

  /// \brief Runtime def of a pin
  struct pin_rt
  {
    hash_t type_id;
    std::string_view name;
  };

  /// \brief List of input pins
  template<typename... Types> class inputs;

  /// \brief List of output pins
  template<typename... Types> class outputs;

  /// \brief List of params
  template<typename... Types> class params;


  //
  // implementations
  //

  /// \brief Implementation of a pin. Handles most operations / operators automatically
  class pin_impl
  {
    public:
      
    private:
      
  };
} // namespace rukh
