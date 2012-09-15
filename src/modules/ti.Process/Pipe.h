/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef Pipe_h
#define Pipe_h

#include <tide/tide.h>
#include <Poco/Thread.h>
#include <Poco/ThreadTarget.h>

namespace ti {

class Pipe : public KEventObject {
public:
    Pipe(const char *type = "Process.Pipe");
    virtual ~Pipe();

    virtual int Write(BytesRef data);
    virtual void CallWrite(ObjectRef target, BytesRef data);
    virtual void Close();
    virtual void CallClose(ObjectRef target);
    virtual void Flush();
    void Attach(ObjectRef object);
    void Detach(ObjectRef object);
    bool IsAttached();
    AutoPtr<Pipe> Clone();

    static void FireEventAsynchronously(AutoPtr<Event> event);

    std::vector<BytesRef> readData;

protected:
    int FindFirstLineFeed(char *data, int length, int *charsToErase);
    void _Close(const ValueList& args, ValueRef result);
    void _SetOnClose(const ValueList& args, ValueRef result);
    void _Attach(const ValueList& args, ValueRef result);
    void _Detach(const ValueList& args, ValueRef result);
    void _IsAttached(const ValueList& args, ValueRef result);
    void _Write(const ValueList& args, ValueRef result);
    void _Flush(const ValueList& args, ValueRef result);

    Poco::Mutex attachedMutex;
    std::vector<ObjectRef> attachedObjects;
    Logger *logger;
};

} // namespace ti

#endif