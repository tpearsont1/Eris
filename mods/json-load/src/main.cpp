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

#include <game/game.h>
#include <core/file.h>
#include <base/resources.h>
#include <input/input.h>
#include <scene/scene.h>
#include <scene/camera.h>
#include <scene/tagmanager.h>
#include <scene/mesh.h>
#include <scene/lightmanager.h>
#include <scene/meshconfig.h>

class MyGame : public Game {
  Scene scene;
  NNode *tagged, *shape;
  NPointLight *pointLight;

public:
  MyGame();
  void Tick(float delta) override {
    glClearColor(0.1f, 0.6f, 0.9f, 1.0f);
    scene.Render();

    pointLight->transform.Location(3.0f * sinf(GetElapsedTime()), 1.0f, 3.0f);

    shape->transform.Translate(delta, 0.0f, 0.0f);
    shape->transform.Rotate(30.0f * delta, 60.0f * delta, 0.0f);

    float mul = delta * 45.0f;
    if (Input::IsKeyDown(KeyCode::SHIFT))
      mul *= 4.0f;

    Vec3 movement;
    if (Input::IsKeyDown(KeyCode::W))
      movement += Vec3(0.0f, 0.0f, -mul);
    if (Input::IsKeyDown(KeyCode::S))
      movement += Vec3(0.0f, 0.0f, mul);
    if (Input::IsKeyDown(KeyCode::A))
      movement += Vec3(-mul, 0.0f, 0.0f);
    if (Input::IsKeyDown(KeyCode::D))
      movement += Vec3(mul, 0.0f, 0.0f);
    if (Input::IsKeyDown(KeyCode::Q))
      movement += Vec3(0.0f, -mul, 0.0f);
    if (Input::IsKeyDown(KeyCode::E))
      movement += Vec3(0.0f, mul, 0.0f);
    NCamera::active->transform.Translate(NCamera::active->transform.GlobalRotation() * movement);
  }
};

void OnMouseScroll(double /* xOffset */, double yOffset) {
  NCamera::active->fov -= yOffset * 0.1f;
}

static double startDragX = 0.0, startDragY = 0.0;

void OnMouseMove(double xPos, double yPos) {
  if (!Input::IsKeyDown(KeyCode::F)) {
    NCamera::active->transform.RotateGlobal(0.0f, static_cast<float>(startDragX - xPos), 0.0f);
    NCamera::active->transform.Rotate(static_cast<float>(yPos - startDragY), 0.0f, 0.0f);
    Input::SetMouseMode(MouseMode::DISABLED);
  }
  else
    Input::SetMouseMode(MouseMode::NORMAL);

  startDragX = xPos;
  startDragY = yPos;
}

MyGame::MyGame() {
  Input::RegisterMouseScrollCallback(OnMouseScroll);
  Input::RegisterMouseMoveCallback(OnMouseMove);
  Input::RegisterKeyCallback(KeyCode::ESCAPE, [](InputEvent action) {
    if (action == InputEvent::PRESS)
      Window::inst->Close();
  });

  Input::GetMousePosition(startDragX, startDragY);

  auto tm = std::make_unique<TagManager>();
  TagManager::active = tm.get();
  auto resources = std::make_unique<Resources>();
  Resources::active = resources.get();

  auto typeManager = std::make_shared<JSON::TypeManager>();
  JSON::ReadData readData{typeManager};
  RegisterSceneTypeAssociations(*typeManager);

  scene.SetActive();

  LightManager::active = std::make_unique<LightManager>();

  {
    using namespace MeshRenderConfigs;
    configurationGenerators["single-texture"] = MakeGenerator<AddTextures<Standard>>();
    configurationGenerators["single-texture-lit"] = MakeGenerator<MakeLit<AddTextures<Standard>>>();
  }

  JSON::Document shaders, textures, sceneDoc;

  JSON::GetDataFromFile(shaders, "mods/json-load/res/shaders.json");
  JSON::Read(Resources::active->shaders, shaders, readData);

  JSON::GetDataFromFile(textures, "mods/json-load/res/textures.json");
  JSON::Read(Resources::active->textures, textures, readData);

  JSON::GetDataFromFile(sceneDoc, "mods/json-load/res/scene.json");
  JSON::Read(scene, sceneDoc, readData);

  tagged = TagManager::active->Get<NNode>("model");
  shape = TagManager::active->Get<NNode>("shape");
  pointLight = TagManager::active->Get<NPointLight>("point-light");
}

extern "C" bool JsonLoad_Run() {
  MyGame().Start();
  return true;
}
