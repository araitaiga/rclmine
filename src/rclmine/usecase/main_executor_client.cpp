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
    std::cout << "[Main] Create Client" << std::endl;
    auto client = node.createClient<example_interfaces::srv::AddTwoInts>(
      "arai_service", [](rcl_client_t * client) {
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
      });

    rclmine::MyExecutor executor(context);
    executor.addMyNode(&node);

    example_interfaces::srv::AddTwoInts::Request req;
    req.a = 1;
    req.b = 2;
    client->request(req);

    std::cout << "[Main] Start client" << std::endl;

    while (true) {
      executor.spin();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  std::cout << "[Main] Node destroyed" << std::endl;

  rclmine::RCLUtils::shutdown(context);

  return 0;
}
