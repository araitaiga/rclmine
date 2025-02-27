#pragma once
#include <rcl/node.h>
#include <rcl/rcl.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include "rclmine/my_client.h"
#include "rclmine/my_publisher.h"
#include "rclmine/my_service.h"
#include "rclmine/my_subscription.h"

namespace rclmine
{
class MyNode
{
private:
  std::shared_ptr<rcl_node_t> node_handle_;
  std::vector<std::shared_ptr<BaseSubscription>> subscriptions_;
  std::vector<std::shared_ptr<BaseService>> services_;
  std::vector<std::shared_ptr<BaseClient>> clients_;
  rcl_context_t context_;

public:
  MyNode(const std::string & node_name, const std::string & name_space, rcl_context_t context)
  {
    context_ = context;
    std::cout << "[MyNode::Constructor] MyNode constructor" << std::endl;

    node_handle_ = std::make_shared<rcl_node_t>(rcl_get_zero_initialized_node());
    std::cout << "[MyNode::Constructor] MyNode handle created" << std::endl;

    auto node_options = rcl_node_get_default_options();

    std::cout << "[MyNode::Constructor] Context and MyNode Options are initialized" << std::endl;

    rcl_ret_t ret = rcl_node_init(
      node_handle_.get(), node_name.c_str(), name_space.c_str(), &context, &node_options);

    if (ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize node");
    }
    std::cout << "[MyNode::Constructor] MyNode is constructed" << std::endl;
  }
  ~MyNode()
  {
    subscriptions_.clear();
    services_.clear();
    clients_.clear();

    rcl_ret_t ret_nodeinit = rcl_node_fini(node_handle_.get());
    if (ret_nodeinit != RCL_RET_OK) {
      std::cerr << "Failed to fini node" << std::endl;
    }

    std::cout << "[MyNode::Destructor] MyNode is destructed" << std::endl;
  }

  template <typename MessageT>
  std::shared_ptr<MyPublisher<MessageT>> createPublisher(const std::string & topic_name)
  {
    const auto publisher = std::make_shared<MyPublisher<MessageT>>(node_handle_.get(), topic_name);
    return publisher;
  }

  template <typename MessageT>
  void createSubscription(
    const std::string & topic_name, std::function<void(rcl_subscription_t *)> callback)
  {
    auto subscription = std::make_shared<MySubscription<MessageT>>(
      node_handle_.get(), topic_name, context_, callback);
    subscriptions_.push_back(subscription);
  }

  std::vector<SubscriptionCallbackPair> getSubscriptions()
  {
    std::vector<SubscriptionCallbackPair> subscription_handles;
    for (const auto & subscription : subscriptions_) {
      subscription_handles.push_back(subscription->getSubscription());
    }
    return subscription_handles;
  }

  template <typename MessageT>
  void createService(
    const std::string & service_name, std::function<void(rcl_service_t *)> callback)
  {
    auto service =
      std::make_shared<MyService<MessageT>>(node_handle_.get(), service_name, context_, callback);
    services_.push_back(service);
  }
  std::vector<ServiceCallbackPair> getServices()
  {
    std::vector<ServiceCallbackPair> service_handles;
    for (const auto & service : services_) {
      service_handles.push_back(service->getService());
    }
    return service_handles;
  }

  template <typename MessageT>
  std::shared_ptr<MyClient<MessageT>> createClient(const std::string & service_name)
  {
    auto client = std::make_shared<MyClient<MessageT>>(node_handle_.get(), service_name, context_);
    clients_.push_back(client);
    return client;
  }
  std::vector<std::shared_ptr<rcl_client_t>> getClients()
  {
    std::vector<std::shared_ptr<rcl_client_t>> client_handles;
    for (const auto & client : clients_) {
      client_handles.push_back(client->getClient());
    }
    return client_handles;
  }
};

}  // namespace rclmine
