cmake_minimum_required(VERSION 3.16)
project(MyGrpcProject)

set(CMAKE_CXX_STANDARD 17)

find_package(gRPC REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
find_package(absl CONFIG REQUIRED)

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_GRPC_GRPCPP gRPC::grpc++)

# Assuming task.proto is in the same directory as your CMakeLists.txt
set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(TASK_SCHEDULER_PROTO "${PROTO_PATH}/task.proto")

set(GENERATED_PROTOBUF_PATH "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GENERATED_PROTOBUF_PATH})

set(TASK_SCHEDULER_PB_CPP_FILE "${GENERATED_PROTOBUF_PATH}/task.pb.cc")
set(TASK_SCHEDULER_PB_H_FILE "${GENERATED_PROTOBUF_PATH}/task.pb.h")
set(TASK_SCHEDULER_GRPC_PB_CPP_FILE "${GENERATED_PROTOBUF_PATH}/task.grpc.pb.cc")
set(TASK_SCHEDULER_GRPC_PB_H_FILE "${GENERATED_PROTOBUF_PATH}/task.grpc.pb.h")

add_custom_command(
      OUTPUT "${TASK_SCHEDULER_PB_H_FILE}"
             "${TASK_SCHEDULER_PB_CPP_FILE}"
             "${TASK_SCHEDULER_GRPC_PB_H_FILE}"
             "${TASK_SCHEDULER_GRPC_PB_CPP_FILE}"
      COMMAND protobuf::protoc
      ARGS --grpc_out "${GENERATED_PROTOBUF_PATH}"
           --cpp_out "${GENERATED_PROTOBUF_PATH}"
           -I "${PROTO_PATH}"
           --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_EXECUTABLE}"
           "${TASK_SCHEDULER_PROTO}"
      DEPENDS "${TASK_SCHEDULER_PROTO}")

include_directories("${GENERATED_PROTOBUF_PATH}")

add_executable(scheduler scheduler.cpp
    ${TASK_SCHEDULER_PB_CPP_FILE}
    ${TASK_SCHEDULER_GRPC_PB_CPP_FILE})
target_link_libraries(scheduler
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    absl::base
    absl::strings
    absl::synchronization
    absl::time
    absl::log)