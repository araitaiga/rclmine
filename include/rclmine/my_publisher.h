#pragma once
#include <rcl/node.h>
#include <rcl/publisher.h>
#include <rcl/rcl.h>

// 以下のマクロを使う場合 (C style)
// const rosidl_message_type_support_t * type_support =
//       ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);
// #include <rosidl_runtime_c/string_functions.h>
// #include <std_msgs/msg/string.h>

#include <chrono>
#include <iostream>
#include <memory>
// get_message_type_support_handle<>()を使う場合 (C++ style)
#include <rosidl_typesupport_cpp/message_type_support.hpp>
#include <stdexcept>
#include <string>
#include <thread>

namespace rclmine
{
template <typename MessageT>
class MyPublisher
{
public:
  // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/publisher.h#L112-L130
  MyPublisher(rcl_node_t * node_handle, const std::string & topic_name) : node_handle_(node_handle)
  {
    std::cout << "[MyPublisher::Constructor] MyPublisher constructor" << std::endl;
    publisher_handle_ = std::make_shared<rcl_publisher_t>(rcl_get_zero_initialized_publisher());
    rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();

    // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/publisher.h#L91-L96
    const rosidl_message_type_support_t * type_support =
      rosidl_typesupport_cpp::get_message_type_support_handle<MessageT>();

    rcl_ret_t ret = rcl_publisher_init(
      publisher_handle_.get(), node_handle_, type_support, topic_name.c_str(), &publisher_options);
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize publisher");
    }

    std::cout << "[MyPublisher::Constructor] Publisher is constructed" << std::endl;
  }

  ~MyPublisher()
  {
    rcl_ret_t ret_pubinit = rcl_publisher_fini(publisher_handle_.get(), node_handle_);
    if (ret_pubinit != RCL_RET_OK) {
      std::cerr << "Failed to fini publisher" << std::endl;
    }
    std::cout << "[MyPublisher::Destructor] Publisher is destructed" << std::endl;
  }

  void publish(const MessageT & msg)
  {
    rcl_ret_t ret = rcl_publish(publisher_handle_.get(), &msg, nullptr);
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to publish message");
    }
    std::cout << "[MyPublisher::publish] Message published" << std::endl;
  }

  std::shared_ptr<rcl_publisher_t> getPublisher() { return publisher_handle_; }

private:
  std::shared_ptr<rcl_publisher_t> publisher_handle_;
  rcl_node_t * node_handle_;
};

}  // namespace rclmine
