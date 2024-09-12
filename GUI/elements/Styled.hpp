#pragma once

namespace geni::elements {

    class Styled {
        public:
            class Style {
                public:
                    
            };
            Styled() = default;
            virtual
            ~Styled() = default;
            auto getStyle() -> Style::Ref&
            { return mStyle; }
            auto getStyle() -> Style::Ref const
            { return mStyle; }
        protected:
            void setStyle( const Style::Ref& style )
            { mStyle = style; }
        private:
            Style::Ref  mStyle;
    };

} // namespace genie::elements

#define IsStyled public geni::elements::Styled
#define IsStyled(access) access geni::elements::Styled