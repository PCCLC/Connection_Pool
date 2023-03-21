#include "Connection.h"
#include <iostream>

using namespace std;

Connection::Connection() {
    _conn = mysql_init(nullptr);
}

Connection::~Connection() {
    if (_conn != nullptr) {
        mysql_close(_conn);
    }
}

bool Connection::connect(string ip, unsigned short port, string username, string password, string dbname) {
    MYSQL* p = mysql_real_connect(_conn, ip.c_str(), username.c_str(),
                password.c_str(), dbname.c_str(), port, nullptr, 0);
    return p != nullptr;
}

bool Connection::update(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        std::cout << "更新失败" << std::endl;
        return false;
    }
    return true;
}

MYSQL_RES* Connection::query(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        cout << "查询失败" << endl;
        return nullptr;
    }
    return mysql_use_result(_conn);
}