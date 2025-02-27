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
public:
  MyNode(const std::string & node_name, const std::string & name_space, rcl_context_t context)
  {
    context_ = context;
    std::cout << "[MyNode::Constructor] MyNode constructor" << std::endl;

    node_handle_ = std::make_unique<rcl_node_t>(rcl_get_zero_initialized_node());
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
    service_.reset();

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
  void createSubscription(const std::string & topic_name)
  {
    auto subscription =
      std::make_shared<MySubscription<MessageT>>(node_handle_.get(), topic_name, context_);
    subscriptions_.push_back(subscription);
  }

  std::vector<std::shared_ptr<rcl_subscription_t>> getSubscriptions()
  {
    std::vector<std::shared_ptr<rcl_subscription_t>> subscription_handles;
    for (const auto & subscription : subscriptions_) {
      subscription_handles.push_back(subscription->getSubscription());
    }
    return subscription_handles;
  }

  void createService(const std::string & service_name)
  {
    service_ = std::make_unique<MyService>(node_handle_.get(), service_name, context_);
  }
  std::shared_ptr<rcl_service_t> getService()
  {
    if (service_) {
      return service_->getService();
    } else {
      return nullptr;
    }
  }

  void createClient(const std::string & service_name)
  {
    client_ = std::make_unique<MyClient>(node_handle_.get(), service_name, context_);
  }
  std::shared_ptr<rcl_client_t> getClient()
  {
    if (client_) {
      return client_->getClient();
    } else {
      return nullptr;
    }
  }

private:
  std::unique_ptr<rcl_node_t> node_handle_;
  std::vector<std::shared_ptr<BaseSubscription>> subscriptions_;
  std::unique_ptr<MyService> service_;
  std::unique_ptr<MyClient> client_;
  rcl_context_t context_;
};

}  // namespace rclmine
