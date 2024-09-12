#pragma once

#include "elements/MouseReactive.hpp"
#include "elements/DragItem.hpp"

namespace geni::elements {

  class DragProducer : IsMouseReactive {
    private:
      virtual
      auto getItem( const PeripheralContext& context ) -> DragItem =0;
      void mouseDown( const PeripheralContext& context,
                      MappedStorage& storage            ) override
      {
        storage.set( "drag::item", getItem(context) );
      }

  }

}