/*
 * Copyright (C) 2006, 2007, 2008, 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef V8LazyEventListener_h
#define V8LazyEventListener_h

#include "bindings/core/v8/V8AbstractEventListener.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/TextPosition.h"
#include <v8.h>

namespace blink {

class Event;
class Node;

// V8LazyEventListener is a wrapper for a JavaScript code string that is compiled and evaluated when an event is fired.
// A V8LazyEventListener is either a HTML or SVG event handler.
class V8LazyEventListener final : public V8AbstractEventListener {
public:
    static V8LazyEventListener* create(const AtomicString& functionName, const AtomicString& eventParameterName, const String& code, const String& sourceURL, const TextPosition& position, Node* node, v8::Isolate* isolate)
    {
        return new V8LazyEventListener(isolate, functionName, eventParameterName, code, sourceURL, position, node);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_node);
        V8AbstractEventListener::trace(visitor);
    }

    const String& code() const { return m_code; }

protected:
    void prepareListenerObject(ExecutionContext*) override;

private:
    V8LazyEventListener(v8::Isolate*, const AtomicString& functionName, const AtomicString& eventParameterName, const String& code, const String sourceURL, const TextPosition&, Node*);

    v8::Local<v8::Value> callListenerFunction(ScriptState*, v8::Local<v8::Value>, Event*) override;

    void fireErrorEvent(v8::Local<v8::Context>, ExecutionContext*, v8::Local<v8::Message>);

    AtomicString m_functionName;
    AtomicString m_eventParameterName;
    String m_code;
    String m_sourceURL;
    Member<Node> m_node;
    TextPosition m_position;
};

} // namespace blink

#endif // V8LazyEventListener_h
