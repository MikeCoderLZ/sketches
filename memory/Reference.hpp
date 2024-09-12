#pragma once

namespace geni::mem {

  template< typename Object >
  class Reference {
    public:

      template< typename ...Args >
      Reference( Args&&... args )
        : mState{ State::occupied },
          mBlock{ std::forward<Args>(args)... }
      {

      }

      [[nodiscard]]
      operator bool() const

      
      static
      auto construct( Args&&... args ) -> Reference
      { return  }

      template< typename ReturnType, typename ...Args >
      call( ReturnType (Object::* member)(Args... args), Args... args )
      {
        if( *this )
          mBlock.object.*member(args...);
      }

    private:

      enum State {
        occupied,
        empty
      } mState;

      struct Empty {
        unsigned char bytes[sizeof(Object)];
      };

      union Block {
        public:
          static
          auto empty() -> Block { return Block<Empty>(); }
          template< typename ...Args >
          Block( Args&&... args )
            : object{ std::forward<Args>(args)... }
          {}

          Object object;
          Empty blank;
        private:
          template<typename Type>
          Block() : blank{ '\0' }
          {}
      } mBlock;

      explicit
      Reference( Ptr object ) : mObject{object} {};

      template< typename ...Args >
      static
      auto allocate( Args&&... args ) -> Ptr
      { return new Object( std::forward<Args>(args)... ); }

      void deallocate() {
        if( mObject != nullptr ) {
          delete mObject;
        }
      }
  };
}