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
class MySubscription
{
public:
  // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/subscription.h#L121-L140
  MySubscription(rcl_node_t * node_handle, const std::string & topic_name, rcl_context_t context)
  : node_handle_(node_handle), context_(context)
  {
    std::cout << "[MySubscription::Constructor] MySubscription constructor" << std::endl;
    subscription_handle_ =
      std::make_unique<rcl_subscription_t>(rcl_get_zero_initialized_subscription());
    subscription_handle2_ =
      std::make_unique<rcl_subscription_t>(rcl_get_zero_initialized_subscription());

    rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();

    // auto type_support =
    //   rosidl_typesupport_c__get_message_type_support_handle__std_msgs__msg__String(); // でも可
    // const rosidl_message_type_support_t * type_support =
    //   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String); // でも可

    // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/subscription.h#L98-L103
    const rosidl_message_type_support_t * type_support =
      rosidl_typesupport_cpp::get_message_type_support_handle<std_msgs::msg::String>();

    rcl_ret_t ret = rcl_subscription_init(
      subscription_handle_.get(), node_handle_, type_support, topic_name.c_str(),
      &subscription_options);
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize subscription");
    }

    rcl_ret_t ret2 = rcl_subscription_init(
      subscription_handle2_.get(), node_handle_, type_support, "arai_test", &subscription_options);
    if (ret2 != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize subscription2");
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

  void subscribe()
  {
    // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/wait.h#L96-L104
    rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();

    //rcl_wait_set_init(
    // rcl_wait_set_t * wait_set,
    // size_t number_of_subscriptions,
    // size_t number_of_guard_conditions,
    // size_t number_of_timers,
    // size_t number_of_clients,
    // size_t number_of_services,
    // size_t number_of_events,
    // rcl_context_t * context,
    // rcl_allocator_t allocator)
    auto ret =
      rcl_wait_set_init(&wait_set, 2, 1, 0, 0, 0, 0, &context_, rcutils_get_default_allocator());
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize wait set");
    }

    // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/guard_condition.h#L64-L74
    rcl_guard_condition_t guard_condition = rcl_get_zero_initialized_guard_condition();
    auto ret_gc = rcl_guard_condition_init(
      &guard_condition, &context_, rcl_guard_condition_get_default_options());
    if (ret_gc != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize guard condition");
    }

    // https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/wait.h#L406-L433
    // 経過時間が一定時間を超えた場合, ループを抜ける
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    size_t duration = 1000;

    while (
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start)
        .count() < duration) {
      ret = rcl_wait_set_clear(&wait_set);
      if (ret != RCL_RET_OK) {
        throw std::runtime_error("Failed to clear wait set");
      }
      ret = rcl_wait_set_add_subscription(&wait_set, subscription_handle_.get(), nullptr);
      if (ret != RCL_RET_OK) {
        throw std::runtime_error("Failed to add subscription to wait set");
      }
      ret = rcl_wait_set_add_subscription(&wait_set, subscription_handle2_.get(), nullptr);
      if (ret != RCL_RET_OK) {
        throw std::runtime_error("Failed to add subscription2 to wait set");
      }

      ret = rcl_wait_set_add_guard_condition(&wait_set, &guard_condition, nullptr);
      if (ret != RCL_RET_OK) {
        throw std::runtime_error("Failed to add guard condition to wait set");
      }

      ret = rcl_wait(&wait_set, RCL_MS_TO_NS(1000));  // wait for 1s

      if (ret == RCL_RET_TIMEOUT) {
        std::cout << "[MyNode::subscribe] Timeout" << std::endl;
        continue;
      }

      for (size_t i = 0; i < wait_set.size_of_subscriptions; i++) {
        if (wait_set.subscriptions[i]) {
          // std_msgs__msg__String * msg = std_msgs__msg__String__create();
          // C++ style
          auto msg = std::make_unique<std_msgs::msg::String>();

          rmw_message_info_t message_info;
          // wait_set.subscriptions[i] == subscription_handle_.get() or subscription_handle2_.get()
          // auto ret_take =
          //   rcl_take(wait_set.subscriptions[i], static_cast<void *>(msg), &message_info, nullptr);
          // C++ style
          auto ret_take = rcl_take(wait_set.subscriptions[i], msg.get(), &message_info, nullptr);
          if (ret_take == RCL_RET_OK) {
            // std::cout << "[MyNode::subscribe] Received: " << msg->data.data << std::endl;
            // C++ style
            std::cout << "[MyNode::subscribe] Received: " << msg->data << std::endl;
          }
          // std_msgs__msg__String__destroy(msg);
          // C++ style
          msg.reset();
        }
      }

      for (size_t i = 0; i < wait_set.size_of_guard_conditions; i++) {
        if (wait_set.guard_conditions[i]) {
          std::cout << "[MyNode::subscribe] Guard condition triggered" << std::endl;
        }
      }
    }

    auto ret_gc_fini = rcl_guard_condition_fini(&guard_condition);
    if (ret_gc_fini != RCL_RET_OK) {
      throw std::runtime_error("Failed to fini guard condition");
    }

    auto ret_fini = rcl_wait_set_fini(&wait_set);
    if (ret_fini != RCL_RET_OK) {
      throw std::runtime_error("Failed to fini wait set");
    }
  }

private:
  std::unique_ptr<rcl_subscription_t> subscription_handle_;
  std::unique_ptr<rcl_subscription_t> subscription_handle2_;
  rcl_node_t * node_handle_;
  rcl_context_t context_;
};

}  // namespace rclmine
