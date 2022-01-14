#ifndef WINDOWJS_FAIL_H
#define WINDOWJS_FAIL_H

#define ASSERT(cond) \
  if (!(cond))       \
  Fail("Assertion failed on " __FILE__ ":%d: " #cond "\n", __LINE__)

#define ASSERT_NO_GL_ERROR()                             \
  if (GLenum error = glGetError(); error != GL_NO_ERROR) \
  Fail("glGetError is %d at %s:%d\n", error, __FILE__, __LINE__)

#define ASSERT_UV(x)                                                \
  if (int _r = (x); _r != 0)                                        \
  Fail("UV library call failed at %s:%d: %s\n", __FILE__, __LINE__, \
       uv_strerror(_r))

template <typename T>
inline void IGNORE_RESULT(const T&) {}

void Fail(const char* reason, ...);

void ErrorQuit(const char* reason, ...);

void InitFail();

#endif  // WINDOWJS_FAIL_H
