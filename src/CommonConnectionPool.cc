#include "CommonConnectionPool.h"

ConnectionPool* ConnectionPool::getConnectionPool() {
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::loadConfigFile() {
    FILE *pf = fopen("mysql.conf", "r");
    if (pf == nullptr) {
        cout << "mysql.conf file is not exist!" << endl;
        return false;
    }

    while (!feof(pf)) {
        char line[1024] = {0};
        fgets(line, 124, pf);
        string str = line;
        int idx = str.find('=', 0);
        if (idx == -1) {
            continue;
        }

        int endidx = str.find('\n', idx);
        string key = str.substr(0, idx);
        string value = str.substr(idx + 1, endidx - idx - 1);

        if (key == "ip") {
            _ip = value;
        }
        else if (key == "port") {
            _port = atoi(value.c_str());
        }
        else if (key == "user") {
            _username = value;
        }
        else if (key == "password") {
            _password = value;
        }
        else if (key == "init_size") {
            _initSize = atoi(value.c_str());
        }
        else if (key == "max_size") {
            _maxSize = atoi(value.c_str());
        }
        else if (key == "max_freeTime") {
            _maxIdleTime = atoi(value.c_str());
        }
        else if (key == "connect_timeout") {
            _connectionTimeout = atoi(value.c_str());
        }
    }
    return true;
}

ConnectionPool::ConnectionPool() {
    if (!loadConfigFile()) {
        return;
    }

    for (int i = 0; i < _initSize; i++) {
        Connection* p = new Connection();
        p->connect(_ip, _port, _username, _password, _dbname);
        p->refreshAliveTime();
        _connectionQue.push(p);
        _connectionCnt++;
    }

    thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

void ConnectionPool::produceConnectionTask() {
    for (; ;) {
        unique_lock<mutex> lock(_queueMutex);
        while (!_connectionQue.empty()) {
            cv.wait(lock);
        }

        if (_connectionCnt < _maxSize) {
            Connection* p = new Connection();
            p->connect(_ip, _port, _username, _password, _dbname);
            p->refreshAliveTime();
            _connectionQue.push(p);
            _connectionCnt++;
        }

        cv.notify_all();
    }
}

shared_ptr<Connection> ConnectionPool::getConnection() {
    unique_lock<mutex> lock(_queueMutex);
    while (_connectionQue.empty()) {
        if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout))) {
            if (_connectionQue.empty()) {
                cout << "获取空闲连接超时..." << endl;
                return nullptr;
            }
        }
    }

    shared_ptr<Connection> sp(_connectionQue.front(),
    [&](Connection* pcon){
    unique_lock<mutex> lock(_queueMutex);
    pcon->refreshAliveTime();
    _connectionQue.push(pcon);
    });

    _connectionQue.pop();
    cv.notify_all();

    return sp;
}

void ConnectionPool::scannerConnectionTask() {
    for (; ;) {
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));

        unique_lock<mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize) {
            Connection* p = _connectionQue.front();
            if (p->getAliveeTime() >= (_maxIdleTime * 1000)) {
                _connectionQue.pop();
                _connectionCnt--;
                delete p;
            }
            else {
                break;
            }
        }
    }
}