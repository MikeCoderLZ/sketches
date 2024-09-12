#pragma once

namespace geni::elements {

    class Hashable {
        public:
            virtual
            size_t getHash() const =0;
    };

} // namespace geni::elements

#define IsHashable public geni::elements::Hashable
#define IsHashable(access) access geni::elements::Hashable