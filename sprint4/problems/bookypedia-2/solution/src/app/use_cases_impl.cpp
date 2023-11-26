#include "use_cases_impl.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string &name) {
  authors_.Save({AuthorId::New(), name});
}



std::string UseCasesImpl::GetAuthorIdFromName(const std::string& name) {
  return authors_.GetAuthorId(name).ToString();
}

std::vector<ui::detail::AuthorInfo> UseCasesImpl::GetAuthors() {
  std::vector<ui::detail::AuthorInfo> authors_info;
  std::vector<domain::Author> authors = authors_.GetAuthors();
  for (auto &author : authors) {
    authors_info.push_back({author.GetId().ToString(),author.GetName()});
  }

  return authors_info;
}

void UseCasesImpl::DeleteAuthor(const std::string& author_id) {
  authors_.DeleteAuthor(AuthorId::FromString(author_id));
}

void UseCasesImpl::EditAuthor(ui::detail::AuthorInfo& author_info) {
  authors_.EditAuthor({AuthorId::FromString(author_info.id),
                          author_info.name});
}

void UseCasesImpl::AddBook(ui::detail::AddBookParams &book_params) {
  books_.Save({BookId::New(),
               AuthorId::FromString(book_params.author_id),
               "",
               book_params.title,
               book_params.publication_year,
               book_params.tags});
}





std::vector<ui::detail::BookInfo> UseCasesImpl::GetAuthorBooks(const std::string &author_id) {
  std::vector<ui::detail::BookInfo> books_info;

  auto books = books_.GetAuthorBooks(AuthorId::FromString(author_id));

  for (auto &book : books) {
    books_info.push_back({book.GetBookId().ToString(), book.GetTitle(), book.GetYear().value(), book.GetAuthorName(), book.GetTags()});
  }

  return books_info;
}

void UseCasesImpl::EditBook(ui::detail::BookInfo& book_info) {
  books_.Edit({BookId::FromString(book_info.id),
               AuthorId::New(),
               book_info.author_name,
               book_info.title,
               book_info.publication_year,
               book_info.tags});
}

std::vector<ui::detail::BookInfo> UseCasesImpl::GetBooks() {
  std::vector<ui::detail::BookInfo> books_info;

  auto books = books_.GetBooks();

  for (auto &book : books) {
    books_info.push_back({book.GetBookId().ToString(), book.GetTitle(), book.GetYear().value(), book.GetAuthorName(), book.GetTags()});
  }

  return books_info;
}

ui::detail::BookInfo UseCasesImpl::GetBook(const std::string& book_id) {
  auto book = books_.GetBook(BookId::FromString(book_id));
  return ui::detail::BookInfo({book.GetBookId().ToString(), book.GetTitle(), book.GetYear().value(), book.GetAuthorName(), book.GetTags()});

}

std::vector<ui::detail::BookInfo> UseCasesImpl::GetBooks(const std::string& start_with) {
  std::vector<ui::detail::BookInfo> books_info;

  auto books = books_.GetBooks(start_with);

  for (auto &book : books) {
    books_info.push_back({book.GetBookId().ToString(), book.GetTitle(), book.GetYear().value(), book.GetAuthorName(), book.GetTags()});

  }

  return books_info;
}



void UseCasesImpl::AddBookTags(ui::detail::BookTagsInfo& book_tags) {
  books_tags_.Save({BookId::FromString(book_tags.book_id), book_tags.tags});
}

void UseCasesImpl::DeleteBook(const std::string& book_id) {
  books_.DeleteBook(BookId::FromString(book_id));
}


}  // namespace app