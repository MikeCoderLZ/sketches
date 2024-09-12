#pragma once

#include <functional>

namespace geni::elements {

  class Selectable {
    public:
      enum State {
        notSelected = 0x0,
        selected = 0x1
      }

      using Callback = std::function<void()>;

      Selectable() : mState{ notSelected } {}

      void select();
      void deselect();
      void selectedCallback( Callback&& callback );
      void deselectedCallback( Callback&& callback );
      bool is( State state );
    
    private:
      State mState;
      Callback mSelect;
      Callback mDeselect;
  };

  inline
  void Selectable::select()
  {
    mState |= selected;
    mSelect();
  }

  inline
  void Selectable::deselect()
  {
    mState &= notSelected;
    mDeselect();
  }

  inline
  bool Selectable::is( State state )
  { return mState & state != 0; }
}