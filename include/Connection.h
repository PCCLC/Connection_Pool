#pragma once

#include <mysql/mysql.h>
#include <string>
#include <ctime>

using namespace std;

class Connection {
public:
    Connection();

    ~Connection();

    bool connect(string ip, unsigned short port, string username, string password, string dbname);

    bool update(string sql);

    MYSQL_RES* query(string sql);

    void refreshAliveTime() { _alivetime = clock(); }

    clock_t getAliveeTime() const { return clock() - _alivetime; }

    MYSQL* get_connect() {
        return _conn;
    }
private:
    MYSQL* _conn;
    clock_t _alivetime;
};