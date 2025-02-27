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
    service_handle_ = std::make_unique<rcl_service_t>(rcl_get_zero_initialized_service());
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

  void service()
  {
    std::cout << "[MyService::service] Start service()" << std::endl;
    rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
    rcl_ret_t ret =
      rcl_wait_set_init(&wait_set, 0, 0, 0, 0, 1, 0, &context_, rcl_get_default_allocator());
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize wait set");
    }

    std::cout << "[MyService::service] Start waiting for request" << std::endl;

    // executorの実装例
    // https://github.com/ros2/rclc/blob/6f0a9edb3cf30df0368d3abbad2fa66149e5cc65/rclc/src/rclc/executor.c#L1808
    while (true) {
      ret = rcl_wait_set_clear(&wait_set);
      if (ret != RCL_RET_OK) {
        throw std::runtime_error("Failed to clear wait set");
      }

      ret = rcl_wait_set_add_service(&wait_set, service_handle_.get(), nullptr);
      if (ret != RCL_RET_OK) {
        throw std::runtime_error("Failed to add service to wait set");
      }

      ret = rcl_wait(&wait_set, RCL_MS_TO_NS(1000));  // wait for 1s
      if (ret == RCL_RET_TIMEOUT) {
        std::cout << "[MyService::service] Continue" << std::endl;
        continue;
      }

      if (wait_set.services[0]) {
        rmw_request_id_t request_header;
        auto req = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
        auto resp = std::make_shared<example_interfaces::srv::AddTwoInts::Response>();

        ret = rcl_take_request(service_handle_.get(), &request_header, req.get());
        if (ret == RCL_RET_OK) {
          handleRequest(req, resp);
          // ToDo: response_handler ??
          ret = rcl_send_response(service_handle_.get(), &request_header, resp.get());
          if (ret != RCL_RET_OK) {
            std::cerr << "Failed to send response" << std::endl;
          }
        }
      }
    }

    rcl_ret_t ret_fini = rcl_wait_set_fini(&wait_set);
    if (ret_fini != RCL_RET_OK) {
      throw std::runtime_error("Failed to fini wait set");
    }
  }

private:
  std::unique_ptr<rcl_service_t> service_handle_;
  rcl_node_t * node_handle_;
  rcl_context_t context_;

  void handleRequest(
    const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> req,
    std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> resp)
  {
    resp->sum = req->a + req->b;
    std::cout << "[MyService::handleRequest] Request: a=" << req->a << ", b=" << req->b
              << " -> Response: sum=" << resp->sum << std::endl;
  }
};

}  // namespace rclmine
