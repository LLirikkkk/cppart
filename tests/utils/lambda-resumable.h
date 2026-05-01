#pragma once

#include "sched/resumable.h"
#include "sched/scheduler.h"

#include <utility>

namespace art::test {

template <typename F>
class LambdaResumable : public sched::Resumable<sched::IntrusiveListScheduler> {
  public:
    explicit LambdaResumable(F&& fn)
        : _fn(std::forward<F>(fn)) {}

    void resume(sched::IntrusiveListScheduler& /*unused*/) noexcept final {
        _fn();
        delete this;
    }

  private:
    std::decay_t<F> _fn;
};

template <typename F>
LambdaResumable(F&&) -> LambdaResumable<F>;

} // namespace art::test
