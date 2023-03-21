aux_source_directory(. SRC_LIST)

add_executable(ConnectionPool ${SRC_LIST})

target_link_libraries(ConnectionPool mysqlclient)

add_library(pool SHARED ${SRC_LIST})
