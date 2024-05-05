
#include <boost/asio.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <cstdint>
#include <boost/array.hpp>
#include "src/server.hpp"

#define loop while(true)

auto main() -> int {
  constexpr std::uint16_t port = 15001;

  boost::asio::io_context io_context;
  chat::Server srv(io_context, port);
  srv.async_accept();
  io_context.run();
  return 0;
}
