#pragma once

#include <chrono>
#include "model_serialization.h"
#include "../app/Application.h"
#include "../app/ApplicationListener.h"

namespace infrastructure {
using namespace std::literals;

 class SerializingListener : public app::ApplicationListener {
 public:
  SerializingListener(const std::string &state_file,
                      std::chrono::milliseconds save_state_period) : state_file_(state_file), save_state_period_(save_state_period) {
  }

  // Methods
  void OnTick(std::chrono::milliseconds time_delta_ms, std::shared_ptr<app::Application> app) override;
  void SaveState(std::shared_ptr<app::Application> app);
  void LoadState(std::shared_ptr<app::Application> app);

 private:
  std::chrono::milliseconds save_state_period_;
  std::chrono::milliseconds awaiting_time_ = 0ms;
  std::string state_file_;
};

} // namespace infrastructure