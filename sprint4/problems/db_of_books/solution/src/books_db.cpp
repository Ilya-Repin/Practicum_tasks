#include "books_db.h"

namespace books {
BooksDatabase::BooksDatabase(std::string url) : conn_(url) {
}

void BooksDatabase::CreateTable() {
  pqxx::work work_creation(conn_);
  work_creation.exec(
      "CREATE TABLE IF NOT EXISTS books (id SERIAL PRIMARY KEY NOT NULL, title varchar(100) NOT NULL, author varchar(100) NOT NULL, year integer NOT NULL, ISBN char(13) UNIQUE);"_zv);

  conn_.prepare("insert_books"_zv, "INSERT INTO books values (DEFAULT, $1, $2, $3, $4);"_zv);


  work_creation.commit();
}

bool BooksDatabase::AddBook(const Book &book) {
  pqxx::work work_add(conn_);
  try {
    work_add.exec_prepared("insert_books"_zv, book.title, book.author, book.year, book.ISBN);
    work_add.commit();
    return true;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

std::vector<Book> BooksDatabase::GetAllBooks() {
  pqxx::read_transaction read(conn_);
  std::vector<Book> books;
  auto query_text =
      "SELECT id, title, author, year, ISBN FROM books ORDER BY year DESC, title ASC, author ASC, ISBN ASC"_zv;
   for (auto [id, title, author, year, isbn] : read.query<std::optional<int>,
                                                         std::optional<std::string>,
                                                         std::optional<std::string>,
                                                         std::optional<int>,
                                                         std::optional<std::string>>(query_text)) {

     books.push_back({*id, *title, *author, *year, isbn});
  }

  return books;
}
}
