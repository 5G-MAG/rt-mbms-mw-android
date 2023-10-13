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
#include "CdnFile.h"
#include <boost/asio.hpp>
namespace MBMS_RT {
  class CdnClient {
    public:
      explicit CdnClient( boost::asio::io_service& io_service, const std::string& host, int port, const std::string& base_path);
      virtual ~CdnClient() = default;

      void get(const std::string& path, std::function<void(std::shared_ptr<CdnFile>)> completion_cb, std::function<void()> errorCallback);
  private:
      std::string _host;
      std::string _base_path;
      int _port;
      boost::asio::io_service& _io_service;
  };
}
