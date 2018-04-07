//
// file : type_identity.hpp
// in : file:///home/tim/projects/rukh/rukh/type_identity.hpp
//
// created by : Timothée Feuillet
// date: sam. avr. 7 12:06:38 2018 GMT-0400
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

#include <functional>

namespace rukh
{
  struct type_id
  {
    const void* id;

    constexpr bool operator == (const type_id& o) { return id == o.id; }
    constexpr bool operator != (const type_id& o) { return id != o.id; }
    constexpr bool operator < (const type_id& o) { return std::less<decltype(id)>{}(id, o.id); }
    constexpr bool operator > (const type_id& o) { return std::greater<decltype(id)>{}(id, o.id); }
    constexpr bool operator <= (const type_id& o) { return std::less_equal<decltype(id)>{}(id, o.id); }
    constexpr bool operator >= (const type_id& o) { return std::greater_equal<decltype(id)>{}(id, o.id); }
  };

  template<typename Type>
  struct type_identity
  {
    private:
      static constexpr unsigned xid = 0;
      static constexpr const void* make_id()
      {
        return &xid;
      }

    public:
      static constexpr type_id id = {make_id()};
  };
} // namespace rukh
