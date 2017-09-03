cmake_minimum_required (VERSION 2.6)
project (ZBARCV)

set(CMAKE_MODULE_PATH ${ZBARCV_SOURCE_DIR})
find_package (OpenCV)
find_package (ZBar0)

CFLAGS=$(pkg-config --cflags libsodium)
LDFLAGS=$(pkg-config --libs libsodium)

add_executable (teste1 teste1.cpp)
target_link_libraries (teste1 ${OpenCV_LIBRARIES} ${ZBAR_LIBRARIES})
