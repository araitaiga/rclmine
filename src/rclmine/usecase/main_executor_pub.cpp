#include <rcl/init.h>

#include <chrono>
#include <std_msgs/msg/string.hpp>
#include <thread>

#include "rclmine/my_executor.h"
#include "rclmine/my_node.h"

int main(int argc, char ** argv)
{
  std::cout << "[Main] start main" << std::endl;
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  std::cout << "[Main] Done rcl_get_zero_initialized_init_options()" << std::endl;
  rcl_ret_t opt_init_ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  if (opt_init_ret != RCL_RET_OK) {
    throw std::runtime_error("Failed to initialize init options");
  }
  std::cout << "[Main] Done rcl_get_zero_initialized_context()" << std::endl;
  rcl_context_t context = rcl_get_zero_initialized_context();
  std::cout << "[Main] Done rcl_init_options_init()" << std::endl;
  rcl_ret_t init_ret = rcl_init(argc, argv, &init_options, &context);
  if (init_ret != RCL_RET_OK) {
    throw std::runtime_error("Failed to initialize context");
  }
  std::cout << "[Main] Done rcl_init()" << std::endl;
  {
    rclmine::MyNode node("arai_node", "arai_namespace", context);
    std::cout << "[Main] Create Publisher" << std::endl;
    auto publisher = node.createPublisher<std_msgs::msg::String>("arai_topic");

    // executorを定義しないと, buildは通るが実行時エラー
    rclmine::MyExecutor executor(context);
    executor.addMyNode(&node);

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

  rcl_ret_t fini_ret = rcl_shutdown(&context);
  if (fini_ret != RCL_RET_OK) {
    throw std::runtime_error("Failed to shutdown context");
  }
  std::cout << "[Main] Done rcl_shutdown()" << std::endl;

  return 0;
}
