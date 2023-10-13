// 5G-MAG Reference Tools
// MBMS Middleware Process
//
// Copyright (C) 2021 Klaus Kühnhammer (Österreichische Rundfunksender GmbH & Co KG)
//
// Licensed under the License terms and conditions for use, reproduction, and
// distribution of 5G-MAG software (the “License”).  You may not use this file
// except in compliance with the License.  You may obtain a copy of the License at
// https://www.5g-mag.com/reference-tools.  Unless required by applicable law or
// agreed to in writing, software distributed under the License is distributed on
// an “AS IS” BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.
// 
// See the License for the specific language governing permissions and limitations
// under the License.
//

#include "CdnClient.h"
#include "CdnFile.h"

#include "spdlog/spdlog.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>


MBMS_RT::CdnClient::CdnClient( boost::asio::io_service& io_service, const std::string& host,
                               int port, const std::string& base_path)
: _host(host)
, _port(port)
, _io_service(io_service)
, _base_path(base_path)
{
    if (_port == 0) _port = 80;
  spdlog::debug("Cdn client constructed with base {}", _host);
}

auto MBMS_RT::CdnClient::get(const std::string& path, std::function<void(std::shared_ptr<CdnFile>)> completion_cb, std::function<void()> error_cb) -> void
{
  //  const std::lock_guard<std::mutex> lock(_mutex);

    spdlog::debug("Cdn client requesting {}:{}{}{}", _host, _port,_base_path, path);


   // std::thread([&] {
            try {
            beast::tcp_stream stream(_io_service);

            tcp::endpoint ep(boost::asio::ip::address::from_string(_host), _port);
            stream.connect(ep);

            http::request<http::string_body> req{http::verb::get, _base_path + path, 11};
            req.set(http::field::host, _host);
            req.set(http::field::user_agent, "nakolos MW/0.9");
            http::write(stream, req);

            beast::flat_buffer buffer(1024*1024*128);

            // Declare a container to hold the response
            http::response<http::string_body> res;

            // Receive the HTTP response
            http::read(stream, buffer, res);

            if (http::to_status_class(res.result()) == http::status_class::successful) {

               // auto body_buffer = beast::buffers_front(res.body().data());
                auto cdn_file = std::make_shared<CdnFile>(res.body().size());

                auto copied = memcpy((void *) cdn_file->buffer(), (void *) res.body().data(),
                                     res.body().size());
                spdlog::debug("got {} bytes, copied {} bytes", res.body().size(), copied);
                // Gracefully close the socket
                beast::error_code ec;
                stream.socket().shutdown(tcp::socket::shutdown_both, ec);

                if (completion_cb) {
                     completion_cb(cdn_file);
                }
            } else {
                if (error_cb) {
                    error_cb();
                }
            }

    } catch(...) {
        error_cb();
    }
   // }).detach();

#if 0
  try {
    _client->request(methods::GET, path)
      .then([completion_cb, error_cb](http_response response) { // NOLINT
          if (response.status_code() == status_codes::OK) {
            Concurrency::streams::container_buffer<std::vector<uint8_t>> buf;
            response.body().read_to_end(buf)
              .then([buf, completion_cb, error_cb](size_t bytes_read){
                spdlog::debug("Downloaded {} bytes", bytes_read);
                auto cdn_file = std::make_shared<CdnFile>(bytes_read);
                memcpy(cdn_file->buffer(), &(buf.collection())[0], bytes_read);
                if (completion_cb) {
                  completion_cb(cdn_file);
                }
            })
                    .then([](pplx::task<void> previousTask)
                          {
                              try
                              {
                                  previousTask.get();
                              }
                              catch (const std::exception& ex)
                              {
                                  spdlog::error("Error reading CDN response body: {}", ex.what());
                              }
                          }).get();
          } else {
                spdlog::debug("got status {}", response.status_code());
              if (error_cb) {
                  error_cb();
              }
          }
        })
            .then([](pplx::task<void> previousTask)
                  {
                      try
                      {
                          previousTask.get();
                      }
                      catch (const std::exception& ex)
                      {
                          spdlog::error("Error getting file from CDN: {}", ex.what());
                      }
                  }).get();
  } catch (web::http::http_exception ex) { }
#endif
}
