#pragma once

#include <utility>
#include <functional>
#include <vector>

namespace geni::elements
{
  class Resolvable
  {
  };
}

namespace geni::sys
{

  class Reason
  {
  };

  class Scheduler {
    public:
      template <typename ResultType>
      using Resolved = std::function<Event(ResultType&)>;
      using Rejected = std::function<Event(Reason&)>;
      
      class Event {
        public:
          using Test = std::function<bool()>;

          // These affect the priority of an Event
          // Only continue call chain when condition is met
          auto when(Test&& test) -> Event&;
          // Only continue call chain after at least so much time has past
          auto inAtLeast(Time time) -> Event&;
          // Make sure that this Event happens after the given Event
          auto after(Event& event) -> Event&;
          // Make sure that the given Event happens after this one
          auto before(Event& event) -> Event&;
          // The Event is never removed from the piority queue and instead
          // fires whenever the conditions are met.
          auto every(Time time) -> Event&;
          auto every(Test&& test) -> Event&;
          // Set the value of the Event as soon as possible (no ordering guarantee)
          void resolve( Resolvable& value ) -> Event&;
          // Reject the Event, indicating an error
          void reject( Reason& reason ) -> Event&;

          // These define consequences and side effects
          template <typename ResultType>
          auto andThen( Resolved<ResultType>&& resolved ) -> Event&;
          auto andThen( Event& event ) -> Event&;
          auto butCatch( Rejected&& rejected ) -> Event&;
        private:
          friend Scheduler;

          // Some sort of pointer implementation is needed here
          // so that Events can be copied swiftly and the details
          // well hidden.

          Event( Scheduler&             owner,
                 Resolved<ResultType>&& resolved,
                 Rejected&&             rejected );
      };

      template <typename ResultType>
      static promise(Resolved<ResultType>&& resolved,
                     Rejected&&             rejected) -> Event&
      {
        static_assert( std::is_derived_from< ResultType, elements::Resolvable >);
      }
  };

  template <typename ResultType>
  class Promise
  {
  public:
    using ResolvedCallback = std::function<void(ResultType &)>;
    using RejectedCallback = std::function<void(const Reason &)>;
    using ThenCallback = std::function<void(class Flag &, ResultType &)>;
    using CatchCallback = std::function<void(const Reason &)>;

    class Flag
    {
    public:
      Flag();
      void raise(Reason error);
      operator bool() { return Reason ? true : false; }
      auto reason() -> Reason { return mReason; }

    private:
      Reason mReason;
    }

    Promise(ResolvedCallback &&ifResolved,
            RejectedCallback &&ifRejected);
    ~Promise() = default;

    template <typename... Args>
    void resolve(Args &&...args);
    template <typename... Args>
    void reject(Args &&...args);

    auto andThen(ThenCallback &&thenCallback) -> Promise &;
    auto butCatch(CatchCallback &&catchCallback) -> Promise &;

  private:
    using ResolutionFrames = std::vector<class ResolutionFrame>;

    enum State
    {
      kResolved,
      kRejected,
      kUnresolved
    };

    class Result
    {
    public:
      Result()
          : mState{State::kUnresolved},
            mData{.unresolved = 0}
      {
      }
      ~Result()
      {
        switch (state)
        {
        case kResolved:
          mData.result->~ResultType();
          break;
        case kRejected:
          mData.reason->~Reason();
          break;
        default:
          break;
        }
      }

      template <typename... Args>
      auto resolve(Args &&...args) -> ResultType &
      {
        switch (state)
        {
        case kResolved:
          mData.result->~ResultType();
          break;
        case kRejected:
          mData.reason->~Reason();
          break;
        default:
          break;
        }
        mData.result = new ResultType(std::forward<Args>(args)...);
        mState = State::kResolved;
        return mData.result;
      }

      template <typename... Args>
      auto reject(Args &&...args) -> Reason &
      {
        switch (state)
        {
        case kResolved:
          data.result->~ResultType();
          break;
        case kRejected:
          data.reason->~Reason();
          break;
        default:
          break;
        }
        mData.result = new Reason(std::forward<Args>(args)...);
        mState = State::kRejected;
        return mData.reason;
      }

      auto state() const -> State { return mState; }
      auto result() -> ResultType & { return *mData.result; }
      auto result() const -> const ResultType & { return *mData.result; }
      auto reason() -> Reason & { return *mData.reason; }
      auto reason() const -> const Reason & { return *mData.reason; }

    private:
      State mState;
      union
      {
        Reason *reason;
        ResultType *result;
        int unresolved;
      } mData;
    };

    class ResolutionFrame
    {
    public:
      ResolutionFrame(ThenCallback &&thenCallback)
          : mHandler{std::move(thenCallback)}
      {
      }

      void addCatcher(CatchCallback &&catchCallback)
      {
        mCatchers.emplace_back(std::move(catchCallback));
      }

      void resolve(Flag &flag, Result &result)
      {
        return mHandler(flag, result);
      }

      void reject(const Promise &promise,
                  const Result &result)
      {
        for (CatchCallback &catcher : mCatchers)
        {
          catcher(promise, result);
        }
      }

    private:
      // TODO use std::queue?
      using Catchers = std::vector<CatchCallback>;

      ThenCallback mHandler;
      Catchers mCatchers;
    };

    Result mResult;
    ResolvedCallback mResolvedCallback;
    RejectedCallback mRejectedCallback;
    ResolutionFrames mResolutionFrames;
  };

  template <typename ResultType>
  inline Promise<ResultType>::Promise(ResolvedCallback &&ifResolved,
                                      RejectedCallback &&ifRejected)
      : mResolvedCallback(std::move(ifResolved)),
        mRejectedCallback(std::move(ifRejected))
  {
  }

  template <typename ResultType>
  template <typename... Args>
  inline void Promise<ResultType>::resolve(Args &&...args)
  {
    mResult.resolve(std::forward<Args>(args)...);
    mResolvedCallback(mResult.result());
    Flag flag;
    for (ResolutionFrame &frame : mResolutionFrames)
    {
      frame.resolve(flag, mResult.result());
      if (flag)
      {
        mResult.reject(flag.reason());
        frame.reject(mResult.reason());
        break;
      }
    }
  }

  template <typename ResultType>
  template <typename... Args>
  inline void Promise<ResultType>::reject(Args &&...args)
  {
    mResult.reject(std::forward<Args>(args)...);
    mRejectedCallback(mResult.reason());
    mResolutionFrames.front().reject(mResult.reason());
  }

  template <typename ResultType>
  inline auto Promise<ResultType>::andThen(ThenCallback &&thenCallback)
      -> Promise &
  {
    mResolutionFrames.emplace_back(ResolutionFrame(std::move(thenCallback)));
  }

  template <typename ResultType>
  inline auto Promise<ResultType>::butCatch(CatchCallback &&catchCallback)
      -> Promise &
  {
    mResolutionFrames.back().addCatcher(std::move(catchCallback));
  }

}