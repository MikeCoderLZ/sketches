#pragma once

#include "algorithm/Iterable.h"

namespace geni::algo {

  template< typename IteratedObject >
  class ForEvery {
    public:
      static
      auto in( Iterable<IteratedObject>& iterable ) -> ForEvery
      { return ForEvery( iterable ); }
      void execute( std::function<void(IteratedObject)>& callback )
      {
        while(  )
      }
    private:
      Iterable<IteratedObject>& mIterable;
      explicit
      ForEvery( Iterable<IteratedObject>& iterable )
        : mIterable{iterable}
      {}

  };

  

}