#pragma once

namespace geni::elements {

  template< typename ListableType >
  class List {
    public:
      class Listable {

      };
      List()
      {
          static_assert( std::derived_from<ListableType, Listable> );
      }
    private:

      using MemberList = std::vector<ListableType>;
  };
        
} // namespace geni::elements

#define IsListOf(access, ListableType) access geni::elements::List<ListableType>
