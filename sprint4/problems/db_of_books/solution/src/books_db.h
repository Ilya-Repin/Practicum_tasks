#pragma once
#include <iostream>
#include <pqxx/pqxx>

namespace books {
using namespace std::literals;
using pqxx::operator "" _zv;

using namespace std::literals;
// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator "" _zv;

struct Book {
  int id = -1;
  std::string title;
  std::string author;
  int year = -1;
  std::optional<std::string> ISBN;
};

class BooksDatabase {
 public:
  BooksDatabase(std::string url);
  void CreateTable();
  bool AddBook(const Book &book);
  std::vector<Book> GetAllBooks();

 private:
  pqxx::connection conn_;
};
}