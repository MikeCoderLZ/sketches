#pragma once

#include "elements/Clickable.hpp"

namespace geni::elements {

    class Draggable : IsClickable {
        public:
            Draggable() = default;
            virtual
            ~Draggable() = default;
        protected:
            virtual
            void mouseMoved( const PeripheralContext& context ) {}
        private:
            // Magic
    };

} // namespace geni::elements

#define IsDraggable public geni::elements::Draggable
#define IsDraggable(access) access geni::elements::Draggable
