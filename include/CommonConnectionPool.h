#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <iostream>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>

using namespace std;

#include "Connection.h"

class ConnectionPool {
public:
    static ConnectionPool* getConnectionPool();
    shared_ptr<Connection> getConnection();

private:
    ConnectionPool();

    bool loadConfigFile();

    void produceConnectionTask();

    void scannerConnectionTask();

    string _ip;
    unsigned short _port;
    string _username;
    string _password;
    string _dbname;

    int _initSize;
    int _maxSize;
    int _maxIdleTime;
    int _connectionTimeout;

    queue<Connection*> _connectionQue;
    mutex _queueMutex;
    atomic_int _connectionCnt;
    condition_variable cv;
};