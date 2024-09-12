///////////////////// control/red_staticAssertion.h /////////////////////////

#pragma once

#include <iostream>

#include "<debug trap header>"

namespace red {

  struct Assert {

    void compiletime( constexpr bool constraint, const char* message )
    {
      #if defined(DEBUG)
        static_assert( constraint, message );
      #endif
    }

    void runtime( bool constraint, const char* message )
    {
      std::cerr << message << std::endl;
      #if defined(DEBUG)
        if( ! constraint ) {
          // Check the standard output for more information.
          // call debug trap
        }
      #endif
    }
  };

} // namespace red

#if defined( RED_DEBUG )
  #define redAssertfalse do{ /*break code*/ } while (false)
#else
  #define redAssertfalse do{(void)0;} while (false)
#endif

#if defined( RED_DEBUG )
  #define redAssertion( constraint, message ) do{std::cerr << (message) << std::endl; red_debugbreak; } while (false)
#else 
  #define redAssertion( constraint, message ) do{(void)0;} while (false)
#endif

/////////////////////////////////////////////////

#pragme once

#include <cstddef>
#include <functional>

#include "control/red_assertion.h"

namespace red {

  struct Loop {

      static void unroll( size_t count,
                          std::function<void(size_t)> action );
      static void unroll( size_t start,
                          size_t count,
                          std::function<void(size_t)> action );
      static void unroll( size_t start,
                          size_t count,
                          size_t increment,
                          std::function<void(size_t)> action );

      template< size_t count, size_t start = 0U, size_t increment = 1U>
      static void unroll( std::function<void(size_t)> action );
  };

  template< size_t count, size_t start, size_t increment>
  inline void Loop::unroll<start, count, increment>( std::function<void(size_t)> action )
  {
    Assert::compiletime( count % increment == 0, "Increment for loop will result in unsigned int underflow." );

    Loop::unroll<count-1U, start, increment>( action );
    action( start + count );
  }

  template< size_t start, size_t increment>
  inline void Loop::unroll<0, start, increment>( std::function<void(size_t)> action )
  { action( start ); }

}