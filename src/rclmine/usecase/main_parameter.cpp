#include <rcl/init.h>

#include <chrono>
#include <std_msgs/msg/string.hpp>
#include <thread>

#include "rclmine/my_executor.h"
#include "rclmine/my_node.h"
#include "rclmine/my_parameter_service.h"
#include "rclmine/rcl_utils.h"

// set_parametersのリクエスト例
// ros2 service call /arai_namespace/set_parameters rcl_interfaces/srv/SetParameters "{parameters: [{name: 'arai_bool_param', value: {type: 0, bool_value: false}}]}"
// requester: making request: rcl_interfaces.srv.SetParameters_Request(parameters=[rcl_interfaces.msg.Parameter(name='arai_bool_param', value=rcl_interfaces.msg.ParameterValue(type=0, bool_value=False, integer_value=0, double_value=0.0, string_value='', byte_array_value=[], bool_array_value=[], integer_array_value=[], double_array_value=[], string_array_value=[]))])
// response:
// rcl_interfaces.srv.SetParameters_Response(results=[rcl_interfaces.msg.SetParametersResult(successful=True, reason='')])

// get_parametersのリクエスト例
// ros2 service call /arai_namespace/get_parameters rcl_interfaces/srv/GetParameters "{names: ['arai_int_param', 'arai_bool_param']}"
// requester: making request: rcl_interfaces.srv.GetParameters_Request(names=['arai_int_param', 'arai_bool_param'])
// response:
// rcl_interfaces.srv.GetParameters_Response(values=[rcl_interfaces.msg.ParameterValue(type=1, bool_value=False, integer_value=813, double_value=0.0, string_value='', byte_array_value=[], bool_array_value=[], integer_array_value=[], double_array_value=[], string_array_value=[]), rcl_interfaces.msg.ParameterValue(type=0, bool_value=False, integer_value=0, double_value=0.0, string_value='', byte_array_value=[], bool_array_value=[], integer_array_value=[], double_array_value=[], string_array_value=[])])

int main(int argc, char ** argv)
{
  rcl_context_t * context = rclmine::RCLUtils::init(argc, argv);

  {
    std::shared_ptr<rclmine::MyNode> node =
      std::make_shared<rclmine::MyNode>("arai_node", "arai_namespace", context);
    std::cout << "[Main] Create ParameterService" << std::endl;

    std::shared_ptr<rclmine::MyParameter> parameter_storage =
      std::make_shared<rclmine::MyParameter>();

    rcl_interfaces::msg::ParameterValue param_value_int;
    param_value_int.type = 1;
    param_value_int.integer_value = 813;
    parameter_storage->declare_parameter("arai_int_param", param_value_int);

    rcl_interfaces::msg::ParameterValue param_value_bool;
    param_value_bool.type = 0;
    param_value_bool.bool_value = true;
    parameter_storage->declare_parameter("arai_bool_param", param_value_bool);

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
