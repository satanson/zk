g++ -DTHREADED -g -O2 -D_GNU_SOURCE -o zk_test zk_test.cpp -lzookeeper_mt -lpthread -L/usr/local/lib -I/usr/local/include -I./ -std=c++11
