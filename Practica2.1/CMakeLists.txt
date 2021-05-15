set (CMAKE_CXX_STANDARD 11)

add_executable(ejercicio1.a ejercicio1.cc utils.cc utils.h)
add_executable(ejercicio2.a ejercicio2.cc utils.cc utils.h)
add_executable(ejercicio3.a ejercicio3.cc utils.cc utils.h)
add_executable(ejercicio4.a ejercicio4.cc utils.cc utils.h)
add_executable(ejercicio5.a ejercicio5.cc utils.cc utils.h)
add_executable(ejercicio6.a ejercicio6.cc utils.cc utils.h)
add_executable(ejercicio7.a ejercicio7.cc utils.cc utils.h)

target_link_libraries(ejercicio6.a pthread)
target_link_libraries(ejercicio7.a pthread)