/*
 * Copyright (c) 2008-2015, Hazelcast, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//
// Created by sancar koyunlu on 25/12/13.
//


#ifndef HAZELCAST_WriteHandler
#define HAZELCAST_WriteHandler

#include "hazelcast/util/ByteBuffer.h"
#include "hazelcast/util/ConcurrentQueue.h"
#include "hazelcast/client/connection/IOHandler.h"
#include "hazelcast/util/AtomicBoolean.h"

#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4251) //for dll export	
#endif 

namespace hazelcast {
    namespace client {
        namespace serialization {
        }

        namespace protocol {
            class ClientMessage;
        }

        namespace connection {
            class Connection;

            class OutSelector;

            class WriteHandler : public IOHandler {
            public:
                WriteHandler(Connection &connection, OutSelector &oListener, size_t bufferSize);

                ~WriteHandler();

                void handle();

                void enqueueData(protocol::ClientMessage *message);

                void run();

            private:
                util::ConcurrentQueue<protocol::ClientMessage> writeQueue;
                bool ready;
                util::AtomicBoolean informSelector;
                protocol::ClientMessage *lastMessage;
                int32_t numBytesWrittenToSocketForMessage;
                int32_t lastMessageFrameLen;
            };
        }
    }
}

#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(pop)
#endif 

#endif //HAZELCAST_WriteHandler

