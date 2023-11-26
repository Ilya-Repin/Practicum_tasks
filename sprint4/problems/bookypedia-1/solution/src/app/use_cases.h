#pragma once

#include <string>

#include "../ui/view.h"

namespace app {

class UseCases {
public:
  virtual void AddAuthor(const std::string& name) = 0;
  virtual void AddBook(ui::detail::AddBookParams& books_info) = 0;

  virtual std::vector<ui::detail::AuthorInfo> GetAuthors() = 0;
  virtual std::vector<ui::detail::BookInfo> GetAuthorBooks(const std::string& author_id) = 0;
  virtual std::vector<ui::detail::BookInfo> GetBooks() = 0;

protected:
    ~UseCases() = default;
};

}  // namespace app
