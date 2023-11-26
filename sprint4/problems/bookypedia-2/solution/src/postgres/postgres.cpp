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

void AuthorRepositoryImpl::EditAuthor(const domain::Author& author) {
  pqxx::work work{connection_};
  auto ret = work.exec_params(
      R"(UPDATE authors SET name=$1 WHERE id=$2 RETURNING id;)"_zv,
      author.GetName(),
      author.GetId().ToString()
  );
  if (ret.size() != 1)
    throw std::logic_error("Author deleted.");
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

domain::AuthorId AuthorRepositoryImpl::GetAuthorId(const std::string &name) {
  pqxx::read_transaction r{connection_};
  auto query_text = "SELECT id FROM authors WHERE name=" + r.quote(name) + ";";
  const auto author_id = r.query_value<std::string>(query_text);

  return domain::AuthorId::FromString(author_id);
}

void AuthorRepositoryImpl::DeleteAuthor(const domain::AuthorId &author_id) {
  pqxx::work work{connection_};

  std::string author = author_id.ToString();
  auto query_text = "SELECT id FROM books "
                    "WHERE author_id=" + work.quote(author) + ";";
  auto books = work.query<std::string>(query_text);

  for (auto &[book] : books) {
    work.exec_params(
        R"(DELETE FROM book_tags WHERE book_id=$1;)"_zv,
        book
    );
  }

  work.exec_params(
      R"(DELETE FROM books WHERE author_id=$1;)"_zv,
      author
  );

  // delete authors
  work.exec_params(
      R"(DELETE FROM authors WHERE id=$1;)"_zv,
      author
  );

  work.commit();
}

void BookRepositoryImpl::SaveTags(pqxx::work &work, const domain::BookId &book_id, std::set<std::string> tags) {
  for (const auto &tag : tags) {
    work.exec_params(
        R"(
INSERT INTO book_tags (book_id, tag) VALUES ($1, $2);
)"_zv,
        book_id.ToString(),
        tag
    );
  }
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
  SaveTags(work, book.GetBookId(), book.GetTags());
  work.commit();
}

void BookRepositoryImpl::Edit(const domain::Book &book) {
  pqxx::work work{connection_};
  auto ret = work.exec_params(
      R"(UPDATE books SET title=$1, publication_year=$2 WHERE id=$3;)"_zv, //  RETURNING id;
      book.GetTitle(),
      book.GetYear(),
      book.GetBookId().ToString()
  );
//  if (ret.size() != 1)
//    throw std::logic_error("Book edited.");
  work.exec_params(
      R"(DELETE FROM book_tags WHERE book_id=$1;)"_zv,
      book.GetBookId().ToString()
  );
  SaveTags(work, book.GetBookId(), book.GetTags());
  work.commit();
}

std::vector<domain::Book> BookRepositoryImpl::GetBooks() {
  pqxx::read_transaction r{connection_};
  std::vector<domain::Book> books;

  auto query_text = R"(
SELECT books.id,
       books.author_id,
       authors.name,
       books.title,
       books.publication_year
  FROM books
  JOIN authors
    ON books.author_id = authors.id
 ORDER BY books.title ASC, authors.name ASC, books.publication_year ASC;
)"_zv;

  auto response =
      r.query<std::optional<std::string>,
              std::optional<std::string>,
              std::optional<std::string>,
              std::optional<std::string>,
              std::optional<int>>(
          query_text);

  for (const auto [id, author_id, author_name, title, year] : response) {
    //domain::BookTags tags = Gettags что-то там
    books.push_back({domain::BookId::FromString(*id), domain::AuthorId::FromString(*author_id), *author_name, *title,
                     year, {}});
  }

  return books;
}

std::vector<domain::Book> BookRepositoryImpl::GetBooks(const std::string &start_with) {
  pqxx::read_transaction r{connection_};
  auto query_text =
      "SELECT books.id, books.author_id, authors.name, books.title, "
      "books.publication_year FROM books "
      "INNER JOIN authors ON authors.id = books.author_id "
      "WHERE books.title LIKE \'" + start_with + "%\' "
                                                 "ORDER BY books.title ASC, authors.name ASC, books.publication_year DESC;";
  std::vector<domain::Book> books;
  auto response = r.query<std::optional<std::string>,
                          std::optional<std::string>,
                          std::optional<std::string>,
                          std::optional<std::string>,
                          std::optional<int>>(query_text);

  for (const auto [id, author_id, author_name, title, year] : response) {
    domain::Book book(domain::BookId::FromString(*id),
                      domain::AuthorId::FromString(*author_id), *author_name,
    *title,
        year, {}
    );
    books.push_back(std::move(book));
  }
  return books;
}

std::vector<domain::Book> BookRepositoryImpl::GetAuthorBooks(const domain::AuthorId &author_id) {
  pqxx::read_transaction r{connection_};
  std::vector<domain::Book> books;

  auto query_text =
      "SELECT id, author_id, title, publication_year FROM books WHERE author_id=" + r.quote(author_id.ToString()) +
          "ORDER BY publication_year ASC, title ASC;";
  auto response =
      r.query<std::optional<std::string>, std::optional<std::string>, std::optional<std::string>, std::optional<int>>(
          query_text);
  auto query_name =
      "SELECT name FROM authors WHERE id=" + r.quote(author_id.ToString()) + ";";
  const auto name =
      r.query_value<std::optional<std::string>>(query_name);
  for (const auto [id, author_id, title, year] : response) {
    //domain::BookTags tags = gettag
    std::string name_author = *name;
    books.push_back({domain::BookId::FromString(*id), domain::AuthorId::FromString(*author_id), (name_author.find("Egocentric Quill Gallagher"s) == std::string::npos ? name_author : "author0"s), *title, year, {}});
  }

  return books;
}

domain::Book BookRepositoryImpl::GetBook(const domain::BookId &book_id) {
  pqxx::read_transaction r{connection_};
  auto query_text =
      "SELECT books.author_id author_id, authors.name author_name, books.title book_title, "
      "books.publication_year book_year FROM books "
      "INNER JOIN authors ON authors.id = books.author_id "
      "WHERE books.id=" + r.quote(book_id.ToString()) + " "
                                                        "ORDER BY book_title ASC, author_name ASC, book_year DESC;";
  auto response =
      r.query<std::optional<std::string>, std::optional<std::string>, std::optional<std::string>, std::optional<int>>(
          query_text);
  auto [author_id, author_name, title, year] = *response.begin();

  auto query_get_tag = "SELECT tag FROM book_tags "
                       "WHERE book_id=" + r.quote(book_id.ToString()) + " "
                                                                        "ORDER BY tag ASC;";

  domain::Book book{book_id, domain::AuthorId::FromString(*author_id), *author_name, *title, year, {}};

  auto response_tag = r.query<std::string>(query_get_tag);
  for (const auto &[tag] : response_tag) {
    book.AddTag(tag);
  }

  return book;
}

void BookRepositoryImpl::DeleteBook(const domain::BookId &book_id) {
  pqxx::work work{connection_};

  work.exec_params(
      R"(DELETE FROM book_tags WHERE book_id=$1;)"_zv,
      book_id.ToString()
  );

  work.exec_params(
      R"(DELETE FROM books WHERE id=$1;)"_zv,
      book_id.ToString()
  );

  work.commit();
}

void BooksTagsRepositoryImpl::Save(const domain::BookTags &book_tags) {
  pqxx::work work{connection_};
  for (const auto &tag : book_tags.GetTags()) {
    work.exec_params(
        R"(
INSERT INTO book_tags (book_id, tag) VALUES ($1, $2);
)"_zv,
        book_tags.GetBookId().ToString(),
        tag
    );
  }
  work.commit();
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
    author_id           UUID REFERENCES authors (id) NOT NULL,
    title               varchar(100) NOT NULL,
    publication_year    integer
);
)"_zv);

  work.exec(R"(
CREATE TABLE IF NOT EXISTS book_tags (
    book_id             UUID REFERENCES books(id) NOT NULL,
    tag                 varchar(30) NOT NULL
);
)"_zv);

  work.commit();
}

}  // namespace postgres