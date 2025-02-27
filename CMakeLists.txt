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
# Add executor node
add_executable(rclmine_executor_client
  src/rclmine/usecase/main_executor_client.cpp
)

target_include_directories(rclmine_executor_client
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(rclmine_executor_client
  rcl
  rcl_action
  rcutils
  std_msgs
  example_interfaces
)
install(
  TARGETS rclmine_executor_client
  DESTINATION lib/${PROJECT_NAME}
)

# ##############################
# Add executor node
add_executable(rclmine_executor_service
  src/rclmine/usecase/main_executor_service.cpp
)

target_include_directories(rclmine_executor_service
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(rclmine_executor_service
  rcl
  rcl_action
  rcutils
  std_msgs
  example_interfaces
)
install(
  TARGETS rclmine_executor_service
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
