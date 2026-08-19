cmake_minimum_required(VERSION 3.8)
project(livoscan)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rviz_common REQUIRED)
find_package(pluginlib REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

find_package(rclcpp_components REQUIRED)
find_package(livox_interfaces_ext REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# ------------------------------------------------------------------------------
# Testing
# ------------------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# ------------------------------------------------------------------------------
# 1. RViz2 Plugin Shared Library Target
# ------------------------------------------------------------------------------
add_library(${PROJECT_NAME} SHARED
  src/rviz2/panel.cpp
  include/rviz2/panel.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Plain signature (no PUBLIC keyword) to remain compatible with ament_target_dependencies
target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  rviz_common
  pluginlib
  sensor_msgs
  cv_bridge
)

# ------------------------------------------------------------------------------
# 2. UVC Camera Driver Executable Target
# ------------------------------------------------------------------------------
add_executable(uvc_camera_driver_node
  src/uvc_camera_driver/uvc_camera_driver_node.cpp
)

target_include_directories(uvc_camera_driver_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  ${OpenCV_INCLUDE_DIRS}
)

# Plain signature (no PRIVATE keyword) to remain compatible with ament_target_dependencies
target_link_libraries(uvc_camera_driver_node
  ${OpenCV_LIBRARIES}
)

ament_target_dependencies(uvc_camera_driver_node
  rclcpp
  sensor_msgs
  cv_bridge
)

#
# My thing
#
add_executable(livox_interface_convert src/livox_interface_convert/livox_interface_convert_node.cpp)
ament_target_dependencies(livox_interface_convert
  rclcpp
  sensor_msgs
  livox_interfaces_ext
)

add_library(camera_time_offset_component SHARED src/camera_time_offset/camera_time_offset_node.cpp)
ament_target_dependencies(camera_time_offset_component
  rclcpp
  rclcpp_components
  sensor_msgs
)
rclcpp_components_register_nodes(
  camera_time_offset_component
  "CameraTimeOffsetNode"
)
install(TARGETS camera_time_offset_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# ------------------------------------------------------------------------------
# Pluginlib & Exports
# ------------------------------------------------------------------------------
# Export the plugin.xml file for RViz2
pluginlib_export_plugin_description_file(rviz_common plugins.xml)

# ------------------------------------------------------------------------------
# Installation Rules
# ------------------------------------------------------------------------------
# Install shared library
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# Install executable (must go into lib/${PROJECT_NAME} for ROS 2)
install(TARGETS uvc_camera_driver_node
  DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS livox_interface_convert
  DESTINATION lib/${PROJECT_NAME}
)

# Install directories & config files
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

install(DIRECTORY usb_cam
  DESTINATION share/${PROJECT_NAME}/
)

install(DIRECTORY include/
  DESTINATION include
)

install(FILES plugins.xml
  DESTINATION share/${PROJECT_NAME}
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rviz2")
  install(DIRECTORY rviz2
    DESTINATION share/${PROJECT_NAME}
  )
endif()

# ------------------------------------------------------------------------------
# Package Export
# ------------------------------------------------------------------------------
ament_export_dependencies(rclcpp rviz_common pluginlib sensor_msgs cv_bridge rclcpp_components)
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

ament_package()
