#include <catch2/catch_test_macros.hpp>

#include "../src/htmldecode.h"

using namespace std::literals;

TEST_CASE("Empty string", "[HtmlDecode]") {
  CHECK(HtmlDecode(""sv) == ""s);
}

TEST_CASE("Text without mnemonics", "[HtmlDecode]") {
  CHECK(HtmlDecode("hello"sv) == "hello"s);
}

TEST_CASE("Text with mnemonics", "[HtmlDecode]") {
  CHECK(HtmlDecode("Hello, &ltUSER&gt;;"sv) == "Hello, <USER>;"s);
}

TEST_CASE("Text with mnemonics in upper-case", "[HtmlDecode]") {
  CHECK(HtmlDecode("Hello, &APOSUSER&QUOT;;"sv) == "Hello, 'USER\";"s);
}

TEST_CASE("Text with mnemonics in different cases", "[HtmlDecode]") {
  CHECK(HtmlDecode("Hello, &lTUSER&Gt;;"sv) == "Hello, &lTUSER&Gt;;"s);
}

TEST_CASE("Text with mnemonics in beginning, middle and end", "[HtmlDecode]") {
  CHECK(HtmlDecode("&ampHello, &ltUSER&gt;"sv) == "&Hello, <USER>"s);
}

TEST_CASE("Text with incomplete mnemonics", "[HtmlDecode]") {
  CHECK(HtmlDecode("&amHello, &lUSER&quo;"sv) == "&amHello, &lUSER&quo;"s);
}

TEST_CASE("Mnemonics with ;", "[HtmlDecode]") {
  CHECK(HtmlDecode("Hello, &lt;USER&gt"sv) == "Hello, <USER>"s);
}

// Напишите недостающие тесты самостоятельно
