#pragma once
#include <rcl/node.h>
#include <rcl/rcl.h>
#include <rcl/subscription.h>
// #include <rosidl_runtime_c/string_functions.h>
// #include <std_msgs/msg/string.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <rosidl_typesupport_cpp/message_type_support.hpp>
#include <std_msgs/msg/string.hpp>
#include <stdexcept>
#include <string>
#include <thread>

namespace rclmine
{

struct SubscriptionCallbackPair
{
  rcl_subscription_t * subscription;
  std::function<void(rcl_subscription_t *)> callback;
};

// context
// https://github.com/ros2/rcl/blob/bfcf66d0d8beece02c195999afe2eb5daeef0412/rcl/include/rcl/context.h#L113-L150
// contextのargumentの中身
// https://github.com/ros2/rcl/blob/bfcf66d0d8beece02c195999afe2eb5daeef0412/rcl/src/rcl/arguments_impl.h#L29-L72
// 解説記事
// https://zenn.dev/uedake/articles/ros2_node5_context#%E3%82%B0%E3%83%AD%E3%83%BC%E3%83%90%E3%83%ABros%E5%BC%95%E6%95%B0%E3%81%8C%E4%BF%9D%E6%8C%81%E3%81%99%E3%82%8B%E6%83%85%E5%A0%B1

class BaseSubscription
{
public:
  virtual ~BaseSubscription() = default;
  virtual SubscriptionCallbackPair getSubscription() = 0;
};

template <typename MessageT>
class MySubscription : public BaseSubscription
{
public:
  // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/subscription.h#L121-L140
  MySubscription(
    rcl_node_t * node_handle, const std::string & topic_name, rcl_context_t context,
    std::function<void(rcl_subscription_t *)> callback)
  : node_handle_(node_handle), context_(context), callback_(callback)
  {
    std::cout << "[MySubscription::Constructor] MySubscription constructor" << std::endl;
    subscription_handle_ =
      std::make_shared<rcl_subscription_t>(rcl_get_zero_initialized_subscription());

    rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();

    // auto type_support =
    //   rosidl_typesupport_c__get_message_type_support_handle__std_msgs__msg__String(); // でも可
    // const rosidl_message_type_support_t * type_support =
    //   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String); // でも可

    // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/subscription.h#L98-L103
    const rosidl_message_type_support_t * type_support =
      rosidl_typesupport_cpp::get_message_type_support_handle<MessageT>();

    rcl_ret_t ret = rcl_subscription_init(
      subscription_handle_.get(), node_handle_, type_support, topic_name.c_str(),
      &subscription_options);
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize subscription");
    }

    std::cout << "[MySubscription::Constructor] Subscription is constructed" << std::endl;
  }

  ~MySubscription()
  {
    rcl_ret_t ret_subinit = rcl_subscription_fini(subscription_handle_.get(), node_handle_);
    if (ret_subinit != RCL_RET_OK) {
      std::cerr << "Failed to fini subscription" << std::endl;
    }
    std::cout << "[MySubscription::Destructor] Subscription is destructed" << std::endl;
  }

  SubscriptionCallbackPair getSubscription() override
  {
    SubscriptionCallbackPair pair;
    pair.subscription = subscription_handle_.get();
    pair.callback = callback_;
    return pair;
  }

private:
  std::shared_ptr<rcl_subscription_t> subscription_handle_;
  std::function<void(rcl_subscription_t *)> callback_;
  rcl_node_t * node_handle_;
  rcl_context_t context_;
};

}  // namespace rclmine
