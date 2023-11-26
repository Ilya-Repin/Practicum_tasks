#pragma once

#include <string>

#include "../ui/view.h"

namespace app {

class UseCases {
public:
  virtual void AddAuthor(const std::string& name) = 0;
  virtual void AddBook(ui::detail::AddBookParams& books_info) = 0;
  virtual void AddBookTags(ui::detail::BookTagsInfo& book_tags) = 0;

  virtual void DeleteAuthor(const std::string& author_id) = 0;
  virtual void DeleteBook(const std::string& book_id) = 0;

  virtual void EditAuthor(ui::detail::AuthorInfo& author_info) = 0;
  virtual void EditBook(ui::detail::BookInfo& book_params) = 0;

  virtual std::string GetAuthorIdFromName(const std::string& name) = 0;
  virtual std::vector<ui::detail::AuthorInfo> GetAuthors() = 0;
  virtual std::vector<ui::detail::BookInfo> GetAuthorBooks(const std::string& author_id) = 0;
  virtual std::vector<ui::detail::BookInfo> GetBooks() = 0;
  virtual std::vector<ui::detail::BookInfo> GetBooks(const std::string& start_with) = 0;
  virtual ui::detail::BookInfo GetBook(const std::string& book_id) = 0;


protected:
    ~UseCases() = default;
};

}  // namespace app
