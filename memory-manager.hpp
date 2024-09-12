#include <cstdlib>

namespace memory {

    template< typename AllocatorType, typename AllocatedType >
    concept bool DiscreteAllocator() {
        return requires( AllocatorType& allocator, AllocatedType* allocation ) {
            { allocator.allocate() } -> AllocatedType*;
            { allocator.release( allocation ) };
        };
    }

    template< typename AllocatorType, typename AllocatedType >
    concept bool ContiguousAllocator() {
        return requires( AllocatorType& allocator, AllocatedType* allocation, size_t count ) {
            { allocator.allocate() } -> AllocatedType*;
            { allocator.allocate( count ) } -> AllocatedType*;
            { allocator.reallocate( allocation, count ) } -> AllocatedType*;
            { allocator.release( allocation ) };
        };
    }

    class Pool {
        public:

            Pool();
            Pool( std::size_t blockSize );
            ~Pool();

            template< typename StorageType >
            void* store();
            template< typename StorageType, SizeType count >
            void* store();
            void* store( std::size_t size );

            void release( void* storage );

        private:

            struct BlockHeader;    

            struct SegmentHeader {
                std::size_t     size;
                BlockHeader*    owner;
                SegmentHeader*  prev;
                SegmentHeader*  next;
            };

            class BlockHeader {
                BlockHeader( std::size_t size, BlockHeader* last );

                std::size_t     size;
                BlockHeader*    lastBlock;
                SegmentHeader*  headSegment;
              private:
                template< typename StorageType >
                static void* allocate( BlockHeader*& head );
                template< typename StorageType, SizeTpye count >
                static void* allocate( BlockHeader*& head );
                static void* allocate( BlockHeader*& head, std::size_t size );
            };

            constexpr std::size_t kDefaultBlockSize{ 1 << 14 + sizeof(BlockHeader) };

            std::size_t  mWorkingBlockSize;
            BlockHeader* mFirstBlock;
            BlockHeader* mHeadBlock;

    };

    inline
    Pool::BlockHeader::BlockHeader( std::size_t size )
      : size{ size },
        lastBlock{ nullptr },
        headSegment{ nullptr }
    {}

    inline
    Pool::Pool()
      : mWorkingBlockSize{ mDefaultBlockSize },
        mFirstBlock{ new( ::malloc( mWorkingBlockSize ) ) (mWorkingBlockSize) },
        mHeadBlock{ mFirstBlock }
    {}

    inline
    Pool::Pool( std::size_t blockSize )
      : mWorkingBlockSize{ blockSize },
        mFirstBlock{ new( ::malloc( mWorkingBlockSize ) ) (mWorkingBlockSize) },
        mHeadBlock{ mFirstBlock }
    {}

    template< typename StorageType >
    inline
    void* Pool::store() {
        return BlockHeader::allocate<StorageType>( mHeadBlock );
    }

    template< typename StorageType, SizeType count >
    inline
    void* Pool::store() {
        return BlockHeader::allocate<StorageType, count>( mHeadBlock );
    }

    template< typename StorageType >
    inline
    void* Pool::BlockHeader::allocate( Pool::BlockHeader*& head ) {
        void* storage{ nullptr };
        std::size_t offset{ head->headSegment - head + headSegment.size };
        std::size_t remainingSpace{ size - offset };
        if( remainingSpace > sizeof(StorageType) ) {
            SegmentHeader* nextSegment{ reinterpret_cast<SegmentHeader*>( head + offset ) };
            *nextSegment = { sizeof(SegmentHeader) + sizeof(StorageType),
                            this,
                            headSegment,
                            nullptr                                         };
            head->headSegment = nextSegment;
            storage = head->headSegment;
        } else {
            std::size_t defaultSize{ head->size };
            if( defaultSize < sizeof(StorageType) ) {
                defaultSize = sizeof(StorageType);
            }
            BlockHeader* newBlock = new( ::malloc( defaultSize ) ) BlockHeader( defaultSize, head );
            storage = BlockHeader::allocate<StorageType>( newBlock );
        }
        return storage;
    }

    template< typename StorageType, SizeType count >
    inline
    void* Pool::BlockHeader::allocate( Pool::BlockHeader*& head ) {
        std::size_t offset{ currentSegment - current };
    }


} // namespace memory