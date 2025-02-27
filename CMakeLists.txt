cmake_minimum_required(VERSION 3.8)
project(rclmine)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rcl REQUIRED)
find_package(rcl_action REQUIRED)
find_package(rcutils REQUIRED)
find_package(std_msgs REQUIRED)
find_package(example_interfaces REQUIRED)

# add_library(rclmine SHARED
# src/rclmine/my_node.cpp
# )

# target_include_directories(rclmine
# PUBLIC
# $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
# $<INSTALL_INTERFACE:include>
# )

# # specific order: dependents before dependencies
# ament_target_dependencies(rclmine
# rcl
# rcl_action
# rcutils
# std_msgs
# example_interfaces
# )
# install(
# TARGETS rclmine EXPORT ${PROJECT_NAME}
# ARCHIVE DESTINATION lib
# LIBRARY DESTINATION lib
# RUNTIME DESTINATION bin
# )

# install(
# DIRECTORY include/
# DESTINATION include
# )

# # ##############################
# # Add publisher node
# add_executable(rclmine_pub_node
# src/rclmine/usecase/main_pub.cpp
# )

# target_include_directories(rclmine_pub_node
# PUBLIC
# $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
# $<INSTALL_INTERFACE:include>
# )

# target_link_libraries(rclmine_pub_node
# rclmine
# )
# ament_target_dependencies(rclmine_pub_node
# rcl
# rcl_action
# rcutils
# std_msgs
# )
# install(
# TARGETS rclmine_pub_node
# DESTINATION lib/${PROJECT_NAME}
# )

# # #################################
# # Add the subscriber node
# add_executable(rclmine_sub_node
# src/rclmine/usecase/main_sub.cpp
# )

# target_include_directories(rclmine_sub_node
# PUBLIC
# $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
# $<INSTALL_INTERFACE:include>
# )

# target_link_libraries(rclmine_sub_node
# rclmine
# )
# ament_target_dependencies(rclmine_sub_node
# rcl
# rcl_action
# rcutils
# std_msgs
# )
# install(
# TARGETS rclmine_sub_node
# DESTINATION lib/${PROJECT_NAME}
# )

# # ##############################
# # Add service node
# add_executable(rclmine_service_node
# src/rclmine/usecase/main_service.cpp
# )

# target_include_directories(rclmine_service_node
# PUBLIC
# $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
# $<INSTALL_INTERFACE:include>
# )

# target_link_libraries(rclmine_service_node
# rclmine
# )
# ament_target_dependencies(rclmine_service_node
# rcl
# rcl_action
# rcutils
# std_msgs
# )
# install(
# TARGETS rclmine_service_node
# DESTINATION lib/${PROJECT_NAME}
# )

# # ##############################
# # Add client node
# add_executable(rclmine_client_node
# src/rclmine/usecase/main_client.cpp
# )

# target_include_directories(rclmine_client_node
# PUBLIC
# $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
# $<INSTALL_INTERFACE:include>
# )

# target_link_libraries(rclmine_client_node
# rclmine
# )
# ament_target_dependencies(rclmine_client_node
# rcl
# rcl_action
# rcutils
# std_msgs
# )
# install(
# TARGETS rclmine_client_node
# DESTINATION lib/${PROJECT_NAME}
# )

# ##############################
# Add executor node
add_executable(rclmine_executor_pub
  src/rclmine/usecase/main_executor_pub.cpp
)

target_include_directories(rclmine_executor_pub
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(rclmine_executor_pub
  rcl
  rcl_action
  rcutils
  std_msgs
  example_interfaces
)
install(
  TARGETS rclmine_executor_pub
  DESTINATION lib/${PROJECT_NAME}
)

# ##############################
# Add executor node
add_executable(rclmine_executor_sub
  src/rclmine/usecase/main_executor_sub.cpp
)

target_include_directories(rclmine_executor_sub
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(rclmine_executor_sub
  rcl
  rcl_action
  rcutils
  std_msgs
  example_interfaces
)
install(
  TARGETS rclmine_executor_sub
  DESTINATION lib/${PROJECT_NAME}
)

# ##############################
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
