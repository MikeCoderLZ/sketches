#pragma once

namespace geni::prim {

  class Boolean {
    public:
      static
      auto True() -> Boolean { return Boolean(true); }
      static
      auto False() -> Boolean { return Boolean(false); }

      Boolean() = delete;
      
      Boolean( const Boolean& other )
        : mTruth{other.mTruth}
      {}

      Boolean( Boolean&& other )
        : mTruth{other.mTruth}
      {}

      auto operator =( const Boolean& other ) -> Boolean&
      {
        mTruth = other.mTruth;
        return *this;
      }

      auto operator =( bool value ) -> Boolean&
      {
        mTruth = value;
        return *this;
      }

      operator bool() const
      { return mTruth; }
    private:
      bool mTruth;

      explicit
      Boolean( bool truth )
        : mTruth{ truth }
      {}
  }
} // namespace genie::prim