cmake_minimum_required(VERSION 3.13)
project(oled_screen)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.cpp WebsocketServer.cpp RpcTask.cpp utils.cpp AsioContext.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

include_directories(base)

# LOG
include_directories(modules/LOG)

# asio
add_definitions(-DASIO_STANDALONE)
include_directories(modules/asio/asio/include)

# websocketpp
add_definitions(-D_WEBSOCKETPP_CPP11_THREAD_)
include_directories(modules/websocketpp)

# RpcCore
add_subdirectory(modules/RpcCore)
target_link_libraries(${PROJECT_NAME} RpcCore)
