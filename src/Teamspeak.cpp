/*
 * File: Teamspeak.cpp
 * Date: 08.02.2018
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

#include "Teamspeak.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <stdio.h>
#include <string.h>

#include <plugin_definitions.h>
#include <microhttpd.h>

#include "Version.h"

#define PLUGIN_API_VERSION 22;

static struct TS3Functions ts3Functions;
static char versionBuffer[16];
static struct MHD_Daemon *httpDaemon;

const char *ts3plugin_name() {
  return "AlternateVoice";
}

const char *ts3plugin_version() {
  sprintf(versionBuffer, "%d.%d.%d.%d", ALTERNATEVOICE_VERSION_MAJOR, ALTERNATEVOICE_VERSION_MINOR, ALTERNATEVOICE_VERSION_PATCH, ALTERNATEVOICE_VERSION_BUILD);
  return versionBuffer;
}

int ts3plugin_apiVersion() {
  return PLUGIN_API_VERSION;
}

const char *ts3plugin_author() {
  return "AlternateVoice";
}

const char *ts3plugin_description() {
  return "3D game voice plugin for communicating with the AlternateVoice server";
}

void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
  ts3Functions = funcs;
}

int answerToConnection(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *uploadData, size_t *uploadDataSize, void **conCls) {
  const char *page = "<html><body>Hello World!</body></html>";

  struct MHD_Response *response;
  int ret;

  response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);

  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);

  return ret;
}

int ts3plugin_init() {
  std::cout << "AlternateVoice: init" << std::endl;

  
  httpDaemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, 8080, NULL, NULL, &answerToConnection, NULL, MHD_OPTION_END);
  if (httpDaemon == NULL) {
    std::cout << "AlternateVoice: Unable to start web server" << std::endl;
    return 0;
  }

  return 0;
}

void ts3plugin_shutdown() {
  std::cout << "AlternateVoice: shutdown" << std::endl;

  MHD_stop_daemon(httpDaemon);
}
