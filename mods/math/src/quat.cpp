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

#include <quat.h>

const Quat Quat::identity{0.0f, 0.0f, 0.0f, 1.0f};

Quat::Quat(Mat4 mat){
  float sqrtVal;
  float half;
  float scale = mat[0][0] + mat[1][1] + mat[2][2];

  if (scale > 0.0f) {
    sqrtVal = sqrtf(scale + 1.0f);
    w = sqrtVal * 0.5f;
    sqrtVal = 0.5f / sqrtVal;

    x = (mat[1][2] - mat[2][1]) * sqrtVal;
    y = (mat[2][0] - mat[0][2]) * sqrtVal;
    z = (mat[0][1] - mat[1][0]) * sqrtVal;
  }
  else if ((mat[0][0] >= mat[1][1]) && (mat[0][0] >= mat[2][2])) {
    sqrtVal = sqrtf(1.0f + mat[0][0] - mat[1][1] - mat[2][2]);
    half = 0.5f / sqrtVal;

    x = 0.5f * sqrtVal;
    y = (mat[0][1] + mat[1][0]) * half;
    z = (mat[0][2] + mat[2][0]) * half;
    w = (mat[1][2] - mat[2][1]) * half;
  }
  else if (mat[1][1] > mat[2][2]) {
    sqrtVal = sqrtf(1.0f + mat[1][1] - mat[0][0] - mat[2][2]);
    half = 0.5f / sqrtVal;

    x = (mat[1][0] + mat[0][1]) * half;
    y = 0.5f * sqrtVal;
    z = (mat[2][1] + mat[1][2]) * half;
    w = (mat[2][0] - mat[0][2]) * half;
  }
  else  {
    sqrtVal = sqrtf(1.0f + mat[2][2] - mat[0][0] - mat[1][1]);
    half = 0.5f / sqrtVal;

    x = (mat[2][0] + mat[0][2]) * half;
    y = (mat[2][1] + mat[1][2]) * half;
    z = 0.5f * sqrtVal;
    w = (mat[0][1] - mat[1][0]) * half;
  }
}

void Quat::SetRad(float _x, float _y, float _z) {
  float halfZ = _z * 0.5f;
  float halfX = _x * 0.5f;
  float halfY = _y * 0.5f;

  float sinZ = sinf(halfZ);
  float cosZ = cosf(halfZ);
  float sinX = sinf(halfX);
  float cosX = cosf(halfX);
  float sinY = sinf(halfY);
  float cosY = cosf(halfY);

  x = cosY * sinX * cosZ + sinY * cosX * sinZ;
  y = sinY * cosX * cosZ - cosY * sinX * sinZ;
  z = cosY * cosX * sinZ - sinY * sinX * cosZ;
  w = cosY * cosX * cosZ + sinY * sinX * sinZ;
}

Mat4 Quat::Matrix() const {
  float xx = x * x;
  float yy = y * y;
  float zz = z * z;
  float xy = x * y;
  float zw = z * w;
  float zx = z * x;
  float yw = y * w;
  float yz = y * z;
  float xw = x * w;
  return Mat4(1.0f - (2.0f * (yy + zz)), 2.0f * (xy + zw),
              2.0f * (zx - yw), 0.0f,
              2.0f * (xy - zw), 1.0f - (2.0f * (zz + xx)),
              2.0f * (yz + xw), 0.0f,
              2.0f * (zx + yw), 2.0f * (yz - xw),
              1.0f - (2.0f * (yy + xx)), 0.0f,
              0.0f, 0.0f,
              0.0f, 1.0f);
}

Quat Quat::Lerp(Quat a, Quat b, float t) {
  float t2 = 1.0f - t;
  Quat q;
  float dot = Dot(a, b);
  if (dot) {
      q.x = t2 * a.x + t * b.x;
      q.y = t2 * a.y + t * b.y;
      q.z = t2 * a.z + t * b.z;
      q.w = t2 * a.w + t * b.w;
  }
  else {
      q.x = t2 * a.x - t * b.x;
      q.y = t2 * a.y - t * b.y;
      q.z = t2 * a.z - t * b.z;
      q.w = t2 * a.w - t * b.w;
  }
  float muls = 1.0f / sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
  q.x *= muls;
  q.y *= muls;
  q.z *= muls;
  q.w *= muls;
  return q;
}

Quat Quat::operator*(const Quat &r) const {
  return Quat(r.x * w + x * r.w + r.y * z - r.z * y,
              r.y * w + y * r.w + r.z * x - r.x * z,
              r.z * w + z * r.w + r.x * y - r.y * x,
              r.w * w - r.x * x - r.y * y - r.z * z);
}

Vec3 Quat::operator*(const Vec3 &r) const {
  float x2 = x * 2.0f;
  float y2 = y * 2.0f;
  float z2 = z * 2.0f;
  float xx2 = x * x2;
  float yy2 = y * y2;
  float zz2 = z * z2;
  float xy2 = x * y2;
  float xz2 = x * z2;
  float yz2 = y * z2;
  float wx2 = w * x2;
  float wy2 = w * y2;
  float wz2 = w * z2;
  return Vec3((1.0f - (yy2 + zz2)) * r.x + (xy2 - wz2) * r.y + (xz2 + wy2) * r.z,
              (xy2 + wz2) * r.x + (1.0f - (xx2 + zz2)) * r.y + (yz2 - wx2) * r.z,
              (xz2 - wy2) * r.x + (yz2 + wx2) * r.y + (1.0f - (xx2 + yy2)) * r.z);
}

void JSONImpl<Quat>::Write(const Quat &value, JSON::Writer &writer) {
  auto a = JSON::ArrayEncloser{writer};
  JSON::Write(value.x, writer);
  JSON::Write(value.y, writer);
  JSON::Write(value.z, writer);
  JSON::Write(value.w, writer);
}

void JSONImpl<Quat>::Read(Quat &out, const JSON::Value &value, const JSON::ReadData &data) {
  auto t = Trace::Pusher{data.trace, "Quat"};

  const auto &arr = JSON::GetArray(value, data);
  for (auto &elem : arr)
    JSON::ParseAssert(elem.IsNumber(), data, "Quat's array must contain float values");

  if (arr.Size() == 4)
    out = Quat(arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat(), arr[3].GetFloat());
  else if (arr.Size() == 3)
    out = Quat({arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat()});
  else
    JSON::ParseError(data, "Quat's array must have either 3 or 4 elements");
}
