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

#include <lightmanager.h>
#include <base/shader.h>
#include <scene/camera.h>

std::unique_ptr<LightManager> LightManager::active;

void NPointLight::SetUniformData(const std::string &prefix) const {
  auto s = Shader::Current();
  s->GetUniform(prefix + ".location").Set(transform.Location());
  s->GetUniform(prefix + ".ambient").Set(ambient);
  s->GetUniform(prefix + ".diffuse").Set(diffuse);
  s->GetUniform(prefix + ".specular").Set(specular);
  s->GetUniform(prefix + ".constant").Set(constant);
  s->GetUniform(prefix + ".linear").Set(linear);
  s->GetUniform(prefix + ".quadratic").Set(quadratic);
}

void JSONImpl<NPointLight>::Read(NPointLight &out, const JSON::Value &value, const JSON::ReadData &data) {
  auto t = Trace::Pusher{data.trace, "NPointLight"};
  const auto &object = JSON::GetObject(value, data);

  JSON::TryGetMember(out.ambient, "ambient", object, Vec3::one * 0.1f, data);
  JSON::GetMember(out.diffuse, "diffuse", object, data);
  JSON::GetMember(out.specular, "specular", object, data);
  JSON::TryGetMember(out.constant, "constant", object, 1.0f,  data);
  JSON::GetMember(out.linear, "linear", object, data);
  JSON::GetMember(out.quadratic, "quadratic", object, data);

  JSON::GetMember<NNode>(out, "NNode", object, data);

  auto lightManager = LightManager::Active();
  JSON::ParseAssert(lightManager, data, "Class 'LightManager' must have its static member 'active' set");
  out.Register(*lightManager);
}

void JSONImpl<NPointLight>::Write(const NPointLight &value, JSON::Writer &writer) {
  auto obj = JSON::ObjectEncloser{writer};
  JSON::Write(static_cast<const NNode &>(value), writer);
  JSON::WritePair("ambient", value.ambient, writer);
  JSON::WritePair("diffuse", value.diffuse, writer);
  JSON::WritePair("specular", value.specular, writer);
  JSON::WritePair("constant", value.constant, writer);
  JSON::WritePair("linear", value.linear, writer);
  JSON::WritePair("quadratic", value.quadratic, writer);
}

void NDirectionalLight::SetUniformData(const std::string &prefix) const {
  auto s = Shader::Current();
  s->GetUniform(prefix + ".direction").Set(GlobalRotation() * Vec3::front);
  s->GetUniform(prefix + ".ambient").Set(ambient);
  s->GetUniform(prefix + ".diffuse").Set(diffuse);
  s->GetUniform(prefix + ".specular").Set(specular);
}

void JSONImpl<NDirectionalLight>::Read(NDirectionalLight &out, const JSON::Value &value, const JSON::ReadData &data) {
  auto t = Trace::Pusher{data.trace, "NDirectionalLight"};
  const auto &object = JSON::GetObject(value, data);

  JSON::TryGetMember(out.ambient, "ambient", object, Vec3::one * 0.1f, data);
  JSON::GetMember(out.diffuse, "diffuse", object, data);
  JSON::GetMember(out.specular, "specular", object, data);

  auto lightManager = LightManager::Active();
  JSON::ParseAssert(lightManager, data, "Class 'LightManager' must have its static member 'active' set");
  out.Register(*lightManager);
}

void JSONImpl<NDirectionalLight>::Write(const NDirectionalLight &value, JSON::Writer &writer) {
  auto obj = JSON::ObjectEncloser{writer};
  JSON::WritePair("ambient", value.ambient, writer);
  JSON::WritePair("diffuse", value.diffuse, writer);
  JSON::WritePair("specular", value.specular, writer);
}

void LightManager::SetDirectionalLights(const LightingConfig &config) {
  const auto *s = Shader::Current();
  assert(s);

  // The variables we would set should not be present in the shader
  if (config.maxDirectionalLights == 0) return;

  GLint numDirLights = std::min(config.maxDirectionalLights, directionalLights.size());
  Shader::Current()->GetUniform("numDirectionalLights").Set(numDirLights);

  auto dirIt = directionalLights.begin();
  for (auto i = 0; i < numDirLights; i++, dirIt++) {
    std::stringstream iss;
    iss << "directionalLights[" << i << ']';
    (*dirIt)->SetUniformData(iss.str());
  }
}

void LightManager::SetPointLights(Vec3 location, const LightingConfig &config) {
  const auto *s = Shader::Current();
  assert(s);

  // The variables we would set should not be present in the shader
  if (config.maxPointLights == 0) return;

  GLint numPointLights = std::min(config.maxPointLights, pointLights.size());
  s->GetUniform("numPointLights").Set(numPointLights);

  // Order point lights by distance to objects
  pointLights.sort([&location](const auto &lhs, const auto &rhs) {
    return Vec3::Distance(lhs->transform.Location(), location) <
           Vec3::Distance(rhs->transform.Location(), location);
  });

  auto pointIt = pointLights.begin();
  for (auto i = 0; i < numPointLights; i++, pointIt++) {
    std::stringstream iss;
    iss << "pointLights[" << i << ']';
    (*pointIt)->SetUniformData(iss.str());
  }
}

void LightManager::SetUniformsForClosestLights(Vec3 location,
                                               const LightingConfig &config) {
  const auto *s = Shader::Current();
  assert(s);

  assert(NCamera::active);
  s->GetUniform("cameraLocation").Set(NCamera::active->transform.Location());

  SetDirectionalLights(config);
  SetPointLights(location, config);
}
