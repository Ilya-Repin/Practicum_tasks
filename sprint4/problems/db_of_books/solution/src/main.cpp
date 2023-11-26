#include <iostream>
#include "books_db.h"
#include "json_parser.h"

using namespace std::literals;
int main(int argc, const char *argv[]) {
  try {
    if (argc == 1) {
      std::cout << "Usage: connect_db <conn-string>\n"sv;
      return EXIT_SUCCESS;
    } else if (argc != 2) {
      std::cerr << "Invalid command line\n"sv;
      return EXIT_FAILURE;
    }
    books::BooksDatabase book_db(argv[1]);
    book_db.CreateTable();

    std::string command_str;
    bool is_running = true;
    while (is_running) {
      std::getline(std::cin, command_str);

      auto command = json_parser::ParseCommand(command_str);

      if (command.first == "add_book"s) {
        bool result = book_db.AddBook(command.second);
        std::cout << json_parser::MakeResultJson(result) << std::endl;
      } else if (command.first == "all_books"s) {
        auto books = book_db.GetAllBooks();
        std::cout << json_parser::MakeBooksJson(books) << std::endl;
      } else if (command.first == "exit"s) {
        break;
      }
    }

  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}
