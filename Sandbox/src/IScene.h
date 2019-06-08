#pragma once
#ifndef ISCENE_H
#define ISCENE_H

#include <basalt/Log.h>

class IScene {
public:
  virtual ~IScene() = default;

  virtual void OnUpdate() = 0;
};

#endif // !ISCENE_H
