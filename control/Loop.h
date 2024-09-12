#pragma once

namespace geni::ctrl {

  using Count = unsigned long int;
  using Callback = std::function<void()>;
  using BreakingCallback = std::function<bool()>;

  template< Count loops >
  void loop( Callback& callback )
  { callback(); loop<loops-1>(callback); }

  template<>
  void loop<1UL>( Callback& callback )
  { callback(); }

  template< Count loops >
  void loop( std::function<void(Count)>& callback )
  { callback(loops); loop<loops-1>(callback); }

  template<>
  void loop<1UL>( std::function<void(Count)>& callback )
  { callback(1UL); }

  template< Count loops >
  void loop( BreakingCallback& callback )
  { callback() ? loop<loops-1>(callback) : return; }

  template<>
  void loop<1UL>( BreakingCallback& callback )
  { callback(); }

  void loop( Count count, const Callback& callback )
  {
    switch( count % 16U ) {
      case 0:   callback(); --count;
      case 15:  callback(); --count;
      case 14:  callback(); --count;
      case 13:  callback(); --count;
      case 12:  callback(); --count;
      case 11:  callback(); --count;
      case 10:  callback(); --count;
      case 9:   callback(); --count;
      case 8:   callback(); --count;
      case 7:   callback(); --count;
      case 6:   callback(); --count;
      case 5:   callback(); --count;
      case 4:   callback(); --count;
      case 3:   callback(); --count;
      case 2:   callback(); --count;
      case 1:
        callback(); if( --count != 0U ) loop( count, callback );
      default:
        return;
    }
  }

  void loop( Count count, const BreakingCallback& callback )
  {
    switch( count % 16U ) {
      case 0:   !callback() ? return : --count;
      case 15:  !callback() ? return : --count;
      case 14:  !callback() ? return : --count;
      case 13:  !callback() ? return : --count;
      case 12:  !callback() ? return : --count;
      case 11:  !callback() ? return : --count;
      case 10:  !callback() ? return : --count;
      case 9:   !callback() ? return : --count;
      case 8:   !callback() ? return : --count;
      case 7:   !callback() ? return : --count;
      case 6:   !callback() ? return : --count;
      case 5:   !callback() ? return : --count;
      case 4:   !callback() ? return : --count;
      case 3:   !callback() ? return : --count;
      case 2:   !callback() ? return : --count;
      case 1:
        !callback() ? return : if( --count != 0U ) loop( count, callback );
      default:
        return;
    }
  }

}