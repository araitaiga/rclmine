#pragma once
#include <rcl/node.h>
#include <rcl/rcl.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <std_msgs/msg/string.hpp>
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
  MyNode(const std::string & node_name, const std::string & name_space, rcl_context_t context);
  ~MyNode();

  void createPublisher(const std::string & topic_name);
  void publish(const std::string & msg_content);

  void createSubscription(const std::string & topic_name);
  void subscribe();

  void createService(const std::string & service_name);
  void service();

  void createClient(const std::string & service_name);
  void sendRequest(int64_t a, int64_t b);

private:
  std::unique_ptr<rcl_node_t> node_handle_;
  std::unique_ptr<MyPublisher> publisher_;
  std::unique_ptr<MySubscription> subscription_;
  std::unique_ptr<MyService> service_;
  std::unique_ptr<MyClient> client_;
  rcl_context_t context_;
};

}  // namespace rclmine
