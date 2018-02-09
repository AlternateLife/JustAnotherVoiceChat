/*
 * File: Client.cpp
 * Date: 09.02.2018
 *
 * MIT License
 *
 * Copyright (c) 2018 AlternateVoice
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Client.h"

#define NETWORK_CHANNELS 2

Client::Client() {
  _client = nullptr;
  _peer = nullptr;
}

Client::~Client() {
  close();
}

bool Client::open(std::string host, uint16_t port) {
  if (_client != nullptr) {
    return false;
  }

  _client = enet_host_create(NULL, 1, NETWORK_CHANNELS, 0, 0);
  if (_client == NULL) {
    _client = nullptr;
    return false;
  }

  return true;
}

void Client::close() {
  if (_client != nullptr) {
    enet_host_destroy(_client);
    _client = nullptr;
  }
}

bool Client::isOpen() const {
  return _client != nullptr;
}
