#pragma once

#include "vrk/helper.h"

#include <iostream>
#include <string>

class Component {
protected:
  bool isActive;

  std::string componentName;
public:
  Component(std::string componentName);
  ~Component();

  virtual bool activate();

  friend std::ostream& operator<<(std::ostream& os,
      const Component& component);
};