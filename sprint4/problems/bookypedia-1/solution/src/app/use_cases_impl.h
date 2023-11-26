#pragma once
#include "../domain/author_fwd.h"
#include "use_cases.h"
#include "../domain/book.h"

namespace app {


class UseCasesImpl : public UseCases {
 public:
  explicit UseCasesImpl(domain::AuthorRepository& authors,
                        domain::BookRepository& book)
      : authors_{authors}
      , books_{book} {
  }

  void AddAuthor(const std::string& name) override;
  void AddBook(ui::detail::AddBookParams& book_params) override;

  std::vector<ui::detail::AuthorInfo> GetAuthors() override;
  std::vector<ui::detail::BookInfo> GetAuthorBooks(const std::string& author_id) override;
  std::vector<ui::detail::BookInfo> GetBooks() override;

 private:
  domain::AuthorRepository& authors_;
  domain::BookRepository& books_;
};

}  // namespace app