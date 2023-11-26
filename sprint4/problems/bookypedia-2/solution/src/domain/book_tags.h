#pragma once

#include "book.h"

namespace domain {

class BookTags {
 public:
  BookTags(BookId book_id, std::set<std::string> tags)
      : book_id_(std::move(book_id)), tags_(std::move(tags)) {
  }

  const BookId &GetBookId() const noexcept {
    return book_id_;
  }
  const std::set<std::string> &GetTags() const noexcept {
    return tags_;
  }

 private:
  BookId book_id_;
  std::set<std::string> tags_;
};

class BooksTagsRepository {
 public:
  virtual void Save(const BookTags &book_tags) = 0;

 protected:
  ~BooksTagsRepository() = default;
};

}