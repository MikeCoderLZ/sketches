#pragma once

#include <list>

namespace geni::relation {

  class Requirement;

  template< typename RequiredType >
  class Requires {
    public:
      using RequirementRef = ref::Shared<RequiredType>;
      using Callback = std::function<bool( const RequiredType& )>;

      Requires( RequirementRef requirement )
        : exclusive{ std::move(requirement) }
      {
        static_assert( std::is_derived_from<RequiredType, Requirment<RequiredType>> );
        exclusive.mRequirement->setDependency( *this );
      }

      void onChange( Callback&& callback )
      { exclusive.mCallbacks.emplace_back( callback ); }

    private:
      friend Requirement; // There's template stuff to this, not worried

      using CallbackList = std::list<Callback>;
      
      class {
        friend Requires;
        RequirementRef mRequirement;
        CallbackList mCallbacks;
      } exclusive;

      void update()
      {
        const RequiredType& requirement{*exclusive.mRequirement};
        for( auto& callback : exclusive.mCallbacks ) {
            // Actually remove callbacks that return false, indicating
            // no further callbacks are needed.
          callback( requirement );
        }
      }

  };

  template< typename ThisType >
  class Requirement {
    protected:
      void requirementChanged()
      { if(mDependency) mDependency->update(); }
    private:
      friend template Requires<ThisType>;
      class {
        friend Requirement;
        ref::Weak<Requires<ThisType>> mDependency;
      } exclusive;

      setDependency( Requires<ThisType>& dependency )
      {
        exclusive.mDependency = ref::makeWeak( dependency );
      }
  };
}