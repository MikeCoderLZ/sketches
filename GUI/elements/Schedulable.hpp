
#pragma once

namespace genie::elements {

    template< typename EventType, typename... Args>
    class Schedulable {
        public:
            class Event {
                public:
                    // May accepts a lambda directly
                private:
                    friend Schedulable<EventType, LoopType, Args...>;
            };
            
            Schedulable()
            {
                // Nothing in Event that warrants inheritance now, but this
                // is a sketch and I anticipate more context being necessary
                static_assert( std::derived_from<EventType, Event> );
                // Not precise code, but you get the idea. EventType
                // must implement the () operator, or something along those
                // lines.
                static_assert( std::is_invocable_v<EventType, Args...> );
            }
        protected:
            // Schedulable objects may only schedule things on themselves,
            // because they are in the best position to know if they should.
            void schedule( Event&& event );
        private:
            // Isolate the event loop implementation behind a
            // std::unique_ptr. That way we can switch it out
            // for whatever we want without recompiling everything.
            class Scheduler;
            using SchedulerPtr = std::unqiue_ptr<Schedule>;

            SchedulerPtr mScheduler;
    };
} // namespace genie::elements

#define IsSchedulable public genie::elements::Schedulable
#define IsSchedulable(access) access genie::elements::Schedulable