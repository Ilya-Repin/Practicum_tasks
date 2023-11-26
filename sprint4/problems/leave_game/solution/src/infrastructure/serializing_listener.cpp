#include "serializing_listener.h"
#include "../logger/logger.h"
#include "application_serialization.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <filesystem>

namespace infrastructure {

void SerializingListener::OnTick(std::chrono::milliseconds time_delta_ms, std::shared_ptr<app::Application> app) {
  awaiting_time_ += time_delta_ms;

  if (awaiting_time_ >= save_state_period_) {
    SaveState(app);
    awaiting_time_ = 0ms;
  }
}

void SerializingListener::SaveState(std::shared_ptr<app::Application> app) {
  if (!state_file_.empty()) {
  try {
    std::ofstream archive{state_file_};
    boost::archive::text_oarchive output_archive{archive};
    output_archive << serialization::AppRepr(app);
    archive.close();
  }
  catch (const std::exception &ex) {
    boost::json::value custom_data{{"code"s, "EXIT_FAILURE"}, {"exception", ex.what()}};
    Log(custom_data, "error during saving state"sv);

    throw;
  }}
}

void SerializingListener::LoadState(std::shared_ptr<app::Application> app) {
  if (std::filesystem::exists(state_file_)) {
    try {
      std::ifstream archive(state_file_);
      boost::archive::text_iarchive input_archive(archive);
      serialization::AppRepr repr;
      input_archive >> repr;
      repr.Restore(app);
      archive.close();
    } catch (const std::exception &ex) {
      boost::json::value custom_data{{"code"s, "EXIT_FAILURE"}, {"exception", ex.what()}};
      Log(custom_data, "error during loading state"sv);

      throw;
    }
  }
}

} // namespace infrastructure
