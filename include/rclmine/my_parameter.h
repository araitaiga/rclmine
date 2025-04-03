#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <rcl_interfaces/msg/parameter_type.hpp>
#include <rcl_interfaces/msg/parameter_value.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace rclmine
{
// rcl_interfaces
// https://github.com/ros2/rcl_interfaces/blob/rolling/rcl_interfaces/README.md

// rclcppではパラメータの型をラップしている
// https://github.com/ros2/rclcpp/blob/1564fc23c62d7966db835c3e5a4c6e3ad0012288/rclcpp/include/rclcpp/parameter_value.hpp#L33-L45
// rclcppではパラメータ値を保持する構造体をラップしている. コンストラクタで自動的に型を判別できる
// https://github.com/ros2/rclcpp/blob/1564fc23c62d7966db835c3e5a4c6e3ad0012288/rclcpp/include/rclcpp/parameter_value.hpp#L72-L357

using rcl_interfaces::msg::ParameterType;
using rcl_interfaces::msg::ParameterValue;
class MyParameter
{
private:
  // https://github.com/ros2/rcl_interfaces/blob/rolling/rcl_interfaces/msg/ParameterValue.msg
  std::map<std::string, ParameterValue> parameters_;
  // std::vector<std::function<void(const std::string &, const ParameterValue &)>> callbacks_;

public:
  explicit MyParameter() {}

  // パラメータを宣言
  // rclcppではコンストラクタのオーバーロードを使用してc++プリミティブ型を引数として呼び出せるようにしている
  // https://github.com/ros2/rclcpp/blob/1564fc23c62d7966db835c3e5a4c6e3ad0012288/rclcpp/src/rclcpp/parameter_value.cpp#L136-L140
  // https://github.com/ros2/rclcpp/blob/1564fc23c62d7966db835c3e5a4c6e3ad0012288/rclcpp/src/rclcpp/node_interfaces/node_parameters.cpp#L574-L579
  bool declare_parameter(const std::string & name, const ParameterValue & default_value)
  {
    if (parameters_.find(name) != parameters_.end()) {
      return false;
    }
    parameters_[name] = default_value;
    return true;
  }

  // パラメータ値を取得
  ParameterValue get_parameter(const std::string & name) const
  {
    auto it = parameters_.find(name);
    if (it == parameters_.end()) {
      // 例外をスロー
      // https://github.com/ros2/rclcpp/blob/1564fc23c62d7966db835c3e5a4c6e3ad0012288/rclcpp/include/rclcpp/parameter_value.hpp#L152
      throw std::runtime_error("Parameter not declared: " + name);
    }
    return it->second;
  }

  // パラメータの存在確認
  bool has_parameter(const std::string & name) const
  {
    return parameters_.find(name) != parameters_.end();
  }

  // パラメータ値を設定
  bool set_parameter(const std::string & name, const ParameterValue & value)
  {
    auto it = parameters_.find(name);
    if (it == parameters_.end()) {
      std::cout << "Parameter is not declared: " << name << std::endl;
      return false;
    }

    // 型チェック
    uint8_t current_type = get_parameter_type(it->second);
    uint8_t new_type = get_parameter_type(value);
    if (current_type != new_type) {
      std::cout << "Parameter type mismatch: " << name
                << " Current type: " << static_cast<int>(current_type)
                << " New type: " << static_cast<int>(new_type) << std::endl;
      return false;
    }

    std::cout << "Set parameter: " << name << std::endl;
    // 値を更新
    parameters_[name] = value;

    // // コールバックを呼び出す
    // for (const auto & callback : callbacks_) {
    //   callback(name, value);
    // }

    return true;
  }

  // パラメータのリストを取得
  std::vector<std::string> get_parameter_names() const
  {
    std::vector<std::string> names;
    for (const auto & param : parameters_) {
      names.push_back(param.first);
    }
    return names;
  }

  // // パラメータイベントのコールバックを追加する例
  // // https://github.com/ros2/rclcpp/blob/1564fc23c62d7966db835c3e5a4c6e3ad0012288/rclcpp/include/rclcpp/parameter_event_handler.hpp#L233-L252
  // void add_parameter_callback(
  //   std::function<void(const std::string &, const ParameterValue &)> callback)
  // {
  //   callbacks_.push_back(callback);
  // }

  // https://docs.ros2.org/foxy/api/rcl_interfaces/msg/ParameterValue.html
  // [ParameterValue]
  // uint8 type
  // bool bool_value
  // int64 integer_value
  // float64 double_value
  // string string_value
  // byte[] byte_array_value
  // bool[] bool_array_value
  // int64[] integer_array_value
  // float64[] double_array_value
  // string[] string_array_value

  // ParameterValueの型を取得するユーティリティ関数
  static uint8_t get_parameter_type(const ParameterValue & value)
  {
    if (value.type == 0) {
      return ParameterType::PARAMETER_BOOL;
    } else if (value.type == 1) {
      return ParameterType::PARAMETER_INTEGER;
    } else if (value.type == 2) {
      return ParameterType::PARAMETER_DOUBLE;
    } else if (value.type == 3) {
      return ParameterType::PARAMETER_STRING;
    } else if (value.type == 4) {
      return ParameterType::PARAMETER_BYTE_ARRAY;
    } else if (value.type == 5) {
      return ParameterType::PARAMETER_BOOL_ARRAY;
    } else if (value.type == 6) {
      return ParameterType::PARAMETER_INTEGER_ARRAY;
    } else if (value.type == 7) {
      return ParameterType::PARAMETER_DOUBLE_ARRAY;
    } else if (value.type == 8) {
      return ParameterType::PARAMETER_STRING_ARRAY;
    }
    return ParameterType::PARAMETER_NOT_SET;
  }

  // bool型パラメータの値を取得
  bool get_parameter_bool(const std::string & name) const
  {
    auto value = get_parameter(name);
    if (get_parameter_type(value) != ParameterType::PARAMETER_BOOL) {
      throw std::runtime_error("Parameter is not of type bool: " + name);
    }
    return value.bool_value;
  }

  // int型パラメータの値を取得
  int64_t get_parameter_int(const std::string & name) const
  {
    auto value = get_parameter(name);
    if (get_parameter_type(value) != ParameterType::PARAMETER_INTEGER) {
      throw std::runtime_error("Parameter is not of type integer: " + name);
    }
    return value.integer_value;
  }

  // double型パラメータの値を取得
  double get_parameter_double(const std::string & name) const
  {
    auto value = get_parameter(name);
    if (get_parameter_type(value) != ParameterType::PARAMETER_DOUBLE) {
      throw std::runtime_error("Parameter is not of type double: " + name);
    }
    return value.double_value;
  }

  // string型パラメータの値を取得
  std::string get_parameter_string(const std::string & name) const
  {
    auto value = get_parameter(name);
    if (get_parameter_type(value) != ParameterType::PARAMETER_STRING) {
      throw std::runtime_error("Parameter is not of type string: " + name);
    }
    return value.string_value;
  }

  // // bool型パラメータを設定する関数の例
  // bool set_parameter_bool(const std::string & name, bool value)
  // {
  //   ParameterValue param_value;
  //   param_value.bool_value = value;
  //   return set_parameter(name, param_value);
  // }
};
}  // namespace rclmine
