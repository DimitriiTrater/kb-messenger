#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <functional>
#include <iostream>
#include <istream>
#include <memory>
#include <queue>
#include <sstream>
#include <utility>

namespace {

namespace io = boost::asio;
using boost::system::error_code;
using io::ip::tcp;

} // namespace

namespace chat {

class Session : public std::enable_shared_from_this<Session> {
  using MessageHandler = std::function<void(std::string)>;
  using ErrorHandler = std::function<void()>;

public:
  Session(tcp::socket &&socket) : socket(std::move(socket)) {}

  auto start(MessageHandler &&message, ErrorHandler &&error) -> void {
    onMessage = std::move(message);
    onError = std::move(error);
    async_read();
  }

  auto post(const std::string& message) -> void {
    bool idle = outgoing.empty();
    outgoing.push(message);

    if (idle) {
      async_write();
    }
  }

private:
  auto async_read() -> void {
    boost::asio::async_read_until(
        socket, streambuf, '\n',
        std::bind(&Session::on_read, shared_from_this(), std::placeholders::_1,
                  std::placeholders::_2));
  }

  auto on_read(error_code error, std::size_t bytes_transferred) -> void {
    if (not error) {
      std::stringstream message;
      message << socket.remote_endpoint(error) << ": "
              << std::istream(&streambuf).rdbuf();
      streambuf.consume(bytes_transferred);
      onMessage(message.str());
      async_read();
    } else {
      [[maybe_unused]] auto _ = socket.close(error);
      onError();
    }
  }

  auto async_write() -> void {
    io::async_write(socket, io::buffer(outgoing.front()),
                    std::bind(&Session::on_write, shared_from_this(),
                              std::placeholders::_1, std::placeholders::_2));
  }

  auto on_write(error_code error, std::size_t bytes_transferred) -> void {
    if (not error) {
      outgoing.pop();

      if (not outgoing.empty()) {
        async_write();
      }
    } else {
      [[maybe_unused]] auto _ = socket.close(error);
      onError();
    }
  }

  tcp::socket socket;
  boost::asio::streambuf streambuf;
  std::queue<std::string> outgoing;
  MessageHandler onMessage;
  ErrorHandler onError;
};
} // namespace chat
