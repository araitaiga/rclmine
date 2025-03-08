#include <rcl/init.h>

#include <chrono>
#include <std_msgs/msg/string.hpp>
#include <thread>

#include "rclmine/my_executor.h"
#include "rclmine/my_node.h"
#include "rclmine/rcl_utils.h"

int main(int argc, char ** argv)
{
  rcl_context_t * context = rclmine::RCLUtils::init(argc, argv);

  {
    rclmine::MyNode node("arai_node", "arai_namespace", context);
    std::cout << "[Main] Create Publisher" << std::endl;
    auto publisher = node.createPublisher<std_msgs::msg::String>("arai_topic");

    std::cout << "[Main] Start publish" << std::endl;

    while (true) {
      std_msgs::msg::String msg;
      msg.data = "Hello, arai!!!";
      publisher->publish(msg);
      std::cout << "[Main] Message published: " << msg.data << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  std::cout << "[Main] Node destroyed" << std::endl;

  rclmine::RCLUtils::shutdown(context);

  return 0;
}
