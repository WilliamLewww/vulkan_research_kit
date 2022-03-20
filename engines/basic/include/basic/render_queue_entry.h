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

  template <class T> std::shared_ptr<T> getEntryPtr() {
    return std::static_pointer_cast<T>(this->entryPtr);
  }

  void appendIndexedImageToDescriptor(
      std::shared_ptr<RenderQueueEntry> renderQueueEntryPtr,
      std::string indexImageName);

  RenderQueueEntryType getRenderQueueEntryType();

  std::vector<std::pair<std::shared_ptr<Material>, std::string>>
  getMaterialPtrIndexedImageNameList();

private:
  std::shared_ptr<void> entryPtr;

  RenderQueueEntryType renderQueueEntryType;

  std::vector<std::pair<std::shared_ptr<Material>, std::string>>
      materialPtrIndexedImageNameList;
};
