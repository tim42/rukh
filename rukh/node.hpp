//
// file : node.hpp
// in : file:///home/tim/projects/rukh/rukh/node.hpp
//
// created by : Timothée Feuillet
// date: dim. févr. 11 21:09:47 2018 GMT-0500
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

#include <string_view>
#include <vector>
#include <tools/ct_list.hpp>
#include "reporter.hpp"
#include "pin.hpp"

namespace rukh
{
  class pin_impl;
  class param_impl;

  /// \brief A base AST node
  class base_node
  {
    protected:
      base_node() noexcept = default;
      virtual ~base_node() noexcept = default;

    public:
      /// \brief Return the name of the node
      virtual std::string_view get_name() const = 0;
      virtual std::string_view get_description() const = 0;

    public: // node infos
      /// \brief Return the list of input pins
      virtual std::vector<pin_rt> get_input_pins() const = 0;

      /// \brief Return the list of output pins
      virtual std::vector<pin_rt> get_output_pins() const = 0;

      /// \brief Return the list of params
      virtual std::vector<pin_rt> get_params() const = 0;

    public: // generate
      /// \brief Called so that the node implementation will define the output types from the input types
      /// Input types are defined at this point
      virtual bool resolve_output_types(reporter& r) = 0;

      /// \brief Called after outputs resolve when further validation is needed
      virtual bool validate(reporter& r) const = 0;

      /// \brief Is the node fully constant ?
      virtual bool is_constant() const = 0;

      /// \brief Called independently of is_constant. The purpose of this function is to set output constants.
      virtual void const_generate(reporter& r) = 0;

      /// \brief Generate IR for the current node. Will not be called if is_constant() returns true
      virtual bool generate(reporter& r/*, generator& g*/) const = 0;
  };

  /// \brief A statically defined AST node. Will perform most actions automatically.
  /// The goal of this class is to provide an optimized and easy way to generate code for an AST node. Unless your node is dynamic, you should use this class.
  /// \tparam Child the class that inherit from this class
  /// \tparam Name a name as returned by rk_str
  template
  <
    typename Child, // CRTP
    typename Name,  // rk_str
    typename InputPins, // inputs < pin<...>, ...>
    typename OutputPins, // outputs < pin<...>, ...>
    typename Params // params < pins<...>, ...>
  >
  class node : base_node
  {
    protected:
      node() noexcept = default;
      virtual ~node() noexcept = default;

    protected: // utilities
      /// \brief Access an input pin. Will generate a compilation error if the pin is not defined
      template<typename PinName>
      const pin_impl& input();

      /// \brief Access an output pin. Will generate a compilation error if the pin is not defined
      template<typename PinName>
      pin_impl& output();

      /// \brief Access a parameter. Will generate a compilation error if the parameter is not defined
      template<typename ParamName>
      const param_impl& param();

    public: // implems of base_node
      std::string_view get_name() const final { return Name::array; };
      std::string_view get_description() const final { return Child::description; };

    private: // node infos helpers
      template<typename... Pins> struct pins_to_array { static constexpr pin_rt array[sizeof...(Pins)] = {{Pins::type_id, Pins::name::array}..., }; };

    public: // implems of base_node / node infos
      std::vector<pin_rt> get_input_pins() const final {return neam::ct::list::extract<InputPins>::template as<pins_to_array>::array;}
      std::vector<pin_rt> get_output_pins() const final {return neam::ct::list::extract<OutputPins>::template as<pins_to_array>::array;}
      std::vector<pin_rt> get_params() const final {return neam::ct::list::extract<Params>::template as<pins_to_array>::array;}
      
  };
} // namespace rukh
