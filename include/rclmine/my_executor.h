#pragma once
#include <rcl/allocator.h>
#include <rcl/client.h>
#include <rcl/node.h>
#include <rcl/publisher.h>
#include <rcl/rcl.h>
#include <rcl/service.h>
#include <rcl/subscription.h>

#include <chrono>
#include <example_interfaces/srv/add_two_ints.hpp>
#include <iostream>
#include <memory>
// get_message_type_support_handle<>()を使う場合 (C++ style)
#include <rosidl_typesupport_cpp/message_type_support.hpp>
#include <rosidl_typesupport_cpp/service_type_support.hpp>
#include <std_msgs/msg/string.hpp>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "rclmine/my_node.h"

namespace rclmine
{
class MyExecutor
{
private:
  rcl_context_t * context_;
  // service,client,pub,subなどのイベント監視のデータセット
  rcl_wait_set_t wait_set_;

  std::vector<SubscriptionCallbackPair> subscriptions_;
  std::vector<rcl_guard_condition_t *> guard_conditions_;
  std::vector<rcl_timer_t *> timers_;
  std::vector<ServiceClientCallbackPair> clients_;
  std::vector<ServiceCallbackPair> services_;
  std::vector<rcl_event_t *> events_;

public:
  MyExecutor(rcl_context_t * context) : context_(context)
  {
    std::cout << "[MyExecutor::Constructor] MyExecutor constructor" << std::endl;
    wait_set_ = rcl_get_zero_initialized_wait_set();
    std::cout << "[MyExecutor::Constructor] Wait set is initialized" << std::endl;
  }

  ~MyExecutor()
  {
    rcl_ret_t ret = rcl_wait_set_fini(&wait_set_);
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to fini wait set");
    }
    std::cout << "[MyExecutor::Destructor] Wait set is destructed" << std::endl;
  }

  void addMyNode(MyNode * my_node)
  {
    if (!my_node->getSubscriptions().empty()) {
      addSubscriptions(my_node->getSubscriptions());
    }
    if (!my_node->getClients().empty()) {
      addClients(my_node->getClients());
    }
    if (!my_node->getServices().empty()) {
      addServices(my_node->getServices());
    }

    auto ret = rcl_wait_set_init(
      &wait_set_, subscriptions_.size(), guard_conditions_.size(), timers_.size(), clients_.size(),
      services_.size(), events_.size(), context_, rcl_get_default_allocator());
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize wait set");
    }
  }

  void spin()
  {
    auto ret = rcl_wait_set_clear(&wait_set_);
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to clear wait set");
    }

    for (auto subscription : subscriptions_) {
      auto subscription_handle = subscription.subscription;
      rcl_wait_set_add_subscription(&wait_set_, subscription_handle, nullptr);
    }
    for (auto client : clients_) {
      auto client_handle = client.client;
      rcl_wait_set_add_client(&wait_set_, client_handle, nullptr);
    }
    for (auto service : services_) {
      auto service_handle = service.service;
      rcl_wait_set_add_service(&wait_set_, service_handle, nullptr);
    }

    // wait
    ret = rcl_wait(&wait_set_, RCL_MS_TO_NS(1000));  // wait for 1s
    if (ret == RCL_RET_TIMEOUT) {
      std::cout << "[MyExecutor::spin] Timeout..." << std::endl;
      return;
    }

    // subscriptionの処理
    for (size_t i = 0; i < subscriptions_.size(); ++i) {
      if (wait_set_.subscriptions[i]) {
        auto callback = subscriptions_[i].callback;
        callback(subscriptions_[i].subscription);
      }
    }

    // servieの処理
    for (size_t i = 0; i < services_.size(); ++i) {
      if (wait_set_.services[i]) {
        auto callback = services_[i].callback;
        callback(services_[i].service);
      }
    }

    // clientの処理
    for (size_t i = 0; i < clients_.size(); ++i) {
      if (wait_set_.clients[i]) {
        auto callback = clients_[i].callback;
        callback(clients_[i].client);
      }
    }
  }

private:
  void addSubscriptions(std::vector<SubscriptionCallbackPair> subscriptions)
  {
    subscriptions_.insert(subscriptions_.end(), subscriptions.begin(), subscriptions.end());
  }

  void addClients(std::vector<ServiceClientCallbackPair> clients)
  {
    clients_.insert(clients_.end(), clients.begin(), clients.end());
  }

  void addServices(std::vector<ServiceCallbackPair> services)
  {
    // services_の末尾にservicesを追加
    services_.insert(services_.end(), services.begin(), services.end());
  }
};

}  // namespace rclmine
