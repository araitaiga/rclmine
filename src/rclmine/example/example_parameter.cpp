#include <rcl/error_handling.h>
#include <rcl/node.h>
#include <rcl/node_options.h>
#include <rcl/parameters_c.h>
#include <rcl/rcl.h>
#include <rcl_interfaces/msg/parameter_type.h>

#include <iostream>
#include <memory>
#include <string>

void check_ret(rcl_ret_t ret, const char * error_msg)
{
  if (ret != RCL_RET_OK) {
    std::cerr << error_msg << ": " << rcl_get_error_string().str << std::endl;
    rcl_reset_error();
    exit(1);
  }
}

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  rcl_ret_t ret;
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_node_t node = rcl_get_zero_initialized_node();

  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  check_ret(ret, "Failed to init options");

  ret = rcl_init(0, NULL, &init_options, &context);
  check_ret(ret, "Failed to init rcl");

  rcl_node_options_t node_options = rcl_node_get_default_options();
  ret = rcl_node_init(&node, "parameter_node", "", &context, &node_options);
  check_ret(ret, "Failed to init node");

  // パラメータを扱うためにrclパラメータインターフェースを初期化
  rcl_params_t * params = rcl_yaml_node_struct_init(rcl_get_default_allocator());
  if (!params) {
    std::cerr << "Failed to initialize params struct" << std::endl;
    return 1;
  }

  // 整数型パラメータを設定
  int64_t int_value = 42;
  rcl_variant_t int_variant;
  memset(&int_variant, 0, sizeof(rcl_variant_t));
  int_variant.integer_value = static_cast<int64_t *>(malloc(sizeof(int64_t)));
  *int_variant.integer_value = int_value;

  ret = rcl_yaml_node_struct_set(params, "parameter_node", "my_int_param", &int_variant);
  check_ret(ret, "Failed to set integer parameter in params");

  // 文字列パラメータを設定
  const char * str_value = "hello_world";
  rcl_variant_t str_variant;
  memset(&str_variant, 0, sizeof(rcl_variant_t));
  str_variant.string_value = strdup(str_value);

  ret = rcl_yaml_node_struct_set(params, "parameter_node", "my_string_param", &str_variant);
  check_ret(ret, "Failed to set string parameter in params");

  // ブールパラメータを設定
  bool bool_value = true;
  rcl_variant_t bool_variant;
  memset(&bool_variant, 0, sizeof(rcl_variant_t));
  bool_variant.bool_value = static_cast<bool *>(malloc(sizeof(bool)));
  *bool_variant.bool_value = bool_value;

  ret = rcl_yaml_node_struct_set(params, "parameter_node", "my_bool_param", &bool_variant);
  check_ret(ret, "Failed to set bool parameter in params");

  // パラメータをノードに適用
  ret = rcl_node_set_parameters_atomically(&node, params);
  check_ret(ret, "Failed to set parameters");

  // パラメータを取得
  rcl_variant_t * get_int_param = NULL;
  ret = rcl_node_get_parameter(&node, "my_int_param", &get_int_param);
  if (ret == RCL_RET_OK && get_int_param && get_int_param->integer_value) {
    std::cout << "my_int_param: " << *get_int_param->integer_value << std::endl;
  } else {
    std::cerr << "Failed to get integer parameter: " << rcl_get_error_string().str << std::endl;
    rcl_reset_error();
  }

  rcl_variant_t * get_str_param = NULL;
  ret = rcl_node_get_parameter(&node, "my_string_param", &get_str_param);
  if (ret == RCL_RET_OK && get_str_param && get_str_param->string_value) {
    std::cout << "my_string_param: " << get_str_param->string_value << std::endl;
  } else {
    std::cerr << "Failed to get string parameter: " << rcl_get_error_string().str << std::endl;
    rcl_reset_error();
  }

  rcl_variant_t * get_bool_param = NULL;
  ret = rcl_node_get_parameter(&node, "my_bool_param", &get_bool_param);
  if (ret == RCL_RET_OK && get_bool_param && get_bool_param->bool_value) {
    std::cout << "my_bool_param: " << (*get_bool_param->bool_value ? "true" : "false") << std::endl;
  } else {
    std::cerr << "Failed to get bool parameter: " << rcl_get_error_string().str << std::endl;
    rcl_reset_error();
  }

  // パラメータ値の変更
  int64_t new_int_value = 100;
  rcl_variant_t new_int_variant;
  memset(&new_int_variant, 0, sizeof(rcl_variant_t));
}
return 0;
check_ret(ret, "Failed to finalize context");
ret = rcl_context_fini(&context);
check_ret(ret, "Failed to shutdown rcl");
ret = rcl_shutdown(&context);
check_ret(ret, "Failed to finalize node");
ret = rcl_node_fini(
  &node);  // 後処理  rcl_yaml_node_struct_fini(params);  // RCLパラメータ構造体のクリーンアップ    free(new_int_variant.integer_value);  free(bool_variant.bool_value);  free(str_variant.string_value);  free(int_variant.integer_value);  // メモリ解放  }    rcl_reset_error();    std::cerr << "Failed to get updated parameter: " << rcl_get_error_string().str << std::endl;  } else {    std::cout << "Updated my_int_param: " << *get_updated_param->integer_value << std::endl;  if (ret == RCL_RET_OK && get_updated_param && get_updated_param->integer_value) {  ret = rcl_node_get_parameter(&node, "my_int_param", &get_updated_param);  rcl_variant_t * get_updated_param = NULL;  // 変更後の値を取得  check_ret(ret, "Failed to update parameters");  ret = rcl_node_set_parameters_atomically(&node, params);    check_ret(ret, "Failed to set updated integer parameter");  ret = rcl_yaml_node_struct_set(params, "parameter_node", "my_int_param", &new_int_variant);    *new_int_variant.integer_value = new_int_value;  new_int_variant.integer_value = static_cast<int64_t*>(malloc(sizeof(int64_t)));
