//
//  Data.cpp
//  Server
//
//  Created by sancar koyunlu on 1/10/13.
//  Copyright (c) 2013 sancar koyunlu. All rights reserved.
//
#include "Data.h"
#include "BufferedDataOutput.h"


namespace hazelcast {
    namespace client {
        namespace serialization {

            Data::Data() : partitionHash(-1)
            , type(SerializationConstants::CONSTANT_TYPE_DATA)
            , isError(false)
            , cd(NULL)
            , buffer(new std::vector<byte>) {

            };

            Data::Data(const Data& rhs) {
                (*this) = rhs;
            };

            Data::Data(const int type, std::auto_ptr <std::vector<byte> > buffer) : partitionHash(-1)
            , isError(false)
            , cd(NULL)
            , buffer(buffer) {
                this->type = type;
            };

            Data::~Data() {
            };

            Data& Data::operator = (const Data& rhs) {
                type = rhs.type;
                cd = rhs.cd;
                partitionHash = rhs.partitionHash;
                isError = rhs.isError;
                buffer.reset(new std::vector<byte >);
                *(buffer.get()) = *(rhs.buffer.get());
                return (*this);
            };

            bool Data::operator ==(const Data& rhs) const {
                if (type != rhs.type) return false;
                if (cd != rhs.cd) return false;
                if (partitionHash != rhs.partitionHash) return false;
                if (*(buffer.get()) != *(rhs.buffer).get()) return false;
                return true;
            };

            bool Data::operator !=(const Data& rhs) const {
                return !((*this) == rhs);
            };


            bool Data::isServerError() {
                return isError;
            }

            void Data::setSerializationContext(SerializationContext *context) {
                this->context = context;
            };

            int Data::bufferSize() const {
                return buffer->size();
            };

            /**
             * Calculates the size of the binary after the Data is serialized.
             *
             * WARNING:
             *
             * Should be in sync with {@link #writeData(com.hazelcast.nio.ObjectDataOutput)}
             */
            int Data::totalSize() const {
                int total = 0;
                total += 4; // id
                if (cd != NULL) {
                    total += 4; // classDefinition-classId
                    total += 4; // classDefinition-namespace-size
                    total += 4; // // classDefinition-factory-id
                    total += 4; // classDefinition-version
                    total += 4; // classDefinition-binary-length
                    total += cd->getBinary().size(); // cd-binary
                } else {
                    total += 4; // no-classId
                }
                total += 4; // buffer-size
                total += bufferSize(); // buffer
                total += 4; // partition-hash
                return total;
            };

            int Data::getPartitionHash() {
                return partitionHash;
            };

            void Data::setPartitionHash(int partitionHash) {
                this->partitionHash = partitionHash;
            };

            int Data::getType() {
                return type;
            };

            void Data::setType(int type) {
                this->type = type;
            };

            void Data::setBuffer(auto_ptr<vector<unsigned char> > buffer) {
                this->buffer = buffer;
            };

            int Data::getFactoryId() const {
                return FACTORY_ID;
            };

            int Data::getClassId() const {
                return ID;
            };


        }
    }
}

