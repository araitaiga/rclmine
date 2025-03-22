#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <rcl_interfaces/msg/parameter.hpp>
#include <rcl_interfaces/msg/parameter_descriptor.hpp>
#include <rcl_interfaces/msg/parameter_type.hpp>
#include <rcl_interfaces/msg/parameter_value.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>
#include <rcl_interfaces/srv/describe_parameters.hpp>
#include <rcl_interfaces/srv/get_parameters.hpp>
#include <rcl_interfaces/srv/list_parameters.hpp>
#include <rcl_interfaces/srv/set_parameters.hpp>
#include <stdexcept>
#include <string>
#include <vector>

#include "rclmine/my_node.h"
#include "rclmine/my_parameter.h"

namespace rclmine
{

using rcl_interfaces::msg::SetParametersResult;
class MyParameterService
{
private:
  using GetParametersService = rcl_interfaces::srv::GetParameters;
  using SetParametersService = rcl_interfaces::srv::SetParameters;
  using ListParametersService = rcl_interfaces::srv::ListParameters;
  using DescribeParametersService = rcl_interfaces::srv::DescribeParameters;

  // パラメータストレージへの参照
  std::shared_ptr<MyParameter> parameter_storage_;

public:
  MyParameterService(std::shared_ptr<MyNode> node, std::shared_ptr<MyParameter> parameter_storage)
  : parameter_storage_(parameter_storage)
  {
    if (!parameter_storage_) {
      throw std::runtime_error("Invalid parameter storage");
    }

    // GetParametersサービスの登録
    // https://docs.ros2.org/foxy/api/rcl_interfaces/srv/GetParameters.html
    auto get_parameters_handler = [this](rcl_service_t * service) {
      rmw_request_id_t request_header;
      auto req = std::make_shared<GetParametersService::Request>();
      auto resp = std::make_shared<GetParametersService::Response>();

      rcl_ret_t ret = rcl_take_request(service, &request_header, req.get());
      if (ret == RCL_RET_OK) {
        resp->values.reserve(req->names.size());
        for (const auto & name : req->names) {
          if (parameter_storage_->has_parameter(name)) {
            resp->values.push_back(parameter_storage_->get_parameter(name));
          } else {
            std::cout << "Parameter not found: " << name << std::endl;
          }
        }

        ret = rcl_send_response(service, &request_header, resp.get());
        if (ret != RCL_RET_OK) {
          std::cerr << "Failed to send response" << std::endl;
        }
      }
    };
    node->createService<GetParametersService>("get_parameters", get_parameters_handler);

    // SetParametersサービスの登録
    // https://docs.ros2.org/foxy/api/rcl_interfaces/srv/SetParameters.html
    auto set_parameters_handler = [this](rcl_service_t * service) {
      rmw_request_id_t request_header;
      auto req = std::make_shared<SetParametersService::Request>();
      auto resp = std::make_shared<SetParametersService::Response>();

      rcl_ret_t ret = rcl_take_request(service, &request_header, req.get());
      if (ret == RCL_RET_OK) {
        for (const auto & param : req->parameters) {
          SetParametersResult result;
          result.successful = parameter_storage_->set_parameter(param.name, param.value);

          if (!result.successful) {
            result.reason = "Failed to set parameter " + param.name;
          }
          resp->results.push_back(result);
        }

        ret = rcl_send_response(service, &request_header, resp.get());
        if (ret != RCL_RET_OK) {
          std::cerr << "Failed to send response" << std::endl;
        }
      }
    };
    node->createService<SetParametersService>("set_parameters", set_parameters_handler);

    // ListParametersサービスの登録
    // https://docs.ros2.org/foxy/api/rcl_interfaces/srv/ListParameters.html
    auto list_parameters_handler = [this](rcl_service_t * service) {
      rmw_request_id_t request_header;
      auto req = std::make_shared<ListParametersService::Request>();
      auto resp = std::make_shared<ListParametersService::Response>();

      rcl_ret_t ret = rcl_take_request(service, &request_header, req.get());
      if (ret == RCL_RET_OK) {
        auto all_params = parameter_storage_->get_parameter_names();
        resp->result.names = all_params;

        ret = rcl_send_response(service, &request_header, resp.get());
        if (ret != RCL_RET_OK) {
          std::cerr << "Failed to send response" << std::endl;
        }
      }
    };
    node->createService<ListParametersService>("list_parameters", list_parameters_handler);

    // DescribeParametersサービスの登録
    auto describe_parameters_handler = [this](rcl_service_t * service) {
      rmw_request_id_t request_header;
      auto req = std::make_shared<DescribeParametersService::Request>();
      auto resp = std::make_shared<DescribeParametersService::Response>();

      rcl_ret_t ret = rcl_take_request(service, &request_header, req.get());
      if (ret == RCL_RET_OK) {
        // discriptorは未実装
        resp->descriptors.reserve(req->names.size());
        ret = rcl_send_response(service, &request_header, resp.get());
        if (ret != RCL_RET_OK) {
          std::cerr << "Failed to send response" << std::endl;
        }
      }
    };
    node->createService<DescribeParametersService>(
      "describe_parameters", describe_parameters_handler);

    std::cout << "[MyParameterService::Constructor] MyParameterService constructor" << std::endl;
  }
};

}  // namespace rclmine
