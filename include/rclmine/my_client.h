#pragma once
#include <rcl/allocator.h>
#include <rcl/client.h>
#include <rcl/node.h>
#include <rcl/rcl.h>

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

struct ServiceClientCallbackPair
{
  rcl_client_t * client;
  std::function<void(rcl_client_t *)> callback;
};

class BaseClient
{
public:
  virtual ~BaseClient() = default;
  virtual ServiceClientCallbackPair getClient() = 0;
};

template <typename MessageT>
class MyClient : public BaseClient
{
public:
  MyClient(
    rcl_node_t * node_handle, const std::string & service_name,
    std::function<void(rcl_client_t *)> callback)
  : node_handle_(node_handle), callback_(callback)
  {
    std::cout << "[MyClient::Constructor] MyClient constructor" << std::endl;
    client_handle_ = std::make_shared<rcl_client_t>(rcl_get_zero_initialized_client());
    rcl_client_options_t client_options = rcl_client_get_default_options();

    const rosidl_service_type_support_t * type_support =
      rosidl_typesupport_cpp::get_service_type_support_handle<MessageT>();

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

  void request(const typename MessageT::Request & req_msg)
  {
    int64_t sequence_number;
    rcl_ret_t ret = rcl_send_request(client_handle_.get(), &req_msg, &sequence_number);
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to send request");
    }
    std::cout << "[MyClient::request] Request sent" << std::endl;
  }

  ServiceClientCallbackPair getClient() override
  {
    ServiceClientCallbackPair pair;
    pair.client = client_handle_.get();
    pair.callback = callback_;
    return pair;
  }

private:
  std::shared_ptr<rcl_client_t> client_handle_;
  std::function<void(rcl_client_t *)> callback_;
  rcl_node_t * node_handle_;
};

}  // namespace rclmine
