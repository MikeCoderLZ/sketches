#pragma once

#include "elements/Bounded2D.hpp"

namespace geni::elements {

    class Paintable : IsBounded2D {
        public:
            Paintable() : mPainter{*this} {}
            ~Paintable() override = default;
        protected:
            void repaint() const
            { mPainter.repaint(); }
        private:
            // friend ThePaintSystemClass;
            friend
            class Painter /* implements some concept from ThePaintSystemClass */{
                // The interface should clear itself from the ThePaintSystemClass
                public:
                    Painter() = delete;
                    bool shouldPaint()
                    { return mRepaint; }
                    void repaint()
                    { 
                        // Instead, toss a repaint event into some scheduler or ThePatinSystemClass?
                        mRepaint = true;
                    }
                    void paint( Context& context ) const
                    {
                        owner.paint(context);
                        mRepaint = false;
                    }
                private:
                    Paintable& mOwner;
                    mutable bool mRepaint;
                    explicit
                    Prainter( Paintable& owner )
                        : mOwner{ owner } {}
            } mPainter;

            virtual
            void paint( Context& context ) const =0;
    };

} // namespace geni::elements

#define IsPaintable public geni::elements::Paintable
#define IsPaintable(access) access geni::elements::Paintable
