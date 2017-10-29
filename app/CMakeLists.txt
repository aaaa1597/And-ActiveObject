cmake_minimum_required(VERSION 3.4.1)

add_library( testlib SHARED src/main/cpp/JniIF.cpp
                            src/main/cpp/ActiveObject.cpp
                            src/main/cpp/CppSurface.cpp
                            src/main/cpp/GLDraw.cpp
                            src/main/cpp/Matrix.cpp
                            )

find_library( log-lib log )

target_link_libraries( testlib ${log-lib} android EGL GLESv2 m )
