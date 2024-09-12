#pragma once

#include "elements/Bounded2D.hpp"

namespace geni::elements {

  class MouseReactive : IsBounded2D {
    public:
      MouseReactive() = default;
      virtual
      ~MouseReactive() = default;
    protected:
      virtual
      void mouseDown( const PeripheralContext&  context,
                      MappedStorage&            storage ) {}
      virtual
      void mouseUp( const PeripheralContext&  context,
                    MappedStorage&            storage ) {}
      virtual
      void mouseEnter( const PeripheralContext& context,
                      MappedStorage&            storage ) {}
      virtual
      void mouseExit( const PeripheralContext&  context,
                      MappedStorage&            storage ) {}
      virtual
      void clicked( const PeripheralContext&  context,
                    MappedStorage&            storage ) {}
      virtual
      void doubleClicked( const PeripheralContext&  context,
                          MappedStorage&            storage ) {}
      virtual
      void tripleClicked( const PeripheralContext&  context,
                          MappedStorage&            storage ) {}
    private:
      // Magic
  };

} // namespace genie::elements

#define ReactsToMouse public geni::elements::MouseReactive
#define ReactsToMouse(access) access geni::elements::MouseReactive
