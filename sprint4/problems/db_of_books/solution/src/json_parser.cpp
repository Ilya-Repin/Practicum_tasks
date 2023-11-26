
#include "json_parser.h"
std::string json_parser::MakeResultJson(bool result) {
  json::object res_obj;
  res_obj["result"] = result;
  return json::serialize(res_obj);
}

std::pair<std::string, books::Book> json_parser::ParseCommand(const std::string &jsn) {
  auto value = json::parse(jsn);
  std::string action = std::string(value.at("action").as_string());
  books::Book book;

  if (value.as_object().contains("payload") && !(value.as_object().at("payload").as_object().empty())) {
    auto& payload_obj = value.at("payload").as_object();
    book.title = payload_obj.at("title").as_string();
      book.author = payload_obj.at("author").as_string();

      book.year =  payload_obj.at("year").as_int64();

      if (payload_obj.contains("ISBN") && !payload_obj.at("ISBN").is_null()) {

        book.ISBN = payload_obj.at("ISBN").as_string();
      }
    }

  return std::pair{ action, book };
}

std::string json_parser::MakeBooksJson(const std::vector<books::Book> &books) {
  json::array books_arr;
  for (auto& book : books)
  {
    json::object book_obj;

    book_obj["id"] = book.id;
    book_obj["title"] = book.title;
    book_obj["author"] = book.author;
    book_obj["year"] = book.year;

    if (book.ISBN.has_value()) {
      book_obj["ISBN"] = book.ISBN.value();
    }
    else {
      book_obj["ISBN"] = nullptr;
    }
    books_arr.push_back(book_obj);

  }

  return json::serialize(books_arr);
}
