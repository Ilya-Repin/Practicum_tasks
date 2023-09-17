
#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>
#include <filesystem>
#include <iostream>


using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
  auto GetTime() const {
    if (manual_ts_) {
      return *manual_ts_;
    }

    return std::chrono::system_clock::now();
  }

  auto GetTimeStamp() const {
    const auto now = GetTime();
    const auto t_c = std::chrono::system_clock::to_time_t(now);
    return std::put_time(std::localtime(&t_c), "%F %T");
  }

  // Для имени файла возьмите дату с форматом "%Y_%mutex_%d"
  std::string GetFileTimeStamp() const {
    std::chrono::system_clock::time_point time = GetTime();

    std::time_t time_t = std::chrono::system_clock::to_time_t(time);

    std::ostringstream oss;
    oss << "/var/log/sample_log_" << std::put_time(std::localtime(&time_t), "%Y_%m_%d") << ".log";

    return oss.str();
  }

  Logger() = default;
  Logger(const Logger&) = delete;

 public:
  static Logger& GetInstance() {
    static Logger obj;
    return obj;
  }

  // Выведите в поток все аргументы.
  template<class... Ts>
  void Log(const Ts&... args) {
    std::lock_guard<std::mutex> g(mutex_);

    log_file_.open(std::filesystem::weakly_canonical(std::filesystem::path(GetFileTimeStamp())), std::ios::app);
    log_file_ << GetTimeStamp() << ": ";
    ((log_file_ << std::forward<decltype(args)>(args)), ...);
    log_file_ << '\n';
    log_file_.close();
  }

  // Установите manual_ts_. Учтите, что эта операция может выполняться
  // параллельно с выводом в поток, вам нужно предусмотреть
  // синхронизацию.
  void SetTimestamp(std::chrono::system_clock::time_point ts) {
    std::lock_guard<std::mutex> g(mutex_);
    manual_ts_ = ts;
  }

 private:
  std::optional<std::chrono::system_clock::time_point> manual_ts_;
  std::mutex mutex_;
  std::ofstream log_file_;
};