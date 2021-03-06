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



#include "hazelcast/client/protocol/codec/ListAddAllCodec.h"
#include "hazelcast/client/exception/UnexpectedMessageTypeException.h"
#include "hazelcast/client/serialization/pimpl/Data.h"

namespace hazelcast {
    namespace client {
        namespace protocol {
            namespace codec {
                const ListMessageType ListAddAllCodec::RequestParameters::TYPE = HZ_LIST_ADDALL;
                const bool ListAddAllCodec::RequestParameters::RETRYABLE = false;
                const int32_t ListAddAllCodec::ResponseParameters::TYPE = 101;
                std::auto_ptr<ClientMessage> ListAddAllCodec::RequestParameters::encode(
                        const std::string &name, 
                        const std::vector<serialization::pimpl::Data > &valueList) {
                    int32_t requiredDataSize = calculateDataSize(name, valueList);
                    std::auto_ptr<ClientMessage> clientMessage = ClientMessage::createForEncode(requiredDataSize);
                    clientMessage->setMessageType((uint16_t)ListAddAllCodec::RequestParameters::TYPE);
                    clientMessage->setRetryable(RETRYABLE);
                    clientMessage->set(name);
                    clientMessage->setArray<serialization::pimpl::Data >(valueList);
                    clientMessage->updateFrameLength();
                    return clientMessage;
                }

                int32_t ListAddAllCodec::RequestParameters::calculateDataSize(
                        const std::string &name, 
                        const std::vector<serialization::pimpl::Data > &valueList) {
                    int32_t dataSize = ClientMessage::HEADER_SIZE;
                    dataSize += ClientMessage::calculateDataSize(name);
                    dataSize += ClientMessage::calculateDataSize<serialization::pimpl::Data >(valueList);
                    return dataSize;
                }

                ListAddAllCodec::ResponseParameters::ResponseParameters(ClientMessage &clientMessage) {
                    if (TYPE != clientMessage.getMessageType()) {
                        throw exception::UnexpectedMessageTypeException("ListAddAllCodec::ResponseParameters::decode", clientMessage.getMessageType(), TYPE);
                    }

                    response = clientMessage.get<bool >();
                }

                ListAddAllCodec::ResponseParameters ListAddAllCodec::ResponseParameters::decode(ClientMessage &clientMessage) {
                    return ListAddAllCodec::ResponseParameters(clientMessage);
                }

                ListAddAllCodec::ResponseParameters::ResponseParameters(const ListAddAllCodec::ResponseParameters &rhs) {
                        response = rhs.response;
                }
                //************************ EVENTS END **************************************************************************//

            }
        }
    }
}

