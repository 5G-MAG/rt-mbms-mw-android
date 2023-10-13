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
#include <vector>

namespace MBMS_RT {
  class HlsPrimaryPlaylist {
    public:
      HlsPrimaryPlaylist(const std::string& content, const std::string& base_path);
      HlsPrimaryPlaylist() = default;
      ~HlsPrimaryPlaylist() = default;

      struct Stream {
        std::string uri;
        std::string resolution;
        std::string codecs;
        unsigned long bandwidth;
        double frame_rate;
      };
      const std::vector<Stream>& streams() const { return _streams; };
      void add_stream(Stream stream) { _streams.push_back(std::move(stream)); };

      std::string to_string() const;
    private:
      std::vector<std::pair<std::string, std::string>> parse_parameters(const std::string& line) const;
      int _version = -1;
      std::vector<Stream> _streams = {};
  };
}
