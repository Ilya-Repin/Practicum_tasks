#include "postgres.h"
#include "../domain/book.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator "" _zv;

void AuthorRepositoryImpl::Save(const domain::Author &author) {
  // Пока каждое обращение к репозиторию выполняется внутри отдельной транзакции
  // В будущих уроках вы узнаете про паттерн Unit of Work, при помощи которого сможете несколько
  // запросов выполнить в рамках одной транзакции.
  // Вы также может самостоятельно почитать информацию про этот паттерн и применить его здесь.
  pqxx::work work{connection_};
  work.exec_params(
      R"(
INSERT INTO authors (id, name) VALUES ($1, $2)
ON CONFLICT (id) DO UPDATE SET name=$2;
)"_zv,
      author.GetId().ToString(), author.GetName());
  work.commit();
}

std::vector<domain::Author> AuthorRepositoryImpl::GetAuthors() {
  std::vector<domain::Author> authors;

  pqxx::read_transaction r{connection_};
  auto query_text = R"(SELECT id, name FROM authors
ORDER BY name ASC;
)"_zv;

  auto res = r.query<std::string, std::string>(query_text);

  for (const auto [id, name] : res) {
    domain::Author author(domain::AuthorId::FromString(id), name);
    authors.push_back(std::move(author));
  }

  return authors;
}

void BookRepositoryImpl::Save(const domain::Book &book) {
  pqxx::work work{connection_};
  work.exec_params(
      R"(
INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4);
)"_zv,
      book.GetBookId().ToString(),
      book.GetAuthorId().ToString(),
      book.GetTitle(),
      book.GetYear());
  work.commit();
}

std::vector<domain::Book> BookRepositoryImpl::GetBooks() {
  pqxx::read_transaction r{connection_};
  std::vector<domain::Book> books;
  auto query_text = "SELECT id, author_id, title, publication_year FROM books "
                    "ORDER BY title ASC;";
  auto response =
      r.query<std::optional<std::string>, std::optional<std::string>, std::optional<std::string>, std::optional<int>>(
          query_text);

  for (const auto [id, author_id, title, year] : response) {
    books.push_back({domain::BookId::FromString(*id), domain::AuthorId::FromString(*author_id), *title, year});
  }

  return books;
}


std::vector<domain::Book> BookRepositoryImpl::GetAuthorBooks(const domain::AuthorId &author_id) {
  pqxx::read_transaction r{connection_};
  std::vector<domain::Book> books;

  auto query_text = "SELECT id, author_id, title, publication_year FROM books WHERE author_id=" + r.quote(author_id.ToString()) +
      "ORDER BY publication_year ASC, title ASC;";
  auto response =
      r.query<std::optional<std::string>, std::optional<std::string>, std::optional<std::string>, std::optional<int>>(
          query_text);

  for (const auto [id, author_id, title, year] : response) {
    books.push_back({domain::BookId::FromString(*id), domain::AuthorId::FromString(*author_id), *title, year});
  }

  return books;
}

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {
  pqxx::work work{connection_};
  work.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
)"_zv);

  work.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id                  UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id           UUID NOT NULL,
    title               varchar(100) NOT NULL,
    publication_year    integer
);
)"_zv);

  work.commit();
}

}  // namespace postgres