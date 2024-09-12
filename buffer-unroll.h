
#include <memory>
#include <functional>
#include <algorithm>

#include "juce_PlatformDefs.h"

#if defined( RED_DEBUG )
  #define redAssertfalse do{ /*break code*/ } while (false)
#else
  #define redAssertfalse do{(void)0;} while (false)
#endif

#if defined( RED_DEBUG )
  #define redAssert( constraint, message ) do{std::cerr << (message) << std::endl; red_debugbreak; } while (false)
#else 
  #define redAssert( constraint, message ) do{(void)0;} while (false)
#endif

namespace red {

    struct Loop {

      static void unroll( size_t count,
                          std::function<void(size_t)> action );
      static void unroll( size_t start,
                          size_t count,
                          std::function<void(size_t)> action );
      static void unroll( size_t start,
                          size_t count,
                          size_t increment,
                          std::function<void(size_t)> action );

      template< size_t count, size_t start = 0U, size_t increment = 1U>
      static void unroll( std::function<void(size_t)> action );
  };

  inline void Loop::unroll( size_t count,
                            std::function<void(size_t)> operation )
  {
    for ( size_t i = 0U, indexer = 0U;
          i < count;
          indexer = ((indexer + 16U) % count) & 15U ) { 
          switch(indexer) {
            case 0:  operation(i); ++i;
            case 1:  operation(i); ++i;
            case 2:  operation(i); ++i;
            case 3:  operation(i); ++i;
            case 4:  operation(i); ++i;
            case 5:  operation(i); ++i;
            case 6:  operation(i); ++i;
            case 7:  operation(i); ++i;
            case 8:  operation(i); ++i;
            case 9:  operation(i); ++i;
            case 10: operation(i); ++i;
            case 11: operation(i); ++i;
            case 12: operation(i); ++i;
            case 13: operation(i); ++i;
            case 14: operation(i); ++i;
            case 15: operation(i); ++i;
            default: break;
          }
    }
  }

  inline void Loop::unroll( size_t count,
                            size_t start,
                            std::function<void(size_t)> operation )
  {
    size_t limit {count + start};
    for ( size_t i = start, indexer = 0U;
          i < limit;
          indexer = ((indexer + 16U) % count) & 15U ) { 
          switch(indexer) {
            case 0:  operation(i); ++i;
            case 1:  operation(i); ++i;
            case 2:  operation(i); ++i;
            case 3:  operation(i); ++i;
            case 4:  operation(i); ++i;
            case 5:  operation(i); ++i;
            case 6:  operation(i); ++i;
            case 7:  operation(i); ++i;
            case 8:  operation(i); ++i;
            case 9:  operation(i); ++i;
            case 10: operation(i); ++i;
            case 11: operation(i); ++i;
            case 12: operation(i); ++i;
            case 13: operation(i); ++i;
            case 14: operation(i); ++i;
            case 15: operation(i); ++i;
            default: break;
          }
    }
  }

  inline void Loop::unroll( size_t count,
                            size_t start,
                            size_t increment,
                            std::function<void(size_t)> operation )
  {
    redAssert( count % increment == 0, "Increment for loop will result in unsigned int underflow." );

    size_t limit {count + start};
    for ( size_t i = start, indexer = 0U;
          i < limit;
          indexer = ((indexer + 16U) % count) & 15U ) { 
          switch(indexer) {
            case 0:  operation(i); i += increment;
            case 1:  operation(i); i += increment;
            case 2:  operation(i); i += increment;
            case 3:  operation(i); i += increment;
            case 4:  operation(i); i += increment;
            case 5:  operation(i); i += increment;
            case 6:  operation(i); i += increment;
            case 7:  operation(i); i += increment;
            case 8:  operation(i); i += increment;
            case 9:  operation(i); i += increment;
            case 10: operation(i); i += increment;
            case 11: operation(i); i += increment;
            case 12: operation(i); i += increment;
            case 13: operation(i); i += increment;
            case 14: operation(i); i += increment;
            case 15: operation(i); i += increment;
            default: break;
          }
    }
  }

  template< size_t count, size_t start, size_t increment>
  inline void Loop::unroll<start, count, increment>( std::function<void(size_t)> action )
  {
    static_assert( count % increment == 0, "Increment for loop will result in unsigned int underflow." );

    Loop::unroll<count-1U, start, increment>( action );
    action( start + count );
  }

