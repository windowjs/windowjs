#ifndef WINDOWJS_GL_H
#define WINDOWJS_GL_H

#include <glad/glad.h>

GLuint CompileShader(GLuint type, const char* source);
GLuint LinkShaders(GLuint vertex, GLuint fragment);

class TextureShader {
 public:
  TextureShader();
  ~TextureShader();

  void Clear(float sw, float sh, float dx, float dy);

  // Draws the given texture directly to normalized device coordinates,
  // which is [-1, 1] for the X, Y and Z axes.
  // Use the scaling factors (sw, sh) and offsets (dx, dy) to position the
  // texture. Depth testing should be disabled for these calls.
  void DrawTexture(GLuint texture, float sw, float sh, float dx, float dy,
                   bool combine_alphas = false);

 private:
  GLuint program_;
  GLuint clear_program_;
  GLuint vao_;
  GLuint vbo_;
};

#endif  // WINDOWJS_GL_H
