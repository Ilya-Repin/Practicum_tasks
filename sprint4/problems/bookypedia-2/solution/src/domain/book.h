#pragma once
#include <string>
#include <vector>
#include <optional>
#include <set>

#include "author.h"
#include "../util/tagged_uuid.h"

namespace domain {

namespace detail {
struct BookTag {};
}  // namespace detail

using BookId = util::TaggedUUID<detail::BookTag>;

class Book {
 public:
  Book(BookId book_id, AuthorId author_id, std::string author_name, std::string title, std::optional<int> year, std::set<std::string> book_tags) : book_id_(std::move(book_id)),
        author_id_(std::move(author_id)),
        author_name_(std::move(author_name)),
        title_(std::move(title)),
        year_(year),
        book_tags_(std::move(book_tags)) {
  }

  const BookId &GetBookId() const noexcept {
    return book_id_;
  }
  const AuthorId &GetAuthorId() const
  noexcept {
    return author_id_;
  }
  const std::string &GetAuthorName() const
  noexcept {
    return author_name_;
  }
  const std::string &GetTitle() const
  noexcept {
    return title_;
  }
  std::optional<int> GetYear() const
  noexcept {
    return year_;
  }
  std::set<std::string> GetTags() const noexcept {
    return book_tags_;
  }

  void AddTag(const std::string& tag) noexcept {
    book_tags_.insert(tag);
  }

 private:
  BookId book_id_;
  AuthorId author_id_;
  std::string author_name_;
  std::string title_;
  std::optional<int> year_;
  std::set<std::string> book_tags_;
};

class BookRepository {
 public:
  virtual void Save(const Book &book) = 0;
  virtual void Edit(const Book& book) = 0;

  virtual void DeleteBook(const domain::BookId& book_id) = 0;

  virtual std::vector<Book> GetAuthorBooks(const AuthorId &author_id) = 0;
  virtual std::vector<Book> GetBooks() = 0;
  virtual std::vector<Book> GetBooks(const std::string& start_with) = 0;
  virtual domain::Book GetBook(const domain::BookId& book_id) = 0;

 protected:
  ~BookRepository() = default;
};

}  // namespace domain