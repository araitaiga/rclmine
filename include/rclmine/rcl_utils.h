#pragma once
#include <rcl/rcl.h>

#include <iostream>
#include <stdexcept>

namespace rclmine
{
class RCLUtils
{
public:
  static rcl_context_t init(int argc, char ** argv)
  {
    std::cout << "[RCLUtils] start main" << std::endl;
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    std::cout << "[RCLUtils] Done rcl_get_zero_initialized_init_options()" << std::endl;
    rcl_ret_t opt_init_ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
    if (opt_init_ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize init options");
    }
    std::cout << "[RCLUtils] Done rcl_get_zero_initialized_context()" << std::endl;
    rcl_context_t context = rcl_get_zero_initialized_context();
    std::cout << "[RCLUtils] Done rcl_init_options_init()" << std::endl;
    rcl_ret_t init_ret = rcl_init(argc, argv, &init_options, &context);
    if (init_ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to initialize context");
    }
    std::cout << "[RCLUtils] Done rcl_init()" << std::endl;
    return context;
  }

  static void shutdown(rcl_context_t context)
  {
    rcl_ret_t fini_ret = rcl_shutdown(&context);
    if (fini_ret != RCL_RET_OK) {
      throw std::runtime_error("Failed to shutdown context");
    }
    std::cout << "[RCLUtils] Done rcl_shutdown()" << std::endl;
  }
};
}  // namespace rclmine
