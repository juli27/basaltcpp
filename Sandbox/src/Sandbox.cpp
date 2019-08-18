#include "Sandbox.h"

#include <memory>

#include <Basalt.h>

#include "d3d9_tutorials/Lights.h"
#include "d3d9_tutorials/Matrices.h"
#include "d3d9_tutorials/Textures.h"
#include "d3d9_tutorials/Vertices.h"

SandboxApp::SandboxApp(bs::Config& config)
: mCurrentSceneIndex(0) {
  config.mWindow.mTitle = "Basalt Sandbox";
  config.mWindow.mMode = bs::WindowMode::Windowed;
}


void SandboxApp::OnInit() {
  mScenes.push_back(std::make_unique<d3d9_tuts::Vertices>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Matrices>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Lights>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Textures>());

  mScenes.at(mCurrentSceneIndex)->OnShow();
}


void SandboxApp::OnShutdown() {
  mScenes.clear();
}


void SandboxApp::OnUpdate() {
  // HACK
  static bool rightPressed = false;
  static bool leftPressed = false;
  if (bs::input::IsKeyPressed(bs::input::Key::PageUp)) {
    if (!rightPressed) {
      rightPressed = true;

      mScenes.at(mCurrentSceneIndex)->OnHide();
      mCurrentSceneIndex++;
      if (mCurrentSceneIndex >= static_cast<bs::i32>(mScenes.size())) {
        mCurrentSceneIndex = 0;
      }

      mScenes.at(mCurrentSceneIndex)->OnShow();
    }
  } else {
    rightPressed = false;
  }

  if (bs::input::IsKeyPressed(bs::input::Key::PageDown)) {
    if (!leftPressed) {
      leftPressed = true;

      mScenes.at(mCurrentSceneIndex)->OnHide();
      mCurrentSceneIndex--;
      if (mCurrentSceneIndex < 0) {
        mCurrentSceneIndex = static_cast<bs::i32>(mScenes.size() - 1);
      }

      mScenes.at(mCurrentSceneIndex)->OnShow();
    }
  } else {
    leftPressed = false;
  }

  if (bs::input::IsKeyPressed(bs::input::Key::Escape)) {
    // TODO: use a top level function for lifecycle management?
    bs::platform::RequestQuit();
  }

  mScenes.at(mCurrentSceneIndex)->OnUpdate();
}


auto bs::IApplication::Create(Config& config) -> bs::IApplication* {
  return new SandboxApp(config);
}
