#include "vrk/render_pass.h"

RenderPass::RenderPass() : Component("render pass") {

}

RenderPass::~RenderPass() {

}

bool RenderPass::activate() {
  if (!Component::activate()) {
    return false;
  }

  return true;
}