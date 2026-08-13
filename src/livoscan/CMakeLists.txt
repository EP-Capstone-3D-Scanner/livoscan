cmake_minimum_required(VERSION 3.8)
project(livoscan)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rviz_common REQUIRED)
find_package(pluginlib REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# Add include directories
include_directories(include)

# Create the shared library for the plugin
add_library(${PROJECT_NAME} SHARED
  src/rviz2/panel.cpp
  include/rviz2/panel.hpp
)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
ament_target_dependencies(${PROJECT_NAME} rclcpp rviz_common pluginlib)

# Export the plugin.xml file
pluginlib_export_plugin_description_file(rviz_common include/rviz2/plugins.xml)

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

install(DIRECTORY include/
  DESTINATION include
)
install(FILES include/rviz2/plugins.xml
  DESTINATION share/${PROJECT_NAME}
)

install(FILES include/rviz2/plugins.xml
  DESTINATION share/${PROJECT_NAME}
)

ament_export_dependencies(rclcpp rviz_common pluginlib)
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

ament_package()
