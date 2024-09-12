#pragma once

#include <csidlib>
#include <atomic>
#include <concepts>

#include "control/Loop.h"

namespace geni::mem {

  using Size = std::size_t;
  using Byte = unsigned char;

  namespace atomic {
    using Bool = std::atomic_bool;
  }

  class Pooled;

  template< typename Object, Size size >
  class Pool {
    public:
      using Ptr = Object*;

      Pool()
      { static_assert(std::derived_from<Object, Pooled> = true, "Classes used as the storage type parameter of geni::mem::Pool<> must implement the base class geni::mem::Pooled."); }

      template< typename... Args >
      Ptr construct( Args&&... args )
      {
        return new( seek()->object() ) Object( std::forward<Args>(args)... );
      }

      template< typename... Args >
      Ptr deferConstruction( Ptr constructor( Ptr, Args&... args ), Args&&... args )
      {
        return constructor( seek()->object(), std::forward<Args>(args)... );
      }

    private:
      friend Pooled;
      struct alignas(alignof(Object)) Slot {
          static
          auto external() -> Slot* { return new Slot(true, true); }
          static
          auto get( Object* object ) -> Slot*
          { return reinterpret_cast<Slot*>(object); }
          
          Slot() = default;

          bool tryOccupy()
          { bool occupy{false}; return mOccupied.compare_exchange_weak(occupy, true); }
          auto object() -> Object*
          { return reinterpret_cast<Object*>(mBytes); }

          bool isExternal()
          { return slot->mExternal.load() }

          void clear() { mOccupied.store(false); }

        private:
          Byte mBytes[sizeof(Object)];
          atomic::Bool mExternal;
          atomic::Bool mOccupied;

          Slot( bool external, bool occupied )
            : external{mExternal},
              occipied{mOccupied}
          {}

      };

      struct {
          Slot slots[size];
        private:
          Slot endSlot;
        public:
          Slot* end{&endSlot};
          Slot* last{slots};
      } mStore;

      auto seek() -> Slot*
      {
        bool found{false};
        bool secondTry{false};
        auto empty = mStore.last;
        auto last = mStore.end;
        SeekEmpty:
          ctrl::loop(
            size - (empty - mStore.slots),
            [this, &empty]() -> bool
            {
              if( (found = empty->tryOccupy()) ) {
                return false;
              }
              ++empty;
              return empty != mStore.end;
            }
          );
          if( found || secondTry ) goto Done;
          secondTry = true;
          empty = mStore.slots;
          last = mStore.last;
          goto SeekEmpty;
        
        Done:
          if( found ) {
            mStore.last = empty;
          } else {
            empty = Slot::external();
          }
          return empty;
      }

      static
      auto destruct( Ptr object )
      {
        Slot* slot{ Slot::get(object) };
        object->~Object();
        Pool<Object, size>::clear(slot);
      }

      static
      void clear( Slot* slot )
      {
        if( slot->isExternal() ) {
          delete slot;
          return;
        }
        slot->clear();
      }
  };

  // TODO: Add a debug-only check to make sure destruct() got called.
  // TODO: Use static duration?
  class Pooled {
    public:
      virtual
      void destruct() =0;
    protected:
      template<typename RealType>
      void destruct( RealType* object )
      { Pool<RealType>::destruct( object ); }
  };

}