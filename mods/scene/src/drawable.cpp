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

#include <drawable.h>

void Drawable::Register(const std::shared_ptr<Shader> &s) {
  assert(Renderer::active);
  Renderer::active->Register(registration, s);
  registration.SetDrawFunction([this] { Draw(); });
}

Drawable::Drawable(const Drawable &other) {
  registration.CreateFrom(other.registration, [this] { Draw(); });
}

Drawable &Drawable::operator=(const Drawable &other) {
  if (this == &other) return *this;
  registration.CreateFrom(other.registration, [this] { Draw(); });
  return *this;
}

Drawable::Drawable(Drawable &&other) {
  registration.CreateFrom(other.registration, [this] { Draw(); });
}

Drawable &Drawable::operator=(Drawable &&other) {
  if (this == &other) return *this;
  registration.CreateFrom(other.registration, [this] { Draw(); });
  return *this;
}
