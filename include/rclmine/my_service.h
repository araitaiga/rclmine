#pragma once
#include <rcl/allocator.h>
#include <rcl/node.h>
#include <rcl/rcl.h>
#include <rcl/service.h>

#include <chrono>
#include <example_interfaces/srv/add_two_ints.hpp>
#include <iostream>
#include <memory>
#include <rosidl_typesupport_cpp/service_type_support.hpp>
#include <stdexcept>
#include <string>
#include <thread>

namespace rclmine
{
class MyService
{
public:
  MyService(rcl_node_t * node_handle, const std::string & service_name, rcl_context_t context)
  : node_handle_(node_handle), context_(context)
  {
    std::cout << "[MyService::Constructor] MyService constructor" << std::endl;
    service_handle_ = std::make_shared<rcl_service_t>(rcl_get_zero_initialized_service());
    rcl_service_options_t service_options = rcl_service_get_default_options();

    // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/service.h#L96-L102
    const rosidl_service_type_support_t * type_support =
      rosidl_typesupport_cpp::get_service_type_support_handle<
        example_interfaces::srv::AddTwoInts>();

    rcl_ret_t ret = rcl_service_init(
      service_handle_.get(), node_handle_, type_support, service_name.c_str(), &service_options);

    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize service");
    }

    std::cout << "[MyService::Constructor] Service is constructed" << std::endl;
  }

  ~MyService()
  {
    rcl_ret_t ret_srvinit = rcl_service_fini(service_handle_.get(), node_handle_);
    if (ret_srvinit != RCL_RET_OK) {
      std::cerr << "Failed to fini service" << std::endl;
    }
    std::cout << "[MyService::Destructor] Service is destructed" << std::endl;
  }

  std::shared_ptr<rcl_service_t> getService() { return service_handle_; }

private:
  std::shared_ptr<rcl_service_t> service_handle_;
  rcl_node_t * node_handle_;
  rcl_context_t context_;
};

}  // namespace rclmine
