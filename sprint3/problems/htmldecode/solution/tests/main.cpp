#include "htmldecode.h"
//
#include <iostream>

int main() {
  while (true) {
    std::string str;
    std::getline(std::cin, str);
    std::cout << HtmlDecode(str) << std::endl;
  }
}