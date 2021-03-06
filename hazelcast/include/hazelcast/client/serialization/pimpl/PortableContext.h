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
//  PortableContext.h
//  Server
//
//  Created by sancar koyunlu on 1/10/13.
//  Copyright (c) 2013 sancar koyunlu. All rights reserved.
//

#ifndef HAZELCAST_SERIALIZATION_CONTEXT
#define HAZELCAST_SERIALIZATION_CONTEXT

#include "hazelcast/client/serialization/Portable.h"
#include "hazelcast/util/SynchronizedMap.h"
#include "hazelcast/client/serialization/pimpl/SerializerHolder.h"
#include <map>
#include <vector>
#include <memory>

#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4251) //for dll export
#endif

namespace hazelcast {
    namespace client {
        namespace serialization {

            class ClassDefinition;

            namespace pimpl {

                class Data;

                class ClassDefinitionContext;

                class SerializationConstants;

                class HAZELCAST_API PortableContext {
                public:

                    PortableContext(int version,const SerializationConstants& constants);

                    int getClassVersion(int factoryId, int classId);

                    void setClassVersion(int factoryId, int classId, int version);

                    boost::shared_ptr<ClassDefinition> lookupClassDefinition(int factoryId, int classId, int version);

                    boost::shared_ptr<ClassDefinition> registerClassDefinition(boost::shared_ptr<ClassDefinition>);

                    boost::shared_ptr<ClassDefinition> lookupOrRegisterClassDefinition(const Portable& portable);

                    int getVersion();

                    boost::shared_ptr<ClassDefinition> readClassDefinition(DataInput &input, int id, int classId, int version);

                    SerializerHolder &getSerializerHolder();


                    SerializationConstants const& getConstants() const;

                private:

                    PortableContext(const PortableContext &);

                    ClassDefinitionContext &getClassDefinitionContext(int factoryId);

                    void operator = (const PortableContext &);

                    int contextVersion;
                    util::SynchronizedMap<int, ClassDefinitionContext> classDefContextMap;
                    SerializerHolder serializerHolder;
                    const SerializationConstants& constants;
                };
            }
        }
    }
}

#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(pop)
#endif

#endif /* HAZELCAST_SERIALIZATION_CONTEXT */

