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

#include "plugin_definitions.h"

#define PLUGIN_API_VERSION 22;

static struct TS3Functions ts3Functions;

const char *ts3plugin_name() {
  return "AlternateVoice";
}

const char *ts3plugin_version() {
  return "1.0.0";
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

int ts3plugin_init() {
  std::cout << "AlternateVoice: init" << std::endl;

  return 0;
}

void ts3plugin_shutdown() {
  std::cout << "AlternateVoice: shutdown" << std::endl;
}
