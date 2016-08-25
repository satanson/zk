#include "ZooKeeper.hpp"
#include <cctype>
using namespace org::apache::zookeeper;

int main(int argc,char** argv){
	if (argc != 3) {
		cerr << "Usage: zk_test is a demo that show usage of Zookeeper.hpp\n"
			 << "\tzk_test -help\t\t#print help info.\n"
			 << "\tzk_test <constr> <znode>\t\t#show usage of APIS:  exists, getChildren, delete, create & etc.\n";
		return 1;
	}
	string connstr(argv[1]);
    string znode(argv[2]);
    try {
        ZooKeeper zk(connstr, 30000,shared_ptr<Watcher>(new Watcher()));
        if (zk.zk_exists(znode, false)){
            list<string> nodes=zk.zk_getChildren(znode,false);
            if (nodes.empty()){
                zk.zk_delete(znode,0);
            }
            while(!nodes.empty()){
                cout<<nodes.front()<<endl;
                nodes.pop_front();
            }
        }else{
            zk.zk_create(znode,string(znode.begin(),znode.end()),CreateMode::PERSISTENT);
            for(int i=0;i<5;++i){
                string child_znode=znode+string("/child_znode")+to_string(i);
                cout<<child_znode<<endl;
                string data(child_znode.rbegin(),child_znode.rend());
                zk.zk_create(child_znode,data,CreateMode::PERSISTENT);
            }
        }
    } catch(std::exception& e) {
        cout << e.what() << endl;
    } catch(std::string& e) {
        cout << e << endl;
    }
}
