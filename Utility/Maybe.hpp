#pragma once

namespace geni::util {

    class Error;

    enum Anything {
        Nothing,
        Something
    };

    template< typename Object >
    class Maybe {
        public:
        
            template< typename... Args >
            static
            something( Args&&... args ) -> Maybe
            { return Maybe{ Something, std::forward<Args>(args)...}; }

            template< typename... Args >
            static
            nothing( Error&& error ) -> Maybe
            { return Maybe{ Nothing, std::forward(error) }; }

            Maybe( const Maybe& other )
              : mAnything{ other.mAnything },
                mHandle{ mAnything == Something
                         ? Handle{other.mHandle.object()}
                         : Handle{other.mHandle.error()}  }
            {}

            Maybe( Maybe&& other )
              : mAnything{ other.mAnything },
                mHandle{ mAnything == Something
                         ? Handle{std::move(other.mHandle.object())}
                         : Handle{std::move(other.mHandle.error())}  }
            {}

            ~Maybe()
            {
              switch(mAnything) {
                case Something: mHandle.destructObject(), break;
                case Nothing: default: mHandle.destructError(), break;
              }
            }

            template< typename... Args >
            auto reset( Args&&... args ) -> Maybe&
            {
              // This assignment does not use copy assignment.
              // You get a destructor call and a new contruction.
              if( *this ) {
                mHandle.destructObject();
                mHandle.set( std::forward<Args>(args)... );
              } else {
                mHandle.destructError();
                mHandle.set( std::forward<Args>(args)... );
              }
              return *this;
            }

            auto operator =( Error& error ) -> Maybe&
            {
              if( *this ) {
                // Don't destruct; assignment should be transparent
                mHandle.object() = Object( std::forward<Args>(args)... );
              } else {
                mHandle.destructError();
                mHandle.set( std::forward<Args>(args)... )
              }
              return *this;
            }

            auto operator =( const Maybe& other ) -> Maybe&
            {
              if( *this ) {
                if( other ) {
                  // Don't destruct; assignment should be transparent
                  mHandle.object() = other.mHandle.object();
                } else {
                  mHandle.destructObject();
                  mHandle.set(other.mHandle.error());
                }
              } else {
                if( other ) {
                  mHandle.destructError();
                  mHandle.set(other.mHandle.object());
                } else {
                  // Don't destruct; assignment should be transparent
                  mHandle.error() = other.mHandle.error();
                }
              }
              return *this;
            }

            operator bool() const
            { return mAnything == Something; }

            operator Anything() const
            { return mAnything; }

            auto operator *() const -> const Object&
            {
                // TODO: Add debug check on mAnything member
                return mHandle.object();
            }

            auto operator *() -> Object&
            {
                // TODO: Add debug check on mAnything member
                return mHandle.object();
            }

            auto operator ->() const -> const Object*
            {
                // TODO: Add debug check on mAnything member
                return &mHandle.object();
            }

            auto operator ->() -> Object*
            {
                // TODO: Add debug check on mAnything member
                return &mHandle.object();
            }

        private:
            Anything mAnything;
            union Handle {
                public:

                    Handle( const Object& object )
                      : mObject{ object }
                    {}

                    Handle( Object&& object )
                      : mObject{ std::move(object) }
                    {}

                    template< typename... Args >
                    Handle( Args&&... args )
                      : mObject{std::forward<Args>(args)...}
                    {}

                    Handle( const Error& error ) : mError{error} {}

                    ~Handle() = default;

                    void destructObject() { (&mObject)->~Object(); }
                    void destructError() { (&mError)->~Error(); }

                    auto object() const -> Object&
                    { return mObject; }

                    auto error() const -> Error&
                    { return mError; }

                    template< typename... Args >
                    void set( Args&&... args )
                    {
                      auto ptr{&mObject};
                      ptr = new(ptr)(std::forward<Args>(args)...);
                    }

                    void set( const Object& object )
                    {
                      auto ptr{&mObject};
                      ptr = new(ptr)(object);
                    }

                    void set( const Error& error )
                    {
                      auto ptr{&mError};
                      ptr = new(ptr)(error);
                    }
                private:
                    Object mObject;
                    Error  mError;
            };

            Handle mHandle;

            template< typename... Args >
            Maybe( Anything anything, Args&&... args )
              : mAnything{anything},
                mHandle{std::forward<Args>(args)...}
            {}
    };
}