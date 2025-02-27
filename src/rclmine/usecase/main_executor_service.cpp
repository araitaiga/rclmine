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
    std::cout << "[Main] Create Service" << std::endl;

    auto service_callback = [](rcl_service_t * service) {
      std::cout << "[MyExecutor::handleService] Start handling service" << std::endl;
      rmw_request_id_t request_header;
      auto req = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
      auto resp = std::make_shared<example_interfaces::srv::AddTwoInts::Response>();

      rcl_ret_t ret = rcl_take_request(service, &request_header, req.get());
      if (ret == RCL_RET_OK) {
        resp->sum = req->a + req->b;
        std::cout << "[MyExecutor::handleRequest] Request: a=" << req->a << ", b=" << req->b
                  << " -> Response: sum=" << resp->sum << std::endl;

        ret = rcl_send_response(service, &request_header, resp.get());
        if (ret != RCL_RET_OK) {
          std::cerr << "Failed to send response" << std::endl;
        }
      }
    };
    node.createService<example_interfaces::srv::AddTwoInts>("arai_service", service_callback);

    rclmine::MyExecutor executor(context);
    executor.addMyNode(&node);

    std::cout << "[Main] Start Service" << std::endl;

    while (true) {
      executor.spin();
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
