cmake_minimum_required(VERSION 3.5)
project(tasks_tools)

set(CMAKE_CXX_STANDARD 14)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(vision_interfaces REQUIRED)
find_package(faces_interfaces REQUIRED)

include_directories(
  include
  ${rclcpp_INCLUDE_DIRS}
  ${std_msgs_INCLUDE_DIRS}
  ${std_srvs_INCLUDE_DIRS}
  ${geometry_msgs_INCLUDE_DIRS}
  ${vision_interfaces_INCLUDE_DIRS}
  ${faces_interfaces_INCLUDE_DIRS}
)

add_library(tasks_tools SHARED
  src/Tasks.cpp
)

ament_target_dependencies(tasks_tools
  rclcpp
  std_msgs
  std_srvs
  geometry_msgs
  vision_interfaces
  faces_interfaces
)

install(DIRECTORY include/
  DESTINATION include
)

install(TARGETS tasks_tools
  DESTINATION lib
)

install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_export_include_directories(include)
ament_export_libraries(tasks_tools)

ament_package()
