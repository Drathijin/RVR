set (CMAKE_CXX_STANDARD 11)

add_executable(client ChatClient.cc Chat.h Chat.cc Socket.h Socket.cc Serializable.h utils.cc utils.h)
add_executable(server ChatServer.cc Chat.h Chat.cc Socket.h Socket.cc Serializable.h utils.cc utils.h)
target_link_libraries(client pthread)
target_link_libraries(server pthread)