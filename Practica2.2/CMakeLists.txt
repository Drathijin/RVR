set (CMAKE_CXX_STANDARD 11)

set(SERIALIZABLE_H Serializable.h)

add_executable(parte1.a ejercicio1.cc utils.cc utils.h ${Serializable})
add_subdirectory(replicacion-chat)