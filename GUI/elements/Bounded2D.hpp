#pragma once

namespace genie::elements {

    class Bounded2D {
            public:
                Bounded2D() = delete;
                Bounded2D( Bounds bounds )
                    : mBounds{std::move(bounds)}
                {}
                virtual
                ~Bounded2D() = default;
                
                auto getBounds() -> Bounds&
                { return mBounds; }
                auto getLocalBounds() -> Bounds&
                { return { 0, 0, mBounds.size() }; }
                auto getBounds() -> Bounds const
                { return mBounds; }
            protected:
                void setBounds( const Bounds& bounds )
                { mBounds = bounds; }
            private:
                Bounds  mBounds;
        };

} // namespace genie::elements

#define Is2DBounded virtual public genie::elements::Bounded2D
#define Is2DBounded(access) virtual access genie::elements::Bounded2D