/*
-------------------------------------------------------------------------------
This file is part of Eris Engine
-------------------------------------------------------------------------------
Copyright (c) 2017 Thomas Pearson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-------------------------------------------------------------------------------
*/

#ifndef _BASE__WINDOW_H
#define _BASE__WINDOW_H

#include <functional>
#include <list>
#include <unordered_map>

#include <base/gl.h>
#include <base/registration.h>

#include <math/vec.h>

class Input;

namespace Monitor {
using Type = GLFWmonitor;

inline Type *Primary() {
  if (!GLFW::IsSetup()) GLFW::Setup();
  return glfwGetPrimaryMonitor();
}
} // namespace Monitor

/*
 * Class for creating and manipulating a window
 */
class Window {
  static std::unordered_map<GLFWwindow *, Window *> windowMapping;
  static Window *active;

  /*
   * Prevent copying
   */
  Window(const Window &other) = delete;
  Window &operator=(const Window &other) = delete;

  using ResizeCallbacks = CallbackList<void(IVec2), true>;
  ResizeCallbacks resizeCallbacks;
  using ResizeCallback = ResizeCallbacks::Function;
  using ResizeRegistration = Registration<ResizeCallbacks>;

  void OnResize(int width, int height);

  IVec2 size;

  /*
   * Handles all input to the window
   */
  std::unique_ptr<Input> input;

  /*
   * The GLFW representation of the window
   */
  GLFWwindow *window;

  static void SetActiveWindowViewportSize(IVec2 size);

public:
  /*
   * The size of the window in pixels
   */
  IVec2 Size() const { return size; }

  /*
   * Constructs a window with the specified size.
   *
   * If a monitor is provided (monitor != nullptr) then the window will be made
   * fullscreen on that monitor.
   */
  Window(IVec2 size, Monitor::Type *monitor = nullptr);
  ~Window();

  static Window *Active() { return active; }

  bool IsActive() const { return Active() == this; }
  void MakeActive() {
    if (IsActive()) return;
    glfwMakeContextCurrent(window);
    active = this;
  }

  /*
   * Whether or not the window should Close
   * @returns whether the window should close
   */
  bool ShouldClose() const { return glfwWindowShouldClose(window); }

  /*
   * Swaps the window buffer
   */
  void SwapBuffers() const { glfwSwapBuffers(window); }

  /*
   * Closes the window
   */
  void Close();

  ResizeRegistration RegisterResizeCallback(ResizeCallback callback) {
    return {callback, resizeCallbacks};
  }

  Input &GetInput() const { return *input; }
};

inline Window *GetWindowFromGLFWwindow(GLFWwindow *window) {
  // Windows store a pointer to themselves upon construction.
  // This is how to retrieve it
  return reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
}

#endif // _BASE__WINDOW_H
