#include "logger.h"

void LoggerFormatter(logging::record_view const &rec, logging::formatting_ostream &strm) {
  json::value data = {{"timestamp", to_iso_extended_string(*rec[timestamp])},
                      {"data", *rec[additional_data]},
                      {"message", *rec[boost::log::expressions::smessage]}};

  strm << data;
}

void LoggerInit() {
  logging::add_common_attributes();

  logging::add_console_log(
      std::cout,
      keywords::format = &LoggerFormatter,
      keywords::auto_flush = true);
}

void Log(json::value &data, std::string_view msg) {
  BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, data) << msg;
}
