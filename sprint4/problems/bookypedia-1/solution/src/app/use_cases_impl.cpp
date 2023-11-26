#include "use_cases_impl.h"

#include "../domain/author.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string &name) {
  authors_.Save({AuthorId::New(), name});
}

std::vector<ui::detail::AuthorInfo> UseCasesImpl::GetAuthors() {
  std::vector<ui::detail::AuthorInfo> authors_info;
  std::vector<domain::Author> authors = authors_.GetAuthors();
  for (auto &author : authors) {
    authors_info.push_back({author.GetId().ToString(),author.GetName()});
  }

  return authors_info;
}

void UseCasesImpl::AddBook(ui::detail::AddBookParams &book_params) {
  books_.Save({BookId::New(),
               AuthorId::FromString(book_params.author_id),
               book_params.title,
               book_params.publication_year});
}

std::vector<ui::detail::BookInfo> UseCasesImpl::GetAuthorBooks(const std::string &author_id) {
  std::vector<ui::detail::BookInfo> books_info;

  auto books = books_.GetAuthorBooks(AuthorId::FromString(author_id));

  for (auto &book : books) {
    books_info.push_back({book.GetTitle(), book.GetYear().value()});
  }

  return books_info;
}

std::vector<ui::detail::BookInfo> UseCasesImpl::GetBooks() {
  std::vector<ui::detail::BookInfo> books_info;

  auto books = books_.GetBooks();

  for (auto &book : books) {
    books_info.push_back({book.GetTitle(), book.GetYear().value()});
  }

  return books_info;
}

}  // namespace app