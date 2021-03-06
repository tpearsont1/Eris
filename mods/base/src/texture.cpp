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

#include <iostream>
#include <texture.h>

bool Texture::Load(const TextureSettings &settings) {
  RawImage r;
  if (!r.Load(settings.path, true)) {
    std::cerr << "Failed to load texture with path: " << settings.path << '\n';
    return false;
  }

  Load(r, settings);
  return true;
}

static GLuint Generate(const TextureSettings &settings, GLint internalFormat,
                       IVec2 size, GLenum format, GLenum type,
                       const GLvoid *data) {
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture((GLenum)settings.type, id);

  // Map the image to the texture
  glTexImage2D(GL_TEXTURE_2D, 0,
               internalFormat, // Internal format
               size.x, size.y, // Width and height
               0, // No border
               format, // Pixels stored in RGBA format
               type, // Pixels stored as unsigned numbers
               data); // The pixel data

  glTexParameteri((GLenum)settings.type, GL_TEXTURE_MAG_FILTER,
                  (GLint)settings.enlargeFilter);
  glTexParameteri((GLenum)settings.type, GL_TEXTURE_MIN_FILTER,
                  (GLint)settings.shrinkFilter);
  return id;
}

// TODO: Add more options and make this more versatile
void Texture::Load(const RawImage &raw, const TextureSettings &settings) {
  size = raw.size;
  id = Generate(settings, GL_RGBA, size, GL_RGBA, GL_UNSIGNED_BYTE,
                raw.data.data());
}

void Texture::CreateForFramebuffer(IVec2 _size) {
  size = _size;
  auto settings =
      TextureSettings{TextureType::Tex2D, TextureShrinkType::Nearest,
                      TextureEnlargeType::Nearest};
  id = Generate(settings, GL_RGB, size, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}
