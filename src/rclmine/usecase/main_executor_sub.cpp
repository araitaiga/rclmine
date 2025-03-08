#include <rcl/init.h>

#include <std_msgs/msg/string.hpp>

#include "rclmine/my_executor.h"
#include "rclmine/my_node.h"
#include "rclmine/rcl_utils.h"

int main(int argc, char ** argv)
{
  rcl_context_t * context = rclmine::RCLUtils::init(argc, argv);

  {
    rclmine::MyNode node("arai_node", "arai_namespace", context);
    std::cout << "[Main] Create Subscriber" << std::endl;

    auto subscription_callback = [](rcl_subscription_t * subscription) {
      std::cout << "[MyExecutor::handleSubscription] Start handling subscription" << std::endl;
      auto msg = std::make_shared<std_msgs::msg::String>();
      rmw_message_info_t message_info;
      rcl_ret_t ret = rcl_take(subscription, msg.get(), &message_info, nullptr);
      if (ret == RCL_RET_OK) {
        std::cout << "[MyExecutor::handleSubscription] Message received: " << msg->data
                  << std::endl;
      } else {
        std::cerr << "Failed to take message" << std::endl;
      }
      msg.reset();
    };

    node.createSubscription<std_msgs::msg::String>("arai_topic", subscription_callback);

    rclmine::MyExecutor executor(context);
    executor.addMyNode(&node);

    std::cout << "[Main] Start executor" << std::endl;

    while (true) {
      executor.spin();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  std::cout << "[Main] Node destroyed" << std::endl;

  rclmine::RCLUtils::shutdown(context);

  return 0;
}
