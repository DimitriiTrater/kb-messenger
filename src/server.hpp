#include "session.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_set>

using boost::asio::ip::tcp;

namespace {

namespace io = boost::asio;
using boost::system::error_code;
using io::ip::tcp;

} // namespace

namespace chat {

class Server {
public:
  Server(io::io_context &io_context, std::uint16_t port)
      : io_context(io_context),
        acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {}

  auto async_accept() -> void {
    socket.emplace(io_context);

    acceptor.async_accept(*socket, [&](error_code error) {
      auto client = std::make_shared<Session>(std::move(*socket));
      client->post("Welcome to chat\n\r");
      post("We have newcomer\n\r");

      clients.insert(client);

      client->start(std::bind(&Server::post, this, std::placeholders::_1),
                    [&, weak = std::weak_ptr(client)] {
                      if (auto shared = weak.lock();
                          shared and clients.erase(shared)) {
                        post("We are one less\n\r");
                      }
                    });
      async_accept();
    });
  }

  auto post(const std::string &message) -> void {
    for (auto &client : clients) {
      client->post(message);
    }
  }

private:
  boost::asio::io_context &io_context;
  tcp::acceptor acceptor;
  std::optional<tcp::socket> socket;
  std::unordered_set<std::shared_ptr<Session>> clients;
};

}; // namespace chat
