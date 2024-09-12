#pragma once

#include "elements/Bounded2D.hpp"

namespace geni::elements {

    class GestureReactive : IsBounded2D {
      public:
        GestureReactive() = default;
        virtual
        ~GestureReactive() = default;
      protected:
        virtual
        void tapped( const PeripheralContext& context,
                      MappedStorage&          storage ) {}
        virtual
        void doubleTapped( const PeripheralContext& context,
                            MappedStorage&          storage ){}
        virtual
        void tripleTapped( const PeripheralContext& context,
                            MappedStorage&          storage ){}
        virtual
        void pressed( const PeripheralContext&  context,
                      MappedStorage&            storage ) {}
        virtual
        void longPress( const PeripheralContext& context,
                        MappedStorage&           storage ) {}
      private:
          // Magic
    };

} // namespace genie::elements

#define ReactsToGestures public geni::elements::GestureReactive
#define ReactsToGestures(access) access geni::elements::GestureReactive
