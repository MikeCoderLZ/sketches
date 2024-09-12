#pragma once

#include "memory/Memory.hpp"

namespace geni::util {

    template<typename NativeType>
    union NativeBytes {
        NativeType native;
        mem::Bytes<sizeof(NativeType)> bytes;
        explicit
        NativeBytes( Byte fill ) : bytes{fill} {}
    }

}