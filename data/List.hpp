#pragma once

#include <algorithm>

#include "utility/Maybe.hpp"
#include "memory/Memory.hpp"

namespace geni::data::List {

  template< typename Object, typename Pool* = nullptr >
  class Forward {
    public:
      template<typename... Args>
      Forward( mem::Size count, Args&&... args )
      {}
      template<typename... Args>
      append( Args&&... args )
      {
        
      }
    private:
      class Node {
        public:
          template<typename... Args>
          Node( Node& next, Args&&...args )
            : mObject( std::forward<Args>(args)...),
              mNext{&next}
          {}
          template<typename... Args>
          Node( Args...args )
            : mObject( std::forward<Args>(args)...),
              mNext{nullptr}
          {}

          bool object( Object*& obj )
          { obj = &mObject; return  }

          bool next( Node*& next )
          { next = mNext, return mNext == nullptr; }

        private:
            Object mObject;
            Node* mNext;
      };

      class Handle {
        public:

          Handle() : mRoot{ util::Error::Empty } {}

          template<typename... Args>
          Handle(mem::Size count, Args&&... args )
            : mRoot{ std::forward<Args>(args)... }
          {

          }

          

          void 
        private:
          util::Maybe<Node> mRoot;
      }


  };
}