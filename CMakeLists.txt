cmake_minimum_required(VERSION 3.13)
project(oled_screen)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.cpp NetChannel.cpp RpcTask.cpp utils.cpp AsioContext.cpp TcpServer.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

include_directories(base)

# LOG
include_directories(modules/LOG)

# asio
add_definitions(-DASIO_STANDALONE)
include_directories(modules/asio/asio/include)

# RpcCore
add_subdirectory(modules/RpcCore)
target_link_libraries(${PROJECT_NAME} RpcCore)

# udp_multicast
include_directories(modules/udp_multicast)

# PacketProcessor
add_subdirectory(modules/PacketProcessor)
target_link_libraries(${PROJECT_NAME} PacketProcessor)
