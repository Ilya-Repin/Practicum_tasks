#include "my_logger.h"

std::string Logger::GetFileTimeStamp() const {
  std::ostringstream buf;
  buf << GetTimeStamp();
  auto ts = buf.str();

  auto res = ts.substr(0, ts.find_first_of(" "));
  std::replace(res.begin(), res.end(), '-', '_');

  return res;
}

void Logger::SetTimestamp(std::chrono::system_clock::time_point ts) {

  std::lock_guard<std::mutex> lock_guard(mutex_);
  manual_ts_ = ts;
}

void Logger::OpenFile() {
  if (log_file_.is_open()) {
    log_file_.flush();
    log_file_.close();
  }
  log_file_.open(GetFileTimeStamp(), std::ios::out | std::ios::app);
  if(!log_file_.is_open()) {
    throw std::runtime_error("fail to open/create log file");
  }
}