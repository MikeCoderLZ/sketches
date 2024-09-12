#pragma once

namespace geni::ctrl {

    class Thenable {
        public:
            template< typename Function >
            static
            auto then( Function&& f ) -> Thenable;
            template< typename Function >
            static
            auto intercept( Function&& f ) -> Thenable;
            template< typename Function >
            static
            auto finally( Function&& f ) -> Thenable;
        protected:
            Thenable();
    };

    // Synchronous
    class Try : public Thenable {

    };

    // Asynchronous
    class Promise : public Thenable  {

    };
}