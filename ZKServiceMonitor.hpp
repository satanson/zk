#ifndef ZK_SERVICE_MONITOR_H_BY_SATANSON
#define ZK_SERVICE_MONITOR_H_BY_SATANSON
#include<ZooKeeper.hpp>
#include<string>
#include<vector>
#include<set>
#include<stdexcept>
#include<iostream>

using namespace std;
using namespace org::apache::zookeeper;

class ServiceMonitor{
    public:
        class SpinLock{
            pthread_spinlock_t m_mutex;
            public:
            class Sync{
                SpinLock* m_lock;
                public:
                Sync(SpinLock* lock):m_lock(lock){pthread_spin_lock(m_lock->get());}
                ~Sync(){pthread_spin_unlock(m_lock->get());}
            };

            SpinLock(){pthread_spin_init(&this->m_mutex,PTHREAD_PROCESS_PRIVATE);}
            ~SpinLock(){pthread_spin_destroy(&this->m_mutex);}
            pthread_spinlock_t* get(){return &this->m_mutex;}
        };
        class ZKSM_Watcher:public Watcher{
            private:
                ServiceMonitor *zksm;
                string baseNode;
            public:
                ZKSM_Watcher(string const& znode,ServiceMonitor* m):baseNode(znode),zksm(m){}
                void process(WatchedEvent event){
                    LOGi("Events are processed by ZooKeeper Service Monitor Watcher");
                    
                    if (event.keeperState !=KeeperState::Connected){
                        LOGe("Lost connection to ZooKeeper");
                        THROW("Lost Connection to ZooKeeper");
                    }

                    if (event.path==baseNode && 
                            event.eventType == EventType::NodeChildrenChanged){
                        LOGi("Child znodes of %s are changed",baseNode.c_str());
                        zksm->registered();
                    }
                }
        };

        string pop(){
            SpinLock::Sync sync(&lock);
            if (!services.size()){
                return string();
            }
            else {
                string serv=*services.begin();
                services.erase(serv);
                return serv;
            }     
        }
        void push(string const& service){
            SpinLock::Sync sync(&lock);
            services.insert(service);
        }

        ServiceMonitor(string const& conString,int timeout,string const& baseNode){
            this->baseNode=baseNode;
            try{
                watcher.reset(new ZKSM_Watcher(baseNode,this));
                zk.reset(new ZooKeeper(conString,timeout,watcher));
            }catch(string& err){
                LOGe("Failed to initialize ZooKeeper.Error is caused by\n%s",err.c_str());
                THROW("Failed to instantiate ServiceMonitor");
            }catch(...){
                THROW("Unknown error happened");
            }
            LOGi("Succeeded in instantiating ServiceMonitor");
        }
        void registered(){

            if(!zk->zk_exists(baseNode,false)){
                LOGe("Base znode %s not exists yet",baseNode.c_str());
                THROW("Base znode %s not exists yet",baseNode.c_str());
            }
            LOGi("Base znode %s already exists",baseNode.c_str());
            list<string> slist=zk->zk_getChildren(baseNode,true);
            while(!slist.empty()){
                string s=slist.front();
                LOGi("Service:%s is available",s.c_str());
                push(s);
                slist.pop_front();
            }
        }
        ~ServiceMonitor(){
        }
    private:
        shared_ptr<ZooKeeper> zk;
        shared_ptr<Watcher> watcher;
        string baseNode;
        set<string> services;
        SpinLock lock;
};
#endif
