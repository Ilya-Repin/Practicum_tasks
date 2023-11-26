#pragma once
#include <boost/json.hpp>
#include "books_db.h"

namespace json = boost::json;
using namespace std::literals;

namespace json_parser {

std::string MakeResultJson(bool result);
std::pair<std::string, books::Book> ParseCommand(const std::string& jsn);
std::string MakeBooksJson(const std::vector<books::Book>& books);

}