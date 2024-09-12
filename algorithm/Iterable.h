#pragma once

namespace geni::algo {

  template<typename I, typename Object>
  concept CanIterate = requires( I mtbl, const I cnst )
  {
    cnst.operator bool();
    { mtbl.operator ++() } -> Iterator&;
    { mtbl.operator ++(int) } -> Iterator;
    { cnst.operator *() } -> std::convertible_to<const Object&>;
    { mtbl.operator *() } -> std::convertible_to<Object&>;
    { cnst.operator ->() } -> std::convertible_to<const Object*>;
    { mtbl.operator ->() } -> std::convertible_to<Object*>;
  }

  template< typename Object, CanIterate<Object> IteratorType >
  class Iterable {
    public:
      virtual auto begin() -> IteratorType =0;
      virtual auto end() -> IteratorType =0;
  };

} // namespace geni::algo