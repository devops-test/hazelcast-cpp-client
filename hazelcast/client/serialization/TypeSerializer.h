//
//  TypeSerializer.h
//  Server
//
//  Created by sancar koyunlu on 1/10/13.
//  Copyright (c) 2013 sancar koyunlu. All rights reserved.
//

#ifndef HAZELCAST_TYPE_SERIALIZER
#define HAZELCAST_TYPE_SERIALIZER

#include <iostream>

namespace hazelcast{ 
namespace client{
namespace serialization{
    
class DataInput;
class DataOutput;

template<typename T>
class TypeSerializer{
public:
    void write(DataOutput*, T ) throw(std::ios_base::failure);
    T read(DataInput*) throw(std::ios_base::failure);
};

}}}
#endif /* HAZELCAST_TYPE_SERIALIZER */