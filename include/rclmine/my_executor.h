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
public:
  MyExecutor(rcl_context_t context) : context_(context)
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
    // if (my_node->getGuardCondition()) {
    //   addGuardCondition(my_node->getGuardCondition().get());
    // }
    // if (my_node->getTimer()) {
    //   addTimer(my_node->getTimer().get());
    // }
    if (my_node->getClient()) {
      addClient(my_node->getClient().get());
    }
    if (my_node->getService()) {
      addService(my_node->getService().get());
    }
    // if (my_node->getEvent()) {
    //   addEvent(my_node->getEvent().get());
    // }

    auto ret = rcl_wait_set_init(
      &wait_set_, subscriptions_.size(), guard_conditions_.size(), timers_.size(), clients_.size(),
      services_.size(), events_.size(), &context_, rcl_get_default_allocator());
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
      rcl_wait_set_add_subscription(&wait_set_, subscription, nullptr);
    }
    // for (auto guard_condition : guard_conditions_) {
    //   rcl_wait_set_add_guard_condition(&wait_set_, guard_condition);
    // }
    // for (auto timer : timers_) {
    //   rcl_wait_set_add_timer(&wait_set_, timer);
    // }
    for (auto client : clients_) {
      rcl_wait_set_add_client(&wait_set_, client, nullptr);
    }
    for (auto service : services_) {
      rcl_wait_set_add_service(&wait_set_, service, nullptr);
    }
    // for (auto event : events_) {
    //   rcl_wait_set_add_event(&wait_set_, event, nullptr);
    // }

    // wait
    ret = rcl_wait(&wait_set_, RCL_MS_TO_NS(1000));  // wait for 1s
    if (ret == RCL_RET_TIMEOUT) {
      std::cout << "[MyExecutor::spin] Timeout..." << std::endl;
      return;
    }

    // subscriptionの処理
    for (size_t i = 0; i < subscriptions_.size(); ++i) {
      if (wait_set_.subscriptions[i]) {
        handleSubscription(subscriptions_[i]);
      }
    }

    // guard_conditionの処理
    for (size_t i = 0; i < guard_conditions_.size(); ++i) {
      if (wait_set_.guard_conditions[i]) {
        std::cout << "[MyExecutor::spin] Guard condition is triggered" << std::endl;
      }
    }

    // servieの処理
    for (size_t i = 0; i < services_.size(); ++i) {
      if (wait_set_.services[i]) {
        handleService(services_[i]);
      }
    }

    // clientの処理
    for (size_t i = 0; i < clients_.size(); ++i) {
      if (wait_set_.clients[i]) {
        handleClient(clients_[i]);
      }
    }
  }

private:
  rcl_context_t context_;
  // service,client,pub,subなどのイベント監視のデータセット
  rcl_wait_set_t wait_set_;

  std::vector<rcl_subscription_t *> subscriptions_;
  std::vector<rcl_guard_condition_t *> guard_conditions_;
  std::vector<rcl_timer_t *> timers_;
  std::vector<rcl_client_t *> clients_;
  std::vector<rcl_service_t *> services_;
  std::vector<rcl_event_t *> events_;

  void addSubscriptions(std::vector<std::shared_ptr<rcl_subscription_t>> subscriptions)
  {
    for (auto subscription : subscriptions) {
      subscriptions_.push_back(subscription.get());
    }
  }

  void addGuardCondition(rcl_guard_condition_t * guard_condition)
  {
    guard_conditions_.push_back(guard_condition);
  }
  void addTimer(rcl_timer_t * timer) { timers_.push_back(timer); }
  void addClient(rcl_client_t * client) { clients_.push_back(client); }
  void addService(rcl_service_t * service) { services_.push_back(service); }
  void addEvent(rcl_event_t * event) { events_.push_back(event); }

  void publish(rcl_publisher_t * publisher)
  {
    std::cout << "[MyExecutor::publish] Start publishing" << std::endl;
    std_msgs::msg::String msg;
    msg.data = "Hello, arai!!!";
    rcl_publish(publisher, &msg, nullptr);
    std::cout << "[MyExecutor::publish] Message published: " << msg.data << std::endl;
  }

  void handleSubscription(rcl_subscription_t * subscription)
  {
    std::cout << "[MyExecutor::handleSubscription] Start handling subscription" << std::endl;
    auto msg = std::make_shared<std_msgs::msg::String>();
    rmw_message_info_t message_info;
    rcl_ret_t ret = rcl_take(subscription, msg.get(), &message_info, nullptr);
    if (ret == RCL_RET_OK) {
      std::cout << "[MyExecutor::handleSubscription] Message received: " << msg->data << std::endl;
    } else {
      std::cerr << "Failed to take message" << std::endl;
    }
    msg.reset();
  }

  void handleService(rcl_service_t * service)
  {
    std::cout << "[MyExecutor::handleService] Start handling service" << std::endl;
    rmw_request_id_t request_header;
    auto req = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
    auto resp = std::make_shared<example_interfaces::srv::AddTwoInts::Response>();

    rcl_ret_t ret = rcl_take_request(service, &request_header, req.get());
    if (ret == RCL_RET_OK) {
      processRequest(req, resp);

      ret = rcl_send_response(service, &request_header, resp.get());
      if (ret != RCL_RET_OK) {
        std::cerr << "Failed to send response" << std::endl;
      }
    }
  }

  void processRequest(
    const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> req,
    std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> resp)
  {
    resp->sum = req->a + req->b;
    std::cout << "[MyExecutor::handleRequest] Request: a=" << req->a << ", b=" << req->b
              << " -> Response: sum=" << resp->sum << std::endl;
  }

  void request(rcl_client_t * client)
  {
    // requestの型とclientの型が一致している必要がある
    example_interfaces::srv::AddTwoInts::Request req;
    req.a = 1;
    req.b = 2;
    int64_t sequence_number;
    // requestメッセージは呼び出し中に変更してはいけない
    rcl_ret_t ret = rcl_send_request(client, &req, &sequence_number);
    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to send request");
    }

    std::cout << "[MyClient::sendRequest] Request sent: a=" << req.a << ", b=" << req.b
              << std::endl;
  }

  void handleClient(rcl_client_t * client)
  {
    std::cout << "[MyExecutor::handleClient] Start handling client" << std::endl;
    rmw_request_id_t request_header;  // request_idが埋め込まれて返ってくる
    example_interfaces::srv::AddTwoInts::Response response;
    rcl_ret_t ret = rcl_take_response(client, &request_header, &response);
    if (ret == RCL_RET_OK) {
      std::cout << "[MyExecutor::handleClient] Response " << request_header.sequence_number
                << " received: sum=" << response.sum << std::endl;
    } else {
      std::cerr << "Failed to take response" << std::endl;
    }
  };
};

}  // namespace rclmine
