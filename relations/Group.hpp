#pragma once

#include <list>

namespace geni::relation {

  template< typename GroupedType >
  class Group {
    public:
      using Processor = std::function<void(GroupedType&)>;

      Group( const std::string& name )
        : mMeta{ name }
      {}
      Group( std::string_view name )
        : mMeta{ name }
      {}
      add( GroupedType& newMember );

      forEach( Processor processor );
      
    private:
      class Meta {
        public:
          Meta( std::string name ) : mName{ std::move(name) } {}
          virtual
          ~Meta() = default;
        protected:
          // TODO: Define Storage
          Storage mStorage;
        private:
          std::string mName;
      };

      Meta mMeta;
      using GroupList = std::list<GroupedType>;

      GroupList mMembers;
  };


}