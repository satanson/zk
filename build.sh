g++ -DTHREADED -g -O2 -D_GNU_SOURCE -o zk_test zk_test.cpp ./.libs/libzookeeper_mt.dll.a -lpthread -L/usr/local/lib -I/usr/local/include -I./ -std=c++0x
