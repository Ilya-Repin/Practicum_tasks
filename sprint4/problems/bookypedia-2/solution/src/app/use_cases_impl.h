#pragma once
#include "../domain/author_fwd.h"
#include "use_cases.h"
#include "../domain/book.h"
#include "../domain/book_tags.h"

namespace app {

class UseCasesImpl : public UseCases {
 public:
  explicit UseCasesImpl(domain::AuthorRepository &authors,
                        domain::BookRepository &book, domain::BooksTagsRepository &books_tags)
      : authors_{authors}, books_{book}, books_tags_(books_tags) {
  }

  void AddAuthor(const std::string &name) override;
  void AddBook(ui::detail::AddBookParams &books_info) override;
  void AddBookTags(ui::detail::BookTagsInfo &book_tags) override;

  void DeleteAuthor(const std::string &author_id) override;
  void DeleteBook(const std::string &book_id) override;

  void EditAuthor(ui::detail::AuthorInfo &author_info) override;
  void EditBook(ui::detail::BookInfo &book_params) override;

  std::string GetAuthorIdFromName(const std::string &name) override;
  std::vector<ui::detail::AuthorInfo> GetAuthors() override;
  std::vector<ui::detail::BookInfo> GetAuthorBooks(const std::string &author_id) override;
  std::vector<ui::detail::BookInfo> GetBooks() override;
  std::vector<ui::detail::BookInfo> GetBooks(const std::string &start_with) override;
  ui::detail::BookInfo GetBook(const std::string &book_id) override;

 private:
  domain::AuthorRepository &authors_;
  domain::BookRepository &books_;
  domain::BooksTagsRepository &books_tags_;
};

}  // namespace app