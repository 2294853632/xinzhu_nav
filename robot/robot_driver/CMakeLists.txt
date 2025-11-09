cmake_minimum_required(VERSION 3.0.2)
project(robot_driver)

find_package(
        catkin REQUIRED COMPONENTS
        geometry_msgs
        nav_msgs
        message_generation
        message_runtime
        roscpp
        serial
)

## Generate messages in the 'msg' folder
add_message_files(
        FILES
        aim.msg
        spinning_control.msg
)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
        CATKIN_DEPENDS geometry_msgs roscpp  std_msgs  message_runtime 
)

include_directories(
        include
        ${catkin_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}_node src/main.cpp src/serialPort.cpp)


add_dependencies(
        ${PROJECT_NAME}_node ${PROJECT_NAME}_generate_messages_cpp
)

target_link_libraries(
        ${PROJECT_NAME}_node
        ${catkin_LIBRARIES}
)
