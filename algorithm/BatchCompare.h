#pragma once

namespace geni::algo {

  template< typename ComparedType, typename... ArgsRest >
  bool equalToAny( ComparedType value, ComparedType arg, ArgsRest... argsRest)
  { return value == arg || euqalToAny( value, argsRest... ); }

  template< typename ComparedType>
  bool equalToAny( ComparedType value, ComparedType arg)
  { return value == arg; }

  template< typename ComparedType, typename... ArgsRest >
  bool equalToAny( ComparedType& value, ComparedType& arg, ArgsRest&&... argsRest)
  { return value == arg || euqalToAny( value, std::forward<ArgsRest>(argsRest)... ); }

  template< typename ComparedType>
  bool equalToAny( ComparedType& value, ComparedType& arg)
  { return value == arg; }


}