
#include <list>

#include "memory/SafeReference.hpp"

namespace geni {

    class Value {
        public:

            enum Type {
                isReal,
                isInteger,
                isBoolean
            };

            using Real = double;
            using Integer = int;
            using Boolean = bool;

            Value() = delete;
            Value( Real number ) : mType{isReal}, mValue{ number } {}
            Value( Integer number ) : mType{isInteger}, mValue{ number } {}
            Value( Boolean value ) : mType{isBoolean}, mValue{ value } {}
            ~Value() = default; // Modify in case a class comes into play

            auto operator =( const Value& other ) -> Value&
            {
                if( *this != other && mType != other.mType ) {
                    switch(mType) {
                        case isReal:
                            mValue.clearReal(); break;
                        case isInteger:
                            mValue.clearInteger(); break;
                        case isBoolean:
                            mValue.clearBoolean(); break;
                        default:
                            break;
                    }
                    mType = other.mType;
                    notifyType(mType);
                }
                assignSameType(other);
                return *this;
            }

            bool operator ==( const Value& other ) const
            {
                bool equivalent{false};
                if( this == &other ) {
                    equivalent = true;
                } else {
                    if( mType == other.mType ) {
                        switch( mType ) {
                            case isReal:
                                equivalent = mValue.real() == other.mValue.real(); break;
                            case isInteger:
                                equivalent = mValue.integer() == other.mValue.integer(); break;
                            case isBoolean:
                                equivalent = mValue.boolean() == other.mValue.boolean(); break;
                            default:
                                break;
                        }
                    }
                }
                return equivalent;
            }

            void addCallback( std::function<void(const Value&)>&& callback )
            {
                mCallbacks.emplace_back( std::move(callback) );
                mCallbacks.last()(*this);
            }
        private:

            union MultiVal {
                public:
                    MultiVal() = delete;
                    MultiVal( Real real ) : mReal{real} {}
                    MultiVal( Integer integer ) : mInteger{integer} {}
                    MultiVal( Boolean boolean ) : mBoolean{boolean} {}
                    ~MultiVal() {}
                    
                    auto real() const -> Real { return mReal; }
                    auto integer() const -> Integer { return mInteger; }
                    auto boolean() const -> Boolean { return mBoolean; }

                    auto real() -> Real& { return mReal; }
                    auto integer() -> Integer& { return mInteger; }
                    auto boolean() -> Boolean& { return mBoolean; }

                    void clearReal() {}
                    void clearInteger() {}
                    void clearBoolean() {}

                private:
                    double  mReal;
                    // Complex mComplex;
                    int     mInteger;
                    bool    mBoolean;
            } mValue;

            using Callback = std::function<bool(const Value&)>
            using CallbackList = std::list<Callback>;

            Type        mType;
            Callback    mCallbacks;

            void assignSameType( const Value& other ) {
                bool changing{false};
                switch(mType) {
                    case isReal:
                        changing = mValue.real() != other.mValue.real();
                        mValue.real() = other.mValue.real(); break;
                    case isInteger:
                        changing = mValue.integer() != other.mValue.integer();
                        mValue.integer() = other.mValue.integer(); break;
                    case isBoolean:
                        changing = mValue.boolean() != other.mValue.boolean();
                        mValue.boolean() = other.mValue.boolean(); break;
                    default:
                        break;
                }
                if(changing) {
                    notifyValue();
                }
            }

            void notifyType()
            {
                #ifdef GENI_DEBUG
                    static std::atomic_flag recursed;
                    geniAssert( !recursed.test_and_set() );
                #endif

                auto listener{ mListeners.begin() };
                auto end{ mListeners.end() };
                for( ; listener != end; ) {
                    if( *listener ) {
                        listener->changed(mType);
                        ++listener
                    } else {
                        auto deadListener{ listener };
                        ++listener;
                        mListeners.erase(deadListener);
                    }
                }
                #ifdef GENI_DEBUG
                    recursed.clear();
                #endif
            }

            void notifyValue()
            {
                #ifdef GENI_DEBUG
                    static std::atomic_flag recursed;
                    geniAssert( !recursed.test_and_set() );
                #endif

                auto callback{ mCallbacks.begin() };
                auto end{ mCallbacks.end() };
                for( ; callback != end; ) {
                    if( (*callback)(*this) ) {
                        ++callback;
                    } else {
                        auto deadCallback{ callback };
                        ++callback;
                        mCallbacks.erase(deadCallback);
                    }
                }
                #ifdef GENI_DEBUG
                    recursed.clear();
                #endif
            }
    };
}