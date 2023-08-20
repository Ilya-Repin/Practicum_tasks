#ifdef WIN32
#include <sdkddkver.h>
#endif

#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <thread>
#include <optional>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = net::ip::tcp;
using StringRequest = http::request<http::string_body>;
using StringResponse = http::response<http::string_body>;

using namespace std::literals;

struct ContentType {
  ContentType() = delete;

  constexpr static std::string_view TEXT_HTML = "text/html"sv;
};

/**
 * Для просмотра содержимого запросов
 */
void DumpRequest(const StringRequest &request) {
  std::cout << request.method_string() << ' ' << request.target() << std::endl;

  for (const auto &header : request) {
    std::cout << " "sv << header.name_string() << ": "sv << header.value() << std::endl;
  }
}

/**
 * Надстройка над http::read
 * @return  Если клиент отправил очередной запрос, она вернёт StringResponse, а если клиент завершил соединение, вернёт std::nullopt
 */
std::optional<StringRequest> ReadRequest(tcp::socket &socket, beast::flat_buffer &buffer) {
  beast::error_code ec;
  StringRequest request;

  http::read(socket, buffer, request, ec);

  if (ec == http::error::end_of_stream) {
    return std::nullopt;
  }

  if (ec) {
    throw std::runtime_error("Failed to read request: "s.append(ec.message()));
  }

  return request;
}

StringResponse MakeStringResponse(http::status status,
                                  std::string_view body,
                                  unsigned http_version,
                                  bool keep_alive,
                                  std::string_view content_type = ContentType::TEXT_HTML) {
  StringResponse response(status, http_version);

  response.body() = body;
  response.content_length(body.size());
  response.keep_alive(keep_alive);
  response.set(http::field::content_type, content_type);

  if(status == http::status::method_not_allowed) {
    response.set(http::field::allow, "GET, HEAD");
  }

  return response;
}

std::string SayHello(const StringRequest &request) {
  return "Hello, "s + std::string(request.target()).substr(1);
}

/**
 * Обработка запроса и формирование ответа
 */
StringResponse HandleRequest(StringRequest &&request) {
  const auto text_response = [&request](http::status status, std::string_view text) {
    return MakeStringResponse(status, text, request.version(), request.keep_alive());
  };

  if (request.method() == http::verb::get) {
    return text_response(http::status::ok, std::string_view(SayHello(request)));
  } else if (request.method() == http::verb::head) {
    return text_response(http::status::ok, "");
  }

  return text_response(http::status::method_not_allowed, "Invalid method");
}

/**
 * Работа с HTTP-сессией
 * @tparam RequestHandler - функция обработки запроса
 * @param socket
 * @param handle_request
 */
template<typename RequestHandler>
void HandleConnection(tcp::socket &socket, RequestHandler &&handle_request) {
  try {
    // Буфер для чтения данных в рамках текущей сессии.
    beast::flat_buffer buffer;

    // Продолжаем обработку запросов, пока клиент их отправляет
    while (auto request = ReadRequest(socket, buffer)) {
      // Обрабатываем запрос и формируем ответ сервера
      DumpRequest(*request);
      StringResponse response = handle_request(*std::move(request));
      http::write(socket, response);

      if (response.need_eof()) {
        break;
      }
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  beast::error_code ec;
  // Запрещаем дальнейшую отправку данных через сокет
  socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
  net::io_context io_context;

  const auto address = net::ip::make_address("0.0.0.0");
  constexpr unsigned short port = 8080;

  tcp::acceptor acceptor(io_context, {address, port});
  std::cout << "Server has started..."sv << std::endl;

  while (true) {
    tcp::socket socket(io_context);
    acceptor.accept(socket);

    std::thread th(
        [](tcp::socket socket) {
          HandleConnection(socket, HandleRequest);
        }, std::move(socket));

    th.detach();
  }
}