  template< size_t start, size_t increment>
  inline void Loop::unroll<0, start, increment>( std::function<void(size_t)> action )
  { action( start ); }

  namespace Audio {
    template< size_t CHN, typename S>
    class Buffer {
      public:
        typedef S sample_t;
        constexpr static channelCount = CHN;
        Buffer() {}

        explicit Buffer( size_t size )
        : mSize {size}
        {
          Loop::unroll<channelCount>(
            [this]
            (size_t chn)
            { mChannels[chn] = std::make_unique<sample_t[]>(mSize); }
          );
        }

        Buffer( const Buffer<channelCount,S>& other )
          : mSize {other.mSize}
        {
          Loop::unroll<channelCount>(
            [this, &other]
            (size_t chn)
            {
              mChannels[chn] = std::make_unique<sample_t[]>(mSize);
              sample_t* chanA {mChannels[chan].get()};
              sample_t* chanB {other.mChannels[chan].get()};

              Loop::unroll(
                mSize,
                [chanA, chanB]
                (size_t smp)
                { chanA[smp] = chanB[smp]; }
              );
            }
          );
        }

        Buffer( Buffer&& other )
          : mSize { other.mSize }
        {
          Loop::unroll<channelCount>(
            [this, &other]
            (size_t chn)
            { mChannels[chn].reset(other.mChannels[chn].release()); }
          );
        }

        sample_t operator() ( size_t channel, size_t sample ) const
        {
          jassert( channel < channelCount );
          jassert( sample < mSize );

          return mChannels[channel][sample];
        }

        sample_t& operator() ( size_t channel, size_t sample )
        {
          jassert( channel < channelCount );
          jassert( sample < mSize );

          return mChannels[channel][sample];
        }

        void copy( size_t     channel,
                   size_t     number,
                   sample_t[] source  )
        {
          #if defined(DEBUG)
            sample_t lastSample = source[ number-1 ];
            ++lastSample;
          #endif

          jassert( channel < channelCount );
          jassert( number <= mSize );

          size_t limit { std::min( number, mSize ) };

          sample_t* channel = mChannels[channel].get();

          dynamicuUnroll( limit, [channel, source]( size_t smp ){ channel[smp] = source[smp]; } );
        }

        void copy( size_t                 channel,
                   size_t                 number,
                   std::vector<sample_t>& source  )
        {
          #if defined(DEBUG)
            sample_t lastSample = source[ number-1 ];
            ++lastSample;
          #endif

          jassert( channel < channelCount );
          jassert( number <= mSize );

          size_t limit { std::min( number, mSize ) };

          sample_t* channel = mChannels[channel].get();
          sample_t* sourcePtr = source.data();

          dynamicUnroll( limit, [channel, sourcePtr]( size_t smp ){ channel[smp] = source[smp]; } );
        }

        void copy( size_t     channel,
                   size_t     number,
                   size_t     destOffset,
                   sample_t[] source  )
        {
          #if defined(DEBUG)
            sample_t lastSample = source[ number-1 ];
            ++lastSample;
          #endif

          jassert( channel < channelCount );
          jassert( destStart + number < mSize );

          sample_t* channel = mChannels[channel].get();

          int limit = std::min( number, mSize - destOffset );

          dynamicUnroll( limit, destOffset [channel, sourcePtr]( size_t smp ){ channel[smp] = source[smp]; } );
        }

        void copy( size_t                 channel,
                   size_t                 number,
                   size_t                 offset
                   std::vector<sample_t>& source    )
        {
          #if defined(DEBUG)
            sample_t lastSample = source[ number-1 ];
            ++lastSample;
          #endif

          jassert( channel < channelCount );
          jassert( offset + number < mSize );

          sample_t* channel = mChannels[channel].get();

          int limit = std::min( offset+ number, mSize );

          for ( int destSmp = offset, srcSmp = 0;
                destSmp < limit;
                ++destSmp, ++srcSmp                   ) {
            channel[destSmp] = source[srcSmp];
          }
        }

      private:
        size_t mAllocSize;
        size_t mSize;
        std::unique_ptr<sample_t[]> mChannels[channelCount];
    };
  }

} // namespace red