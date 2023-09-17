#include "request_handler.h"

namespace http_handler {
StringResponse MakeStringResponse(http::status status, std::string_view body, size_t http_version,
                                  bool keep_alive,
                                  std::string_view content_type) {
  StringResponse response(status, http_version);
  response.set(http::field::content_type, content_type);
  response.body() = body;
  response.content_length(body.size());
  response.keep_alive(keep_alive);

  return response;
}

std::string RequestHandler::DecodeURL(std::string_view target) {
  std::string path;
  path.reserve(target.size());

  for (size_t i = 0; i < target.size(); ++i) {
    char letter = target[i];

    if (letter == '+') {
      letter = ' ';
    } else if (letter == '%') {
      ++i;
      std::string substr(target.substr(i, 2));
      int code = std::stoi(substr, nullptr, 16);
      ++i;
      letter = static_cast<char>(code);
    }

    path += letter;
  }

  return path;
}

fs::path RequestHandler::GetPath(std::string_view url) {
  return fs::weakly_canonical(static_path_.string() + DecodeURL(url));
}

bool RequestHandler::CheckFileInRoot(fs::path &path) {
  int result = path.string().compare(0, static_path_.string().length(), static_path_.string());

  return result == 0;
}

bool RequestHandler::CheckFileExistence(fs::path &path) {
  return fs::exists(path);
}

}  // http_handler
