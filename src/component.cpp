#include "vrk/component.h"

Component::Component(std::string componentName) {
  this->isActive = false; 
  this->componentName = componentName;
}

Component::~Component() {

}

bool Component::activate() {
  if (this->isActive) {
    std::string message = this->componentName + " is already active";
    PRINT_MESSAGE(std::cerr, message);
    return false;
  }

  this->isActive = true;
  return true;
}

std::ostream& operator<<(std::ostream& os, const Component& component) {
  std::string activeMessage = (component.isActive) ? "active" : "inactive";

  os << component.componentName << " (" << activeMessage << "): " << 
      &component;

  return os;
}