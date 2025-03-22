#include <rcl/init.h>

#include <chrono>
#include <std_msgs/msg/string.hpp>
#include <thread>

#include "rclmine/my_executor.h"
#include "rclmine/my_node.h"
#include "rclmine/my_parameter_service.h"
#include "rclmine/rcl_utils.h"

int main(int argc, char ** argv)
{
  rcl_context_t * context = rclmine::RCLUtils::init(argc, argv);

  {
    std::shared_ptr<rclmine::MyNode> node =
      std::make_shared<rclmine::MyNode>("arai_node", "arai_namespace", context);
    std::cout << "[Main] Create ParameterService" << std::endl;

    std::shared_ptr<rclmine::MyParameter> parameter_storage =
      std::make_shared<rclmine::MyParameter>();
    rclmine::MyParameterService parameter_service(node, parameter_storage);

    rclmine::MyExecutor executor(context);
    executor.addMyNode(node.get());

    std::cout << "[Main] Start Service" << std::endl;

    while (true) {
      executor.spin();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  std::cout << "[Main] Node destroyed" << std::endl;
  rclmine::RCLUtils::shutdown(context);
  return 0;
}
