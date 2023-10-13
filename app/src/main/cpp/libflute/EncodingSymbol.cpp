// libflute - FLUTE/ALC library
//
// Copyright (C) 2021 Klaus Kühnhammer (Österreichische Rundfunksender GmbH & Co KG)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cmath>
#include <arpa/inet.h>
#include "EncodingSymbol.h"

auto LibFlute::EncodingSymbol::from_payload(char* encoded_data, size_t data_len, const FecOti& fec_oti, ContentEncoding encoding) -> std::vector<EncodingSymbol> 
{
  auto source_block_number = 0;
  auto encoding_symbol_id = 0;
  std::vector<EncodingSymbol> symbols;

  if (encoding != ContentEncoding::NONE) {
    throw "Only unencoded content is supported";
  }
  
  if (fec_oti.encoding_id == FecScheme::CompactNoCode) {
    source_block_number = ntohs(*(uint16_t*)encoded_data);
    encoded_data += 2;
    encoding_symbol_id = ntohs(*(uint16_t*)encoded_data);
    encoded_data += 2;
    data_len -= 4;
  } else {
    throw "Only compact no-code FEC is supported";
  }

  int nof_symbols = std::ceil((float)data_len / (float)fec_oti.encoding_symbol_length);
  for (int i = 0; i < nof_symbols; i++) {
    if (fec_oti.encoding_id == FecScheme::CompactNoCode) {
      symbols.emplace_back(encoding_symbol_id, source_block_number, encoded_data, std::min(data_len, (size_t)fec_oti.encoding_symbol_length), fec_oti.encoding_id);
    }
    encoded_data += fec_oti.encoding_symbol_length;
    encoding_symbol_id++;
  }

  return symbols;
}

auto LibFlute::EncodingSymbol::to_payload(const std::vector<EncodingSymbol>& symbols, char* encoded_data, size_t data_len, const FecOti& fec_oti, ContentEncoding encoding) -> size_t
{
  size_t len = 0;
  auto ptr = encoded_data;
  auto first_symbol = symbols.begin();
  if (fec_oti.encoding_id == FecScheme::CompactNoCode) {
    *((uint16_t*)ptr) = htons(first_symbol->source_block_number());
    ptr += 2;
    *((uint16_t*)ptr) = htons(first_symbol->id());
    ptr += 2;
    len += 4;
  } else {
    throw "Only compact no-code FEC is supported";
  }

  for (const auto& symbol : symbols) {
    if (symbol.len() <= data_len) {
      auto symbol_len = symbol.encode_to(ptr, data_len);
      data_len -= symbol_len;
      encoded_data += symbol_len;
      len += symbol_len;
    }
  }
  return len;
}

auto LibFlute::EncodingSymbol::decode_to(char* buffer, size_t max_length) const -> void {
  if (_fec_scheme == FecScheme::CompactNoCode) {
    if (_data_len <= max_length) {
      memcpy(buffer, _encoded_data, _data_len);
    }
  }
}

auto LibFlute::EncodingSymbol::encode_to(char* buffer, size_t max_length) const -> size_t {
  if (_fec_scheme == FecScheme::CompactNoCode) {
    if (_data_len <= max_length) {
      memcpy(buffer, _encoded_data, _data_len);
      return _data_len;
    }
  }
  return 0;
}
