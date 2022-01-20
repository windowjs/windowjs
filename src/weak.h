#ifndef WINDOWJS_WEAK_H
#define WINDOWJS_WEAK_H

#include <memory>

// WeakPtrs hold a weak reference to an object that lives in the main thread.
//
// WeakPtrs can be copied across threads, but can only be dereferenced in the
// main thread. This enables running tasks in background threads and getting
// their results back in the main thread, and proceed only if the wrapped object
// is still alive.
//
// WeakPtrs are created by WeakPtrFactory objects. At destruction time, the
// WeakPtrFactory automatically invalidates all outstanding WeakPtrs.

class WeakPtrBase {
 public:
  // Can be copied across threads.
  WeakPtrBase(const WeakPtrBase& other) = default;
  WeakPtrBase(WeakPtrBase&& other) = default;

  ~WeakPtrBase() = default;

  WeakPtrBase& operator=(const WeakPtrBase& other) = default;
  WeakPtrBase& operator=(WeakPtrBase&& other) = default;

  void* Get() const;

 protected:
  explicit WeakPtrBase(std::shared_ptr<void*> flag) : flag_(flag) {}

 private:
  // flag_ is set to nullptr when the WeakPtrs get invalidated.
  std::shared_ptr<void*> flag_;
};

template <typename T>
class WeakPtrFactory;

template <typename T>
class WeakPtr : public WeakPtrBase {
 public:
  // Can be copied across threads.
  WeakPtr(const WeakPtr& other) = default;
  WeakPtr(WeakPtr&& other) = default;

  ~WeakPtr() = default;

  WeakPtr& operator=(const WeakPtr& other) = default;
  WeakPtr& operator=(WeakPtr&& other) = default;

  // Returns the object pointer if the object is still alive, otherwise
  // returns nullptr. Can be called only in the main thread.
  T* Get() const { return static_cast<T*>(WeakPtrBase::Get()); }

 private:
  // Only WeakPtrFactory can create instances.
  explicit WeakPtr(std::shared_ptr<void*> flag) : WeakPtrBase(flag) {}
  friend class WeakPtrFactory<T>;
};

class WeakPtrFactoryBase {
 public:
  explicit WeakPtrFactoryBase(void* ptr);

  ~WeakPtrFactoryBase() { Invalidate(); }

 protected:
  void EnsureFlag();
  void Invalidate();

  std::shared_ptr<void*> flag_;

 private:
  void* ptr_;
};

template <typename T>
class WeakPtrFactory : public WeakPtrFactoryBase {
 public:
  explicit WeakPtrFactory(T* ptr) : WeakPtrFactoryBase(ptr) {}

  ~WeakPtrFactory() { Invalidate(); }

  WeakPtr<T> MakeWeakPtr() {
    EnsureFlag();
    return WeakPtr<T>(flag_);
  }
};

#endif  // WINDOWJS_WEAK_H
