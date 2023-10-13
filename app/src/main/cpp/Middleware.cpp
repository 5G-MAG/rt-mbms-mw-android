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
#include "Middleware.h" 
#include "spdlog/spdlog.h"
#include <functional>

MBMS_RT::Middleware::Middleware( boost::asio::io_service& io_service, const std::string& api_url,
                                 const std::string& device_name, const std::string& api_key)
  : _cache()
  , _api(api_url, _cache, &_service_announcement, _services)
  , _tick_interval(1)
  , _timer(io_service, _tick_interval)
  //, _cfg(cfg)
  //, _interface(iface)
  , _io_service(io_service)
{
  _timer.async_wait(boost::bind(&Middleware::tick_handler, this)); //NOLINT
}

void MBMS_RT::Middleware::stop()
{
  _service_announcement->stop();
  for (auto& service : _services) {
    service.second->stop();
  }
  _running = false;
  _timer.cancel();
}

void MBMS_RT::Middleware::set_local_service_announcement(std::string sa)
{
    _service_announcement = std::make_unique<MBMS_RT::ServiceAnnouncement>(_io_service, _cache, _seamless,
                                                                           boost::bind(&Middleware::get_service, this,
                                                                                       boost::placeholders::_1),  //NOLINT
                                                                           boost::bind(&Middleware::set_service, this,
                                                                                       boost::placeholders::_1, boost::placeholders::_2)); //NOLINT
    _service_announcement->parse_bootstrap(sa);
}
void MBMS_RT::Middleware::handle_received_data(const signed char* buffer, size_t size)
{
  for (auto& service : _services) {
    service.second->handle_received_data(buffer, size);
  }
}
void MBMS_RT::Middleware::tick_handler()
{
  if (!_running) return;
#if 0
  auto mchs = _rp.getMchInfo();
  for (auto const &mch : mchs.as_array()) {
    for (auto const &mtch : mch.at("mtchs").as_array()) {
      auto tmgi = mtch.at("tmgi").as_string();
      auto dest = mtch.at("dest").as_string();
      auto is_service_announcement = std::stoul(tmgi.substr(0,6), nullptr, 16) < 0xF;
      if (!dest.empty() && is_service_announcement && !_service_announcement ) {
        // automatically start receiving the service announcement
        // 26.346 5.2.3.1.1 : the pre-defined TSI value shall be "0". 
        _service_announcement = std::make_unique<MBMS_RT::ServiceAnnouncement>(_cfg, tmgi, dest, 0 /*TSI*/, _interface, _io_service, 
            _cache, _seamless, boost::bind(&Middleware::get_service, this, _1), boost::bind(&Middleware::set_service, this, _1, _2) ); //NOLINT
      }
    }
  }
#endif
  _cache.check_file_expiry_and_cache_size();

  auto now = boost::posix_time::second_clock::local_time();
  boost::posix_time::time_duration time_since_last_stream_activity = now - _last_stream_activity;

  _timer.expires_at(_timer.expires_at() + _tick_interval);
  _timer.async_wait(boost::bind(&Middleware::tick_handler, this)); //NOLINT
}


auto MBMS_RT::Middleware::get_service(const std::string& service_id) -> std::shared_ptr<Service> 
{
  if (_services.find(service_id) != _services.end()) {
    return _services[service_id];
  } else {
    return nullptr;
  }
}
