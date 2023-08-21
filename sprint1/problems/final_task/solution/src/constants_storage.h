#pragma once
#include <string_view>

using namespace std::literals;

boost::string_view inline MakeBoostSV(std::string_view sv) {
  return {sv.data(), sv.length()};
}

struct JsonAttributes {
  JsonAttributes() = delete;

  constexpr static std::string_view ID = "id"sv;
  constexpr static std::string_view NAME = "name"sv;

  constexpr static std::string_view X = "x"sv;
  constexpr static std::string_view Y = "y"sv;

  constexpr static std::string_view X0 = "x0"sv;
  constexpr static std::string_view Y0 = "y0"sv;

  constexpr static std::string_view X1 = "x1"sv;
  constexpr static std::string_view Y1 = "y1"sv;

  constexpr static std::string_view OFFSET_X = "offsetX"sv;
  constexpr static std::string_view OFFSET_Y = "offsetY"sv;

  constexpr static std::string_view WIDTH = "w"sv;
  constexpr static std::string_view HEIGHT = "h"sv;

  constexpr static std::string_view ROADS = "roads"sv;
  constexpr static std::string_view BUILDINGS = "buildings"sv;
  constexpr static std::string_view OFFICES = "offices"sv;

  constexpr static std::string_view CODE = "code"sv;
  constexpr static std::string_view MESSAGE = "message"sv;
};

struct ContentType {
  ContentType() = delete;

  constexpr static std::string_view APPLICATION_JSON = "application/json"sv;
  constexpr static std::string_view TEXT_HTML = "text/html"sv;
};

struct HttpStatus {
  HttpStatus() = delete;

  constexpr static std::string_view CODE_400 = "badRequest"sv;
  constexpr static std::string_view MESSAGE_400 = "Bad request"sv;

  constexpr static std::string_view CODE_MAP_404 = "mapNotFound"sv;
  constexpr static std::string_view MESSAGE_MAP_404 = "Map not found"sv;

  constexpr static std::string_view MESSAGE_INVALID_METHOD = "Invalid method"sv;
};

struct SystemPaths {
  SystemPaths() = delete;

  constexpr static std::string_view MAPS = "/api/v1/maps"sv;
};