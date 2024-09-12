# pragma once

#include <cstdlib>

namespace geni::mem {

    using Byte = char;
    using Size = std::size_t;

    template<mem::Size n>
    class Bytes {
      public:
        Bytes() : mFirst{'\0'}, mRest{'\0'} {}
        explicit
        Bytes( Byte fill ) : mFirst{fill}, mRest{fill} {}

        operator bool() const {return true;}

        void each( const std::function<void( Byte& )>& f )
        { f(first()); rest().each(f); }

        auto first() -> Byte& { return mFirst; }
        auto rest() -> Bytes<n-1>& {return mRest;}

      private:
        Byte mFirst;
        Bytes<n-1> mRest;
    };

    template<>
    class Bytes<1> {
      public:
        Bytes() : mFirst{'\0'} {}
        explicit
        Bytes( Byte fill ) : mFirst{fill} {}
        void each( const std::function<void( Byte& )>& f )
        { f(first()); }
        operator bool() const {return true;}
        Byte first() const { return mFirst; }
        auto rest() -> Bytes<0>& {return mRest;}
      private:
        Byte mFirst;
        Bytes<0> mRest;
    };


    template<>
    class Bytes<0> {
      public:
        operator bool() const {return false;}
        void each( const std::function<void( Byte& )>& f ) { return; }
    };

    template<typename Type>
    constexpr Type* NullInstance{ static_cast<Type*>(nullptr) };

    template<typename Type, Type::* MemberPtr>
    constexpr Size Offset{ reinterpret_cast<size_t>(NullInstance<Type>()->*MemberPtr) };

    template<typename Type, typename Member>
    inline
    auto ThisFrom( Member& member, Type::* memberPtr ) -> Type&
    {
        Type* instance{ reinterpret_cast<Type*>(reinterpret_cast<Byte*>(&member) - Offset<Type, memberPtr> ) };
        return *instance;
    }

    
}