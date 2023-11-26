#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "../domain/author.h"
#include "../domain/book.h"
#include "../domain/book_tags.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
 public:
  explicit AuthorRepositoryImpl(pqxx::connection &connection)
      : connection_{connection} {
  }

  void Save(const domain::Author &author) override;
  std::vector<domain::Author> GetAuthors() override;
  domain::AuthorId GetAuthorId(const std::string& name) override;
  void DeleteAuthor(const domain::AuthorId& author_id) override;
  void EditAuthor(const domain::Author& author) override;

 private:
  pqxx::connection &connection_;
};

class BookRepositoryImpl : public domain::BookRepository {
 public:
  explicit BookRepositoryImpl(pqxx::connection &connection)
      : connection_{connection} {
  }

  void Save(const domain::Book &book) override;
  void Edit(const domain::Book& book) override;
  std::vector<domain::Book> GetBooks() override;
  std::vector<domain::Book> GetAuthorBooks(const domain::AuthorId &author_id) override;
  std::vector<domain::Book> GetBooks(const std::string& start_with) override;
  domain::Book GetBook(const domain::BookId& book_id) override;
  void DeleteBook(const domain::BookId& book_id) override;


 private:
  pqxx::connection &connection_;

  void SaveTags(pqxx::work& work, const domain::BookId& book_id, std::set<std::string> tags);
};

class BooksTagsRepositoryImpl : public domain::BooksTagsRepository {
 public:
  explicit BooksTagsRepositoryImpl(pqxx::connection& connection)
      : connection_{connection} {
  }

  void Save(const domain::BookTags& book_tags) override;

 private:
  pqxx::connection& connection_;
};

class Database {
 public:
  explicit Database(pqxx::connection connection);

  AuthorRepositoryImpl &GetAuthors() &{
    return authors_;
  }

  BookRepositoryImpl &GetBooks() &{
    return books_;
  }

  BooksTagsRepositoryImpl& GetBooksTags() & {
    return books_tags_;
  }

 private:
  pqxx::connection connection_;
  AuthorRepositoryImpl authors_{connection_};
  BookRepositoryImpl books_{connection_};
  BooksTagsRepositoryImpl books_tags_{connection_};
};

}  // namespace postgres