#pragma once

#include "elements/DragItem.hpp"

namespace geni::elements {

  class DragReciever {
    private:
      virtual
      void itemEntered( const DragItem& item ) =0;
      virtual
      void itemExited( const DragItem& item ) =0;
      virtual
      void acceptsItem( const DragItem& item ) =0;
      virtual
      void itemDropped( const DragItem& item ) =0;
  }

}