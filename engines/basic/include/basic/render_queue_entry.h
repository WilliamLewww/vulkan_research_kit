#pragma once
#include "basic/material.h"
#include "basic/model.h"

#include <memory>
#include <vector>

enum class RenderQueueEntryType { MODEL, MATERIAL };

class RenderQueueEntry {
public:
  RenderQueueEntry(RenderQueueEntryType renderQueueEntryType,
                   std::shared_ptr<void> entryPtr);

  ~RenderQueueEntry();

  void appendIndexedImageToDescriptor(
      std::shared_ptr<RenderQueueEntry> renderQueueEntryPtr,
      std::string indexImageName);

  RenderQueueEntryType getRenderQueueEntryType();

  template <class T> std::shared_ptr<T> getEntryPtr() {
    return std::static_pointer_cast<T>(this->entryPtr);
  }

private:
  RenderQueueEntryType renderQueueEntryType;

  std::shared_ptr<void> entryPtr;

  std::vector<std::pair<std::shared_ptr<Material>, std::string>>
      materialPtrIndexedImageNameList;
};
