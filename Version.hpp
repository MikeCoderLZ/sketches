#pragma once

namespace geni::util {

  template<typename VersionSource>
  class Version {
    public:
      template< typename Derived >
      static
      auto generate( const VersionSource& source ) -> Derived
      {
        static_assert(std::derived_from<Version, Derived> == true>;
        Derived derived;
        derived.initInternal(source);
        return derived;
      }
      [[nodiscard]]
      int getMajor() const { return mMajor; }
      [[nodiscard]]
      int getMinor() const { return mMinor; }
      [[nodiscard]]
      int getRevision() const { return mRevision; }
      [[nodiscard]]
      auto toString() const -> std::string_view { return mString; }
    protected:
      Version()
        : mMajor{0}, mMinor{0}, mRevision{0}, mString{"v"}
      {}
      void setMajor( int major )
      { mMajor = major; }
      void setMinor( int minor )
      { mMinor = minor; }
      void setRevision( int revision )
      { mRevision = revision; }
    private:
      int mMajor;
      int mMinor;
      int mRevision;
      std::string mString;

      virtual
      void init( const VersionSource& source ) =0;
      void internalInit( const VersionSource& source )
      {
        init( source );
        mString += mMajor;
        mString += ".";
        mString += mMinor;
        mString += ".";
        mString += mRevision;
      }
  }
}
