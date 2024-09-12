#pragma once

#include <memory>

namespace sys {

    class Pixie {
        public:
            using Ptr = std::unique_ptr<Pixie>;
            
            virtual
            void do() =0;
    };

    class Runner {
        public:
            void add( Pixie::Ptr&& pixie );
    }

}