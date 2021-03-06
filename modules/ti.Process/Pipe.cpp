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

#include "Pipe.h"

#include <queue>
#include <vector>
#include <cstring>

#if defined(OS_WIN32)
# include "win/PipeWin.h"
#else
# include "posix/PipePosix.h"
#endif

namespace Titanium {

static void FireEvents();
static Poco::ThreadTarget eventsThreadTarget(&FireEvents);
static Poco::Thread eventsThread;
Poco::Mutex pipesNeedingReadEventsMutex;
std::queue<AutoPtr<Pipe> > pipesNeedingReadEvents;
Poco::Mutex pipesNeedingCloseEventsMutex;
std::queue<AutoPtr<Pipe> > pipesNeedingCloseEvents;
Poco::Mutex otherEventsMutex;
std::vector<AutoPtr<Event> > otherEvents;

Pipe::Pipe(const char *type)
    : KEventObject(type)
    , logger(Logger::Get("Process.Pipe"))
{
    /**
     * @tiapi(method=True,name=Process.Pipe.close,since=0.5)
     * @tiapi Close this pipe to further reading/writing.
     */
    SetMethod("close", &Pipe::_Close);
    
    /**
     * @tiapi(method=True,name=Process.Pipe.write,since=0.5)
     * @tiapi Write data to this pipe
     * @tiarg[Bytes|String, data] a Bytes object or String to write to this pipe
     * @tiresult[Number] The number of bytes actually written on this pipe
     */
    SetMethod("write", &Pipe::_Write);
    
    /**
     * @tiapi(method=True,name=Process.Pipe.attach,since=0.5)
     * @tiapi Attach an IO object to this pipe. An IO object is an object that
     * @tiapi implements a public "write(Bytes)". In Titanium, this include
     * @tiapi FileStreams, and Pipes. You may also use your own custom IO implementation
     * @tiapi here.
     */
    SetMethod("attach", &Pipe::_Attach);
    
    /**
     * @tiapi(method=True,name=Process.Pipe.detach,since=0.5)
     * @tiapi Detach an IO object from this pipe. see Process.Pipe.attach.
     */
    SetMethod("detach", &Pipe::_Detach);
    
    /**
     * @tiapi(method=True,name=Process.Pipe.isAttached,since=0.5)
     * @tiresult[bool] True if this pipe is attached to any IO objects, false otherwise.
     */
    SetMethod("isAttached", &Pipe::_IsAttached);


    if (!eventsThread.isRunning())
    {
        eventsThread.start(eventsThreadTarget);
    }
}

Pipe::~Pipe()
{
}

void Pipe::Attach(KObjectRef object)
{
    Poco::Mutex::ScopedLock lock(attachedMutex);
    attachedObjects.push_back(object);
}

void Pipe::Detach(KObjectRef object)
{
    Poco::Mutex::ScopedLock lock(attachedMutex);
    std::vector<KObjectRef>::iterator i = attachedObjects.begin();
    while (i != attachedObjects.end())
    {
        KObjectRef obj = *i;
        if (obj->Equals(object))
        {
            i = attachedObjects.erase(i);
        }
        else
        {
            i++;
        }
    }
}

bool Pipe::IsAttached()
{
    Poco::Mutex::ScopedLock lock(attachedMutex);
    return attachedObjects.size() > 0;
}

int Pipe::FindFirstLineFeed(char *data, int length, int *charsToErase)
{
    int newline = -1;
    for (int i = 0; i < length; i++)
    {
        if (data[i] == '\n')
        {
            newline = i;
            *charsToErase = 1;
            break;
        }
        else if (data[i] == '\r')
        {
            if (i < length-1 && data[i+1] == '\n')
            {
                newline = i+1;
                *charsToErase = 2;
                break;
            }
        }
    }
    
    return newline;
}

AutoPtr<Pipe> Pipe::Clone()
{
    AutoPtr<Pipe> pipe = new Pipe();
    return pipe;
}

void Pipe::_Attach(const ValueList& args, KValueRef result)
{
    args.VerifyException("attach", "o");
    this->Attach(args.at(0)->ToObject());
}

void Pipe::_Detach(const ValueList& args, KValueRef result)
{
    args.VerifyException("detach", "o");
    this->Detach(args.at(0)->ToObject());
}

void Pipe::_IsAttached(const ValueList& args, KValueRef result)
{
    result->SetBool(this->IsAttached());
}

void Pipe::_Write(const ValueList& args, KValueRef result)
{
    args.VerifyException("write", "o|s");
    
    BytesRef bytes = new Bytes();
    if (args.at(0)->IsObject())
    {
        bytes = args.at(0)->ToObject().cast<Bytes>();
    }
    else if (args.at(0)->IsString())
    {
        bytes = new Bytes(args.at(0)->ToString());
    }
    
    if (bytes.isNull())
    {
        throw ValueException::FromString("Pipe.write argument should be a Bytes or string");
    }

    int written = this->Write(bytes);
    result->SetInt(written);
}

void Pipe::_Flush(const ValueList& args, KValueRef result)
{
    this->Flush();
}

void Pipe::_Close(const ValueList& args, KValueRef result)
{
    this->Close();
}

int Pipe::Write(BytesRef bytes)
{
    { // Start the callbacks
        Poco::Mutex::ScopedLock lock(pipesNeedingReadEventsMutex);
        readData.push_back(bytes);

        this->duplicate();
        pipesNeedingReadEvents.push(this);
    }

    // We want this to execute on the same thread and to make all
    // our writeable objects thread safe. This will allow data to
    // flow through pipes more quickly.
    {
        Poco::Mutex::ScopedLock lock(attachedMutex);
        for (size_t i = 0; i < attachedObjects.size(); i++)
        {
            this->CallWrite(attachedObjects.at(i), bytes);
        }
    }

    return bytes->Length();
}

void Pipe::CallWrite(KObjectRef target, BytesRef bytes)
{
    KMethodRef writeMethod = target->GetMethod("write");

    if (writeMethod.isNull())
    {
        std::string error = "Target object did not have a write method";
        logger->Error(error);
        return;
    }
    else
    {
        try
        {
            writeMethod->Call(ValueList(Value::NewObject(bytes)));
        }
        catch (ValueException &e)
        {
            SharedString ss = e.DisplayString();
            logger->Error("Exception while trying to write to target: %s",
                ss->c_str());
        }
    }
}

void Pipe::Close()
{
    {
        Poco::Mutex::ScopedLock lock(pipesNeedingCloseEventsMutex);
        this->duplicate();
        pipesNeedingCloseEvents.push(this);
    }

    // Call the close method on our attached objects
    {
        Poco::Mutex::ScopedLock lock(attachedMutex);
        for (size_t i = 0; i < attachedObjects.size(); i++)
        {
            this->CallClose(attachedObjects.at(i));
        }
    }
}

void Pipe::CallClose(KObjectRef target)
{
    KValueRef closeMethod = target->Get("close");

    if (!closeMethod->IsMethod())
    {
        logger->Warn("Target object did not have a close method");
        return;
    }
    else
    {
        try
        {
            closeMethod->ToMethod()->Call(ValueList());
        }
        catch (ValueException &e)
        {
            SharedString ss = e.DisplayString();
            logger->Error("Exception while trying to write to target: %s",
                ss->c_str());
        }
    }
}

void Pipe::Flush()
{
}

// TODO: This should eventually be a feature of the event handling system.
/*static*/
void Pipe::FireEventAsynchronously(AutoPtr<Event> event)
{
    Poco::Mutex::ScopedLock lock(otherEventsMutex);
    otherEvents.push_back(event);
}

static void FireEvents()
{
    while (true)
    {

        // We need to collect all the events in the reverse order
        // that they should be fired. This avoid a race condition where
        // a CLOSE or EXIT event happens before a READ event.
        std::queue<AutoPtr<Event> > otherEventsCopy;
        {
            Poco::Mutex::ScopedLock lock(otherEventsMutex);
            for (size_t i = 0; i < otherEvents.size(); i++)
                otherEventsCopy.push(otherEvents[i]);

            otherEvents.clear();
        }

        std::queue<AutoPtr<Event> > closeEvents;
        {
            Poco::Mutex::ScopedLock lock(pipesNeedingCloseEventsMutex);
            while (pipesNeedingCloseEvents.size() > 0)
            {
                AutoPtr<Pipe> pipe = pipesNeedingCloseEvents.front();
                pipesNeedingCloseEvents.pop();

                AutoPtr<Event> closeEvent = new Event(pipe, Event::CLOSE);
                AutoPtr<Event> closedEvent = new Event(pipe, Event::CLOSED);
                closeEvents.push(closeEvent);
                closeEvents.push(closedEvent);
            }
        }

        std::queue<AutoPtr<Event> > readEvents;
        {
            Poco::Mutex::ScopedLock lock(pipesNeedingReadEventsMutex);
            while (pipesNeedingReadEvents.size() > 0)
            {
                AutoPtr<Pipe> pipe = pipesNeedingReadEvents.front();
                pipesNeedingReadEvents.pop();

                if (pipe->readData.size() > 0)
                {
                    BytesRef glob(Bytes::Concat(pipe->readData));
                    AutoPtr<Event> event = new ReadEvent(pipe, glob);
                    readEvents.push(event);
                    pipe->readData.clear();
                }
            }
        }

        while (readEvents.size() > 0)
        {
            AutoPtr<Event> event = readEvents.front();
            event->target->FireEvent(event);
            readEvents.pop();
        }

        while (closeEvents.size() > 0)
        {
            AutoPtr<Event> event = closeEvents.front();
            event->target->FireEvent(event);
            closeEvents.pop();
        }

        while (otherEventsCopy.size() > 0)
        {
            AutoPtr<Event> event = otherEventsCopy.front();
            event->target->FireEvent(event);
            otherEventsCopy.pop();
        }

        Poco::Thread::sleep(5);
    }
}

} // namespace Titanium
