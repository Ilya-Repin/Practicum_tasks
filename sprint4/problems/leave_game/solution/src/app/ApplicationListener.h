#pragma once
#include <chrono>
#include "Application.h"

namespace app {

class ApplicationListener {
 public:
  virtual void OnTick(std::chrono::milliseconds time_delta_ms, std::shared_ptr<Application> app) = 0;
};

} // namespace app
