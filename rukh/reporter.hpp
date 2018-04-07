//
// file : reporter.hpp
// in : file:///home/tim/projects/rukh/rukh/reporter.hpp
//
// created by : Timothée Feuillet
// date: dim. févr. 11 21:23:55 2018 GMT-0500
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

#include <string>
#include <string_view>

namespace rukh
{
  /// \brief Error / Warning reporter
  class reporter
  {
    public:
      /// \brief Make a context in the reporter. A context is an object on the stack that will hold some contextual information about the current node, the current script, ...
      struct context
      {
        context(reporter& _r) : r(_r) {/*TODO*/}
        ~context() {/*TODO*/}

        reporter& r;
      };

    public:
      enum class severity_t
      {
        debug,
        message,
        warning,
        error,
        critical // should only be used to specify unrecoverable errors where the compiler should die
      };

      // Serialization friendly & unformatted log entry:
      struct ser_log
      {
        severity_t severity;
        std::string message; // unformatted string

        // TODO
      };


      using handler_t = void(*)(/* TODO */);
      class handler_id
      {
        public:
          handler_id(handler_id &&o) : r(o.r), id(o.id) { o.id = 0; }
          handler_id &operator = (handler_id &&o)
          {
            if (&r == &o.r && this != &o)
            {
              unregister();
              id = o.id;
              o.id = 0;
            }
            return *this;
          }
          ~handler_id() { unregister(); }

          void unregister()
          {
            if (id)
              r.remove_handler(*this);
            id = 0;
          }
          bool is_registered() const { return id; }

        private:
          handler_id(reporter& _r, unsigned _id) : r(_r), id(_id) {}

        private:
          reporter& r;
          unsigned id = 0;
          friend reporter;
      };

      /// \brief Add a new handler. It will be called for every log events
      /// \note To remove the handler before the destruction of the reporter instance,
      ///       give the returned value to remove_handler.
      handler_id add_handler(handler_t handler);
      void remove_handler(const handler_id& id);

      /// \brief Log a message.
      /// Messages should have {}-style string formatting (with positional parameters using {n})
      /// \note The string is not formatted right away but values are copied and are sent to handlers in a specific format
      template<typename... Types>
      reporter& log(severity_t s, const std::string_view& msg, Types &&... values);
  };
} // namespace rukh
