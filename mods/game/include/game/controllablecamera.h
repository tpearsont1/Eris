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

#ifndef _GAME__CONTROLLABLE_CAMERA_H
#define _GAME__CONTROLLABLE_CAMERA_H

#include <base/input.h>
#include <base/window.h>
#include <scene/camera.h>

class NControllableCamera : public NCamera {
  Vec2 prevPosition;
  Input::MouseMoveRegistration moveRegistration;
  Input &input;

protected:
  virtual void OnMouseMove(Vec2 mousePosition) = 0;

  Vec2 GetMouseMovementChange(Vec2 mousePosition) const {
    return mousePosition - prevPosition;
  }

public:
  NControllableCamera(Input &i = Window::Active()->GetInput());

  Input &GetInput() { return input; }
  const Input &GetInput() const { return input; }
};

#endif // _GAME__CONTROLLABLE_CAMERA_H
