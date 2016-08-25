simple zookeeper c++ bindings,  zookeeper service monitor implemented via zookeeper c++ bindings.
https://github.com/satanson/zk

**Zk is built on conditions as follows:**

1. **zookeeper 3.4.3 and 3.4.8**
2. **g++ 4.8.4 (c++11 option enabled)**

I don't build zk on other versions of zookeeper and g++.

ZK can be grasped in fews steps!

1. download zookeeper-3.4.8 and build c bindings of zookeeper

        $ wget  https://mirrors.tuna.tsinghua.edu.cn/apache/zookeeper/zookeeper-3.4.8/zookeeper-3.4.8.tar.gz
        $ tar xzvf zookeeper-3.4.8.tar.gz
        $ cd zookeeper-3.4.8/src/c
        $ ./configure && make && sudo make install

2. build zk demos

        $ ./build.sh
        $ ./zk_test -help
        $ ./zk_test 172.17.0.2:2181 /foobar

        $ ./build_zksm.sh
        $ ./zksm_test -help
        $ ./zksm_test 172.17.0.2:2181 /foobar

A standalone zookeeper instance or a zookeeper cluster should be started when trying zk_test and zksm_test.
If you familiar with docker, image jplock/zookeeper can be played with for testing.

API manual
You can learn how to use zk from demos(zk_test.cpp, zksm_test.cpp), which is quit easy!

1. Zookeeper ctor: establish connection to Zookeeper cluster and create a session.
```cpp
ZooKeeper(string const& connStr, int sessionTimeout,shared_ptr<Watcher> watcher)

```
override Watcher's virtual function void process(WatchedEvent &const)

2. zk_create: create znode.
```cpp
string zk_create(string const& path,string const& data,CreateMode createMode)
```
3. zk_delete: delete znode.
```cpp
void zk_delete(string const& path,int version)
```
4. zk_exists: test if znode exists.
```cpp
bool zk_exists(string const& path, bool watch)
```
5. zk_getData: fetch data from znode.
```cpp
string zk_getData(string const& path, bool watch)
```
6. zk_setData: store data into znode.
```cpp
void zk_setData(string const& path, string const& data, int version)
```
7. zk_getChildren: get znode's children.
```cpp
list<string> zk_getChildren(string const& path,bool watch)
```
