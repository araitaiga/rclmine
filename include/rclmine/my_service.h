#pragma once
#include <rcl/allocator.h>
#include <rcl/node.h>
#include <rcl/rcl.h>
#include <rcl/service.h>

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <rosidl_typesupport_cpp/service_type_support.hpp>
#include <stdexcept>
#include <string>
#include <thread>

namespace rclmine
{

// void handleService(rcl_service_t * service)
// {
//   std::cout << "[MyExecutor::handleService] Start handling service" << std::endl;
//   rmw_request_id_t request_header;
//   auto req = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
//   auto resp = std::make_shared<example_interfaces::srv::AddTwoInts::Response>();

//   rcl_ret_t ret = rcl_take_request(service, &request_header, req.get());
//   if (ret == RCL_RET_OK) {
//     processRequest(req, resp);

//     ret = rcl_send_response(service, &request_header, resp.get());
//     if (ret != RCL_RET_OK) {
//       std::cerr << "Failed to send response" << std::endl;
//     }
//   }
// }
struct ServiceCallbackPair
{
  rcl_service_t * service;
  std::function<void(rcl_service_t *)> callback;
};

class BaseService
{
public:
  virtual ~BaseService() = default;
  virtual ServiceCallbackPair getService() = 0;
};

template <typename MessageT>
class MyService : public BaseService
{
public:
  MyService(
    rcl_node_t * node_handle, const std::string & service_name, rcl_context_t context,
    std::function<void(rcl_service_t *)> callback)
  : node_handle_(node_handle), context_(context), callback_(callback)
  {
    std::cout << "[MyService::Constructor] MyService constructor" << std::endl;
    service_handle_ = std::make_shared<rcl_service_t>(rcl_get_zero_initialized_service());
    rcl_service_options_t service_options = rcl_service_get_default_options();

    // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/service.h#L96-L102
    const rosidl_service_type_support_t * type_support =
      rosidl_typesupport_cpp::get_service_type_support_handle<MessageT>();

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

  ServiceCallbackPair getService()
  {
    ServiceCallbackPair pair;
    pair.service = service_handle_.get();
    pair.callback = callback_;
    return pair;
  }

private:
  std::shared_ptr<rcl_service_t> service_handle_;
  std::function<void(rcl_service_t *)> callback_;
  rcl_node_t * node_handle_;
  rcl_context_t context_;
};

}  // namespace rclmine
