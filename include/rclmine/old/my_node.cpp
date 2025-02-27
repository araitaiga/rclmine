#include "rclmine/my_node.h"

namespace rclmine
{
// https://github.com/ros2/rcl/blob/3ea07c7e853aa51f843c1ba686927352b85fc5e1/rcl/include/rcl/node.h#L106-L116
MyNode::MyNode(const std::string & node_name, const std::string & name_space, rcl_context_t context)
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

void MyNode::createPublisher(const std::string & topic_name)
{
  publisher_ = std::make_unique<MyPublisher>(node_handle_.get(), topic_name);
}
void MyNode::publish(const std::string & msg_content)
{
  if (!publisher_) return;
  publisher_->publish(msg_content);
}

void MyNode::createSubscription(const std::string & topic_name)
{
  subscription_ = std::make_unique<MySubscription>(node_handle_.get(), topic_name, context_);
}

void MyNode::subscribe()
{
  if (!subscription_) return;
  subscription_->subscribe();
}

void MyNode::createService(const std::string & service_name)
{
  service_ = std::make_unique<MyService>(node_handle_.get(), service_name, context_);
}

void MyNode::service()
{
  if (!service_) return;
  service_->service();
}

void MyNode::createClient(const std::string & service_name)
{
  client_ = std::make_unique<MyClient>(node_handle_.get(), service_name, context_);
}

void MyNode::sendRequest(int64_t a, int64_t b)
{
  if (!client_) return;
  client_->sendRequest(a, b);
}

MyNode::~MyNode()
{
  publisher_.reset();
  subscription_.reset();
  service_.reset();

  rcl_ret_t ret_nodeinit = rcl_node_fini(node_handle_.get());
  if (ret_nodeinit != RCL_RET_OK) {
    std::cerr << "Failed to fini node" << std::endl;
  }

  std::cout << "[MyNode::Destructor] MyNode is destructed" << std::endl;
}

}  // namespace rclmine
