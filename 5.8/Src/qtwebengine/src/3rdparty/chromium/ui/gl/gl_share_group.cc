// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_share_group.h"

#include "base/logging.h"
#include "build/build_config.h"
#include "ui/gl/gl_context.h"

namespace gl {

GLShareGroup::GLShareGroup()
    : shared_context_(NULL)
#if defined(OS_MACOSX)
    , renderer_id_(-1)
#endif
    {
}

void GLShareGroup::AddContext(GLContext* context) {
  if (contexts_.empty())
    AboutToAddFirstContext();

  contexts_.insert(context);
}

void GLShareGroup::RemoveContext(GLContext* context) {
  contexts_.erase(context);
  if (shared_context_ == context)
    shared_context_ = NULL;
}

void* GLShareGroup::GetHandle() {
  GLContext* context = GetContext();
  if (context)
    return context->GetHandle();

  return NULL;
}

GLContext* GLShareGroup::GetContext() {
  for (ContextSet::iterator it = contexts_.begin();
       it != contexts_.end();
       ++it) {
    if ((*it)->GetHandle())
      return *it;
  }

  return NULL;
}

void GLShareGroup::SetSharedContext(GLContext* context) {
  DCHECK(contexts_.find(context) != contexts_.end());
  shared_context_ = context;
}

GLContext* GLShareGroup::GetSharedContext() {
  return shared_context_;
}

#if defined(OS_MACOSX)
void GLShareGroup::SetRendererID(int renderer_id) {
  renderer_id_ = renderer_id;
}

int GLShareGroup::GetRendererID() {
  return renderer_id_;
}
#endif

GLShareGroup::~GLShareGroup() {
}

}  // namespace gl
