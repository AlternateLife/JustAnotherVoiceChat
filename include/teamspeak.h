/*
 * File: include/teamspeak.h
 * Date: 08.02.2018
 *
 * MIT License
 *
 * Copyright (c) 2018 JustAnotherVoiceChat
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

#pragma once

#include <string>
#include <set>
#include <teamspeak/public_definitions.h>

// wrapped functions
void ts3_log(std::string message, enum LogLevel severity);

bool ts3_verifyServer(std::string uniqueIdentifier);
bool ts3_moveToChannel(uint64 channelId, std::string password);
bool ts3_muteClient(anyID clientId, bool mute);
bool ts3_muteClients(std::set<anyID> &clients, bool mute);
bool ts3_unmuteAllClients();
std::set<anyID> ts3_clientsInChannel(uint64 channelId);
bool ts3_setNickname(std::string nickname);
bool ts3_resetNickname();
std::string ts3_getClientIdentity();

uint64 ts3_serverConnectionHandle();
anyID ts3_clientId(uint64 serverConnectionHandlerId);
uint64 ts3_channelId(uint64 serverConnectionHandlerId);
void ts3_setClientVolumeModifier(anyID clientID, float value);
void ts3_setClientPosition(anyID clientID, const struct TS3_Vector *position);
bool ts3_isInputMuted(uint64 serverConnectionHandlerId);
bool ts3_isOutputMuted(uint64 serverConnectionHandlerId);
