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
#pragma once

#include <string>
#include <filesystem>
//#include <libconfig.h++>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "Service.h"
#include "ServiceAnnouncement.h"
#include "File.h"
#include "RestHandler.h"
#include "CacheManagement.h"
#include "Service.h"
//#include "on_demand/ControlSystemRestClient.h"

namespace MBMS_RT {
  class Middleware {
    public:
      Middleware( boost::asio::io_service& io_service, const std::string& api_url,
               const std::string& device_name, const std::string& api_key
      );//, const libconfig::Config& cfg, const std::string& api_url, const std::string& iface);

      std::shared_ptr<Service> get_service(const std::string& service_id);
      void set_service(const std::string& service_id, std::shared_ptr<Service> service) { _services[service_id] = service; };

      void set_local_service_announcement(std::string sa);
      void handle_received_data(const signed char* buffer, size_t size);

      void stop();
    private:
      void tick_handler();

      bool _seamless = true;

      MBMS_RT::RestHandler _api;
      MBMS_RT::CacheManagement _cache;

      std::unique_ptr<MBMS_RT::ServiceAnnouncement> _service_announcement = {nullptr};
      std::map<std::string, std::shared_ptr<Service>> _services;

      boost::posix_time::seconds _tick_interval = boost::posix_time::seconds(1);
      boost::asio::deadline_timer _timer;

      boost::asio::io_service& _io_service;

      boost::posix_time::ptime _last_stream_activity;
      boost::posix_time::seconds _stream_activity_timeout = boost::posix_time::seconds(4);

      bool _running = true;
    };
};
