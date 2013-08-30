//
// Created by sancar koyunlu on 5/23/13.
// Copyright (c) 2013 hazelcast. All rights reserved.

#include "MembershipEvent.h"
#include "hazelcast/client/impl/SerializableCollection.h"
#include "hazelcast/client/protocol/AddMembershipListenerRequest.h"
#include "hazelcast/client/ClientConfig.h"
#include "hazelcast/client/spi/ClusterService.h"
#include <boost/thread.hpp>

namespace hazelcast {
    namespace client {
        namespace connection {
            ClusterListenerThread::ClusterListenerThread(ConnectionManager& connectionMgr, ClientConfig& clientConfig, spi::ClusterService& clusterService, spi::LifecycleService& lifecycleService, serialization::SerializationService& serializationService)
            : connectionManager(connectionMgr)
            , clientConfig(clientConfig)
            , clusterService(clusterService)
            , lifecycleService(lifecycleService)
            , serializationService(serializationService)
            , conn(NULL) {
                ;
            };

            void ClusterListenerThread::setInitialConnection(connection::Connection *connection) {
                this->conn = connection;
            };

            void ClusterListenerThread::run() {
                while (true) {
                    try{
                        if (conn == NULL) {
                            try {
                                conn = pickConnection();
                            } catch (std::exception & e) {
                                std::cerr << "Error while connecting to cluster! " << *conn << e.what() << std::endl;
                                return;
                            }
                        }
                        loadInitialMemberList();
                        listenMembershipEvents();
                        boost::this_thread::sleep(boost::posix_time::seconds(1));
                    }catch(std::exception & e){
                        if (lifecycleService.isRunning()) {
                            std::cerr << "Error while listening cluster events! -> " << *conn << e.what() << std::endl;
                        }
                        delete conn;
                        conn = NULL;
                        boost::this_thread::sleep(boost::posix_time::seconds(1));
                    }catch(...){
                        std::cerr << "cluster Listener Thread unknown exception\n";
                    }

                }

            };

            Connection *ClusterListenerThread::pickConnection() {
                std::vector<Address> addresses;
                if (!members.empty()) {
                    std::vector<Address> clusterAddresses = getClusterAddresses();
                    addresses.insert(addresses.begin(), clusterAddresses.begin(), clusterAddresses.end());
                }
                std::vector<Address> configAddresses = getConfigAddresses();
                addresses.insert(addresses.end(), configAddresses.begin(), configAddresses.end());
                return clusterService.connectToOne(addresses);
            };

            void ClusterListenerThread::loadInitialMemberList() {
                protocol::AddMembershipListenerRequest requestObject;
                serialization::Data request = serializationService.toData<protocol::AddMembershipListenerRequest>(&requestObject);
                conn->write(request);
                serialization::Data response = conn->read();
                impl::SerializableCollection coll = serializationService.toObject<impl::SerializableCollection >(response);


                std::map<std::string, Member> prevMembers;
                if (!members.empty()) {
                    for (std::vector<Member>::iterator it = members.begin(); it != members.end(); ++it) {
                        prevMembers[(*it).getUuid()] = *it;
                    }
                    members.clear();
                }

                const std::vector<serialization::Data *>& collection = coll.getCollection();
                for (std::vector<serialization::Data *>::const_iterator it = collection.begin(); it != collection.end(); ++it) {
                    Member member = serializationService.toObject<Member>(**it);
                    members.push_back(member);
                }
                updateMembersRef();
                std::vector<MembershipEvent> events;

                for (std::vector<Member>::iterator it = members.begin(); it != members.end(); ++it) {
                    if (prevMembers.count((*it).getUuid()) > 0) {
                        prevMembers.erase((*it).getUuid());
                    } else {
                        events.push_back(MembershipEvent((*it), MembershipEvent::MEMBER_ADDED));
                    }
                }

                for (std::map< std::string, Member >::iterator it = prevMembers.begin(); it != prevMembers.end(); ++it) {
                    events.push_back(MembershipEvent(it->second, MembershipEvent::MEMBER_REMOVED));
                }


                for (std::vector<MembershipEvent>::iterator it = events.begin(); it != events.end(); ++it) {
                    clusterService.fireMembershipEvent((*it));
                }
            };

            void ClusterListenerThread::listenMembershipEvents() {
                while (true) {
                    serialization::Data data = conn->read();
                    MembershipEvent event = serializationService.toObject<MembershipEvent>(data);
                    Member member = event.getMember();
                    if (event.getEventType() == MembershipEvent::MEMBER_ADDED) {
                        members.push_back(member);
                    } else {
                        members.erase(std::find(members.begin(), members.end(), member));
                        std::cerr << "Removing connection pool of Member[" << member << " ]:  reason => Member closed event\n";
                        connectionManager.removeConnectionPool(member.getAddress());
                    }
                    updateMembersRef();
                    clusterService.fireMembershipEvent(event);
                }
            };

            void ClusterListenerThread::updateMembersRef() {
                std::map<Address, Member, addressComparator > *map = new std::map<Address, Member, addressComparator>;
                std::cerr << "Members [" << members.size() << "]  {" << std::endl;
                for (std::vector<Member>::iterator it = members.begin(); it != members.end(); ++it) {
                    std::cerr << "\t" << (*it) << std::endl;
                    (*map)[(*it).getAddress()] = (*it);
                }
                std::cerr << "}" << std::endl;
                clusterService.membersRef.reset(map);

            };

            std::vector<Address> ClusterListenerThread::getClusterAddresses() const {
                std::vector<Address> socketAddresses;
                for (std::vector<Member>::const_iterator it = members.begin(); it != members.end(); ++it) {
                    socketAddresses.push_back((*it).getAddress());
                }
                return socketAddresses;
            };

            std::vector<Address>  ClusterListenerThread::getConfigAddresses() const {
                std::vector<Address> socketAddresses;
                std::vector<Address>  & configAddresses = clientConfig.getAddresses();

                for (std::vector<Address>::iterator it = configAddresses.begin(); it != configAddresses.end(); ++it) {
                    socketAddresses.push_back((*it));
                }
                return socketAddresses;
            };


        }
    }
}