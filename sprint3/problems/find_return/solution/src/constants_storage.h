#pragma once

#include <string_view>
#include <unordered_map>
#include <boost/json.hpp>

namespace constants {

using namespace std::literals;

boost::string_view inline MakeBoostSV(std::string_view sv) {
  return {sv.data(), sv.length()};
}

struct GameSettings {
  GameSettings() = delete;

  static constexpr double LANE_WIDTH = 0.4;
  static constexpr double DOG_WIDTH = 0.6;
  static constexpr double BASE_WIDTH = 0.5;
  static constexpr double LOOT_WIDTH = 0;
};

struct JsonAttributes {

  JsonAttributes() = delete;

  constexpr static std::string_view
  ID = "id"sv;
  constexpr static std::string_view
  NAME = "name"sv;
  constexpr static std::string_view
  TYPE = "type"sv;
  constexpr static std::string_view
  FILE = "file"sv;
  constexpr static std::string_view
  ROTATION = "rotation"sv;
  constexpr static std::string_view
  COLOR = "color"sv;
  constexpr static std::string_view
  SCALE = "scale"sv;

  constexpr static std::string_view
  X = "x"sv;
  constexpr static std::string_view
  Y = "y"sv;

  constexpr static std::string_view
  X0 = "x0"sv;
  constexpr static std::string_view
  Y0 = "y0"sv;

  constexpr static std::string_view
  X1 = "x1"sv;
  constexpr static std::string_view
  Y1 = "y1"sv;

  constexpr static std::string_view
  OFFSET_X = "offsetX"sv;
  constexpr static std::string_view
  OFFSET_Y = "offsetY"sv;

  constexpr static std::string_view
  WIDTH = "w"sv;
  constexpr static std::string_view
  HEIGHT = "h"sv;

  constexpr static std::string_view
  MAPS = "maps"sv;
  constexpr static std::string_view
  ROADS = "roads"sv;
  constexpr static std::string_view
  BUILDINGS = "buildings"sv;
  constexpr static std::string_view
  OFFICES = "offices"sv;

  constexpr static std::string_view
  CODE = "code"sv;
  constexpr static std::string_view
  MESSAGE = "message"sv;

  constexpr static std::string_view
  LOOT_GENERATOR_CONFIG = "lootGeneratorConfig"sv;
  constexpr static std::string_view
  LOOT_CONFIG_PERIOD = "period"sv;
  constexpr static std::string_view
  LOOT_CONFIG_PROBABILITY = "probability"sv;

  constexpr static std::string_view
  DOG_SPEED = "dogSpeed"sv;
  constexpr static std::string_view
  DEFAULT_DOG_SPEED = "defaultDogSpeed"sv;

  constexpr static std::string_view
  BAG_CAPACITY = "bagCapacity"sv;
  constexpr static std::string_view
  DEFAULT_BAG_CAPACITY = "defaultBagCapacity"sv;

  constexpr static std::string_view
  USERNAME = "userName"sv;
  constexpr static std::string_view
  MAP_ID = "mapId"sv;

  constexpr static std::string_view
  TOKEN = "authToken"sv;
  constexpr static std::string_view
  PLAYER_ID = "playerId"sv;

  constexpr static std::string_view
  POS = "pos"sv;
  constexpr static std::string_view
  SPEED = "speed"sv;
  constexpr static std::string_view
  DIR = "dir"sv;
  constexpr static std::string_view
  BAG = "bag"sv;
  constexpr static std::string_view
  PLAYERS = "players"sv;
  constexpr static std::string_view
  LOST_OBJECTS = "lostObjects"sv;
  constexpr static std::string_view
  LOOT_TYPES = "lootTypes"sv;

  constexpr static std::string_view
  MOVE = "move"sv;
  constexpr static std::string_view
  TIME_DELTA = "timeDelta"sv;
};

struct HttpResponseConstants {

  HttpResponseConstants() = delete;

  constexpr static std::string_view
  CODE_INVALID_ARGUMENT = "invalidArgument"sv;
  constexpr static std::string_view
  CODE_MAP_NOT_FOUND = "mapNotFound"sv;

  constexpr static std::string_view
  GET_HEAD_EXPECTED = "Only GET, HEAD methods are expected"sv;
  constexpr static std::string_view
  ALLOWED_GET_HEAD = "GET, HEAD"sv;

  constexpr static std::string_view
  POST_EXPECTED = "Only POST method is expected"sv;
  constexpr static std::string_view
  ALLOWED_POST = "POST"sv;

  constexpr static std::string_view
  CODE_INVALID_METHOD = "invalidMethod"sv;

  constexpr static std::string_view
  MESSAGE_INVALID_NAME = "Invalid name"sv;
  constexpr static std::string_view
  MESSAGE_INVALID_MAP_ID = "Invalid mapId"sv;
  constexpr static std::string_view
  MESSAGE_INVALID_CONTENT_TYPE = "Invalid content type"sv;

  constexpr static std::string_view
  CODE_INVALID_TOKEN = "invalidToken"sv;
  constexpr static std::string_view
  MESSAGE_AUTH_HEADER_MISSING = "Authorization header is missing"sv;
  constexpr static std::string_view
  MESSAGE_INVALID_TOKEN = "Invalid token"sv;

  constexpr static std::string_view
  CODE_UNKNOWN_TOKEN = "unknownToken"sv;
  constexpr static std::string_view
  MESSAGE_TOKEN_NOT_FOUND = "Player token has not been found"sv;

  constexpr static std::string_view
  JOIN_GAME_PARSE_ERROR = "Join game request parse error"sv;
  constexpr static std::string_view
  JOIN_GAME_ERROR = "Error joining game"sv;
  constexpr static std::string_view
  PARSE_ACTION_ERROR = "Failed to parse action"sv;
  constexpr static std::string_view
  PARSE_TICK_ERROR = "Failed to parse tick request JSON"sv;

  constexpr static std::string_view
  CODE_400 = "badRequest"sv;
  constexpr static std::string_view
  MESSAGE_400 = "Bad request"sv;

  constexpr static std::string_view
  MESSAGE_404 = "File not found"sv;

  constexpr static std::string_view
  CODE_MAP_404 = "mapNotFound"sv;
  constexpr static std::string_view
  MESSAGE_MAP_404 = "Map not found"sv;

  constexpr static std::string_view
  MESSAGE_INVALID_METHOD = "Invalid method"sv;
  constexpr static std::string_view
  MESSAGE_INVALID_ENDPOINT = "Invalid endpoint"sv;

  constexpr static std::string_view
  NO_CACHE = "no-cache"sv;
};

struct ContentType {

  ContentType() = delete;

  constexpr static std::string_view
  TEXT_HTML = "text/html"sv;
  constexpr static std::string_view
  TEXT_CSS = "text/css"sv;
  constexpr static std::string_view
  TEXT_PLAIN = "text/plain"sv;
  constexpr static std::string_view
  TEXT_JAVASCRIPT = "text/javascript"sv;
  constexpr static std::string_view
  APPLICATION_JSON = "application/json"sv;
  constexpr static std::string_view
  APPLICATION_XML = "application/xml"sv;
  constexpr static std::string_view
  APPLICATION_OCTET = "application/octet-stream"sv;
  constexpr static std::string_view
  IMAGE_PNG = "image/png"sv;
  constexpr static std::string_view
  IMAGE_JPEG = "image/jpeg"sv;
  constexpr static std::string_view
  IMAGE_GIF = "image/gif"sv;
  constexpr static std::string_view
  IMAGE_BMP = "image/bmp"sv;
  constexpr static std::string_view
  IMAGE_ICON = "image/vnd.microsoft.icon"sv;
  constexpr static std::string_view
  IMAGE_TIFF = "image/tiff"sv;
  constexpr static std::string_view
  IMAGE_SVG_XML = "image/svg+xml"sv;
  constexpr static std::string_view
  AUDIO_MPEG = "audio/mpeg"sv;

  constexpr static std::string_view
  EXT_HTM = ".htm"sv;
  constexpr static std::string_view
  EXT_HTML = ".html"sv;
  constexpr static std::string_view
  EXT_CSS = ".css"sv;
  constexpr static std::string_view
  EXT_TXT = ".txt"sv;
  constexpr static std::string_view
  EXT_JS = ".js"sv;
  constexpr static std::string_view
  EXT_JSON = ".json"sv;
  constexpr static std::string_view
  EXT_XML = ".xml"sv;
  constexpr static std::string_view
  EXT_PNG = ".png"sv;
  constexpr static std::string_view
  EXT_JPG = ".jpg"sv;
  constexpr static std::string_view
  EXT_JPEG = ".jpeg"sv;
  constexpr static std::string_view
  EXT_JPE = ".jpe"sv;
  constexpr static std::string_view
  EXT_GIF = ".gif"sv;
  constexpr static std::string_view
  EXT_BMP = ".bmp"sv;
  constexpr static std::string_view
  EXT_ICO = ".ico"sv;
  constexpr static std::string_view
  EXT_TIFF = ".tiff"sv;
  constexpr static std::string_view
  EXT_TIF = ".tif"sv;
  constexpr static std::string_view
  EXT_SVG = ".svg"sv;
  constexpr static std::string_view
  EXT_SVGZ = ".svgz"sv;
  constexpr static std::string_view
  EXT_MP3 = ".mp3"sv;

  static const std::unordered_map<std::string_view, std::string_view> extensions;
};

inline const std::unordered_map<std::string_view, std::string_view> ContentType::extensions{
    {ContentType::EXT_HTM, ContentType::TEXT_HTML}, {ContentType::EXT_HTML, ContentType::TEXT_HTML},
    {ContentType::EXT_CSS, ContentType::TEXT_CSS}, {ContentType::EXT_TXT, ContentType::TEXT_PLAIN},
    {ContentType::EXT_JS, ContentType::TEXT_JAVASCRIPT},
    {ContentType::EXT_JSON, ContentType::APPLICATION_JSON},
    {ContentType::EXT_XML, ContentType::APPLICATION_XML}, {ContentType::EXT_PNG, ContentType::IMAGE_PNG},
    {ContentType::EXT_JPG, ContentType::IMAGE_JPEG}, {ContentType::EXT_JPEG, ContentType::IMAGE_JPEG},
    {ContentType::EXT_JPE, ContentType::IMAGE_JPEG}, {ContentType::EXT_GIF, ContentType::IMAGE_GIF},
    {ContentType::EXT_BMP, ContentType::IMAGE_BMP}, {ContentType::EXT_ICO, ContentType::IMAGE_ICON},
    {ContentType::EXT_TIFF, ContentType::IMAGE_TIFF}, {ContentType::EXT_TIF, ContentType::IMAGE_TIFF},
    {ContentType::EXT_SVG, ContentType::IMAGE_SVG_XML}, {ContentType::EXT_SVGZ, ContentType::IMAGE_SVG_XML},
    {ContentType::EXT_MP3, ContentType::AUDIO_MPEG}
};

struct SystemPaths {

  SystemPaths() = delete;

  constexpr static std::string_view
  MAPS = "/api/v1/maps"sv;
  constexpr static std::string_view
  API = "/api/"sv;
  constexpr static std::string_view
  JOIN = "/api/v1/game/join"sv;
  constexpr static std::string_view
  PLAYERS = "/api/v1/game/players"sv;
  constexpr static std::string_view
  GAME_STATE = "/api/v1/game/state"sv;
  constexpr static std::string_view
  ACTION = "/api/v1/game/player/action"sv;
  constexpr static std::string_view
  TICK = "/api/v1/game/tick"sv;
};
}