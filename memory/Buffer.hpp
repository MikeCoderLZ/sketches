#pragma once

#include "memory/Memory.hpp"

namespace geni::mem {

  template< Size N >
  class Buffer {
    public:
      Buffer() : mBytes{} {}
    private:
      Byte mBytes[N];
      
  };
}