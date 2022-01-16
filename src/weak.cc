#include "weak.h"

#include "fail.h"
#include "thread.h"

void* WeakPtrBase::Get() const {
  ASSERT(IsMainThread());
  return *flag_;
}

WeakPtrFactoryBase::WeakPtrFactoryBase(void* ptr) : ptr_(ptr) {
  ASSERT(IsMainThread());
}

void WeakPtrFactoryBase::EnsureFlag() {
  ASSERT(IsMainThread());
  if (!flag_) {
    flag_.reset(new void*(ptr_));
  }
}

void WeakPtrFactoryBase::Invalidate() {
  ASSERT(IsMainThread());
  if (flag_) {
    *flag_ = nullptr;
    flag_ = nullptr;
  }
}
