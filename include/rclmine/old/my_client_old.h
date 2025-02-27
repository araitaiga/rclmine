#pragma once
#include <rcl/allocator.h>
#include <rcl/client.h>
#include <rcl/node.h>
#include <rcl/rcl.h>

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
class MyClient
{
public:
  MyClient(rcl_node_t * node_handle, const std::string & service_name, rcl_context_t context)
  : node_handle_(node_handle), context_(context)
  {
    std::cout << "[MyClient::Constructor] MyClient constructor" << std::endl;
    client_handle_ = std::make_unique<rcl_client_t>(rcl_get_zero_initialized_client());
    rcl_client_options_t client_options = rcl_client_get_default_options();

    const rosidl_service_type_support_t * type_support =
      rosidl_typesupport_cpp::get_service_type_support_handle<
        example_interfaces::srv::AddTwoInts>();

    rcl_ret_t ret = rcl_client_init(
      client_handle_.get(), node_handle_, type_support, service_name.c_str(), &client_options);

    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize client");
    }

    std::cout << "[MyClient::Constructor] Client is constructed" << std::endl;
  }

  ~MyClient()
  {
    rcl_ret_t ret_client_fini = rcl_client_fini(client_handle_.get(), node_handle_);
    if (ret_client_fini != RCL_RET_OK) {
      std::cerr << "Failed to fini client" << std::endl;
    }
    std::cout << "[MyClient::Destructor] Client is destructed" << std::endl;
  }

  void sendRequest(int64_t a, int64_t b)
  {
    // requestの型とclientの型が一致している必要がある
    auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
    request->a = a;
    request->b = b;

    // requestメッセージは呼び出し中に変更してはいけない
    rcl_ret_t ret = rcl_send_request(client_handle_.get(), request.get(), &sequence_number_);
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to send request");
    }

    std::cout << "[MyClient::sendRequest] Request sent: a=" << a << ", b=" << b << std::endl;

    // wait for response
    rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
    ret = rcl_wait_set_init(&wait_set, 0, 0, 0, 1, 0, 0, &context_, rcl_get_default_allocator());
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize wait set");
    }

    while (true) {
      ret = rcl_wait_set_clear(&wait_set);
      if (ret != RCL_RET_OK) {
        throw std::runtime_error("Failed to clear wait set");
      }

      ret = rcl_wait_set_add_client(&wait_set, client_handle_.get(), nullptr);
      if (ret != RCL_RET_OK) {
        throw std::runtime_error("Failed to add client to wait set");
      }

      ret = rcl_wait(&wait_set, RCL_MS_TO_NS(1000));  // wait for 1s
      if (ret == RCL_RET_TIMEOUT) {
        std::cout << "[MyClient::sendRequest] Timeout" << std::endl;
        continue;
      }

      if (wait_set.clients[0]) {
        rmw_request_id_t request_header;  // request_idが埋め込まれて返ってくる
        example_interfaces::srv::AddTwoInts::Response response;

        ret = rcl_take_response(client_handle_.get(), &request_header, &response);
        if (ret == RCL_RET_OK) {
          std::cout << "[MyClient::sendRequest] Response " << request_header.sequence_number
                    << " received: sum=" << response.sum << std::endl;
          break;
        } else {
          std::cerr << "Failed to receive response" << std::endl;
        }
      }
    }

    rcl_ret_t ret_fini = rcl_wait_set_fini(&wait_set);
    if (ret_fini != RCL_RET_OK) {
      throw std::runtime_error("Failed to fini wait set");
    }
  }

private:
  std::unique_ptr<rcl_client_t> client_handle_;
  rcl_node_t * node_handle_;
  rcl_context_t context_;
  int64_t sequence_number_;
};

}  // namespace rclmine
