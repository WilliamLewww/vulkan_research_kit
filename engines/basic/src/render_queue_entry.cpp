#include "basic/render_queue_entry.h"

RenderQueueEntry::RenderQueueEntry(RenderQueueEntryType renderQueueEntryType,
                                   std::shared_ptr<void> entryPtr)
    : renderQueueEntryType(renderQueueEntryType), entryPtr(entryPtr) {}

RenderQueueEntry::~RenderQueueEntry() {}

void RenderQueueEntry::appendIndexedImageToDescriptor(
    std::shared_ptr<RenderQueueEntry> renderQueueEntryPtr,
    std::string indexImageName) {

  std::shared_ptr<Material> materialPtr;

  if (renderQueueEntryPtr->getRenderQueueEntryType() ==
      RenderQueueEntryType::MODEL) {

    materialPtr = renderQueueEntryPtr->getEntryPtr<Model>()->getMaterialPtr();
  } else if (renderQueueEntryPtr->getRenderQueueEntryType() ==
             RenderQueueEntryType::MATERIAL) {

    materialPtr = renderQueueEntryPtr->getEntryPtr<Material>();
  }

  this->materialPtrIndexedImageNameList.push_back(
      std::make_pair(materialPtr, indexImageName));
}

RenderQueueEntryType RenderQueueEntry::getRenderQueueEntryType() {
  return this->renderQueueEntryType;
}

std::vector<std::pair<std::shared_ptr<Material>, std::string>>
RenderQueueEntry::getMaterialPtrIndexedImageNameList() {
  return this->materialPtrIndexedImageNameList;
}
