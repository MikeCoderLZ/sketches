
#pragma once

#include "elements/Hierarchy.hpp"
#include "elements/Paintable.hpp"
#include "elements/Styled.hpp"
#include "elements/Bounded2D.hpp"

namespace geni::gui {

    class Component :   IsHeirarchyOf(public,geni::gui::Component),
                        IsHierarchyMember,
                        IsPaintable,
                        IsStyled {
        public:

            class Layout : IsBounded2D {
                public:
                    using Ptr = std::unique_ptr<Layout>;

                    static
                    auto make( const Bounds& bounds ) -> Ptr
                    {
                        return make_unique<Layout>( bounds );
                    }

                    Layout() = delete;
                    virtual
                    ~Layout() = default;
                protected:
                
                    friend Component;
                    // The actual signature of std::make_unique changes
                    // depending on the implementation, so you need a macro
                    gfxl_make_unique_is_friend;

                    Layout( const Bounds& bounds ) : Bounded2D{ bounds }
                    {}
                    virtual
                    void boundsChanged( const Bounds& bounds )
                    {
                        setBounds( bounds );
                    }
            };

            Component( std::string_view name )
                :   mName{ name },
                    mLayout{ Layout::make(*this) }
            {}
            Component( std::string_view name,
                       Layout::Ptr& layout )
                :   mName{ name },
                    mLayout{ std::move(layout) }
            {}
            Component( std::string_view name,
                       Layout::Ptr&& layout )
                :   mName{ name },
                    mLayout{ layout }
            {}
            
            void setStyle( const Style::Ref& style )
            {
                Stylable::setStyle(style);
                for( ChildRef child : mChildren ) {
                    child->mLayout->styleChanged(style);
                }
                repaint();
            }

            void setBounds( const Bounds& bounds )
            {
                Boundable::setBounds( bounds );
                for( ChildRef child : mChildren ) {
                    child->mLayout->boundsChanged(bounds);
                }
                repaint();
            }

            size_t getHash() const override
            {
                return hash(mName);
            }

        private:
            std::string mName;
            
            Layout::Ptr mLayout;

            void hierarchyChanged( Hierarchy::ParentRef& parent ) override
            {
                if( parent ) {
                    mLayout.boundsChanged(getLocalBounds());
                    mLayout.styleChanged(getStyle());
                } else {
                    getBounds().clear();
                    getStyle().clear();
                }
                repaint();
            }

            void paint( Context& context ) override
            {
                // Draw a magenta semi-transparent rectangle with
                // a hard edge on debug
            }

    };



}