cmake_minimum_required(VERSION 3.8)
project(scheduler_service CXX)

# include gRPC if needed 
include(/Users/rashmisharma/Development/sjsu/cmpe-275/grpc/examples/cpp/cmake/common.cmake)

# Shared Memory-Based Heap Code
add_library(heap STATIC heap.cpp)  # assuming heap code is in heap.cpp
target_include_directories(heap PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>) 

# scheduler Service
add_executable(scheduler scheduler.cpp)

# link with Heap Library
target_link_libraries(scheduler heap)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR} ${gRPC_CPP_PLUGIN_INCLUDE_DIR} ${gRPC_INCLUDE_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS protos/healthcheck.proto)
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS protos/healthcheck.proto)

add_executable(scheduler scheduler.cpp ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS})
target_link_libraries(scheduler heap ${PROTOBUF_LIBRARY} ${gRPC_LIBRARY})
