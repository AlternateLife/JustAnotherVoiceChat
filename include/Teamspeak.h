/*
 * File: Teamspeak.h
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

#pragma once

#include <stdint.h>
#include <stdio.h>

#include "ts3_functions.h"

#ifdef _WIN32
#ifdef ALTERNATEVOICE_EXPORTS
#define ALTERNATEVOICE_API __declspec(dllexport)
#else
#define ALTERNATEVOICE_API __declspec(dllimport)
#endif
#else
#define ALTERNATEVOICE_API __attribute__ ((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Required functions
ALTERNATEVOICE_API const char* ts3plugin_name();
ALTERNATEVOICE_API const char* ts3plugin_version();
ALTERNATEVOICE_API int ts3plugin_apiVersion();
ALTERNATEVOICE_API const char* ts3plugin_author();
ALTERNATEVOICE_API const char* ts3plugin_description();
ALTERNATEVOICE_API void ts3plugin_setFunctionPointers(const struct TS3Functions funcs);
ALTERNATEVOICE_API int ts3plugin_init();
ALTERNATEVOICE_API void ts3plugin_shutdown();

// Optional functions
ALTERNATEVOICE_API int ts3plugin_offersConfigure();
ALTERNATEVOICE_API void ts3plugin_configure(void* handle, void* qParentWidget);
ALTERNATEVOICE_API void ts3plugin_registerPluginID(const char* id);
ALTERNATEVOICE_API const char* ts3plugin_commandKeyword();
ALTERNATEVOICE_API int ts3plugin_processCommand(uint64 serverConnectionHandlerID, const char* command);
ALTERNATEVOICE_API void ts3plugin_currentServerConnectionChanged(uint64 serverConnectionHandlerID);
ALTERNATEVOICE_API const char* ts3plugin_infoTitle();
ALTERNATEVOICE_API void ts3plugin_infoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data);
ALTERNATEVOICE_API void ts3plugin_freeMemory(void* data);
ALTERNATEVOICE_API int ts3plugin_requestAutoload();
ALTERNATEVOICE_API void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon);
ALTERNATEVOICE_API void ts3plugin_initHotkeys(struct PluginHotkey*** hotkeys);

// Clientlib rare
ALTERNATEVOICE_API void ts3plugin_onClientBanFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, uint64 time, const char* kickMessage);
ALTERNATEVOICE_API int  ts3plugin_onClientPokeEvent(uint64 serverConnectionHandlerID, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored);
ALTERNATEVOICE_API void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue);
ALTERNATEVOICE_API void ts3plugin_onFileListEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char* path, const char* name, uint64 size, uint64 datetime, int type, uint64 incompletesize, const char* returnCode);
ALTERNATEVOICE_API void ts3plugin_onFileListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char* path);
ALTERNATEVOICE_API void ts3plugin_onFileInfoEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char* name, uint64 size, uint64 datetime);
ALTERNATEVOICE_API void ts3plugin_onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char* name, int type, int iconID, int saveDB);
ALTERNATEVOICE_API void ts3plugin_onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID);
ALTERNATEVOICE_API void ts3plugin_onServerGroupByClientIDEvent(uint64 serverConnectionHandlerID, const char* name, uint64 serverGroupList, uint64 clientDatabaseID);
ALTERNATEVOICE_API void ts3plugin_onServerGroupPermListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
ALTERNATEVOICE_API void ts3plugin_onServerGroupPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID);
ALTERNATEVOICE_API void ts3plugin_onServerGroupClientListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, uint64 clientDatabaseID, const char* clientNameIdentifier, const char* clientUniqueID);
ALTERNATEVOICE_API void ts3plugin_onChannelGroupListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char* name, int type, int iconID, int saveDB);
ALTERNATEVOICE_API void ts3plugin_onChannelGroupListFinishedEvent(uint64 serverConnectionHandlerID);
ALTERNATEVOICE_API void ts3plugin_onChannelGroupPermListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
ALTERNATEVOICE_API void ts3plugin_onChannelGroupPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID);
ALTERNATEVOICE_API void ts3plugin_onChannelPermListEvent(uint64 serverConnectionHandlerID, uint64 channelID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
ALTERNATEVOICE_API void ts3plugin_onChannelPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID);
ALTERNATEVOICE_API void ts3plugin_onClientPermListEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
ALTERNATEVOICE_API void ts3plugin_onClientPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID);
ALTERNATEVOICE_API void ts3plugin_onChannelClientPermListEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
ALTERNATEVOICE_API void ts3plugin_onChannelClientPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID);
ALTERNATEVOICE_API void ts3plugin_onClientChannelGroupChangedEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, uint64 channelID, anyID clientID, anyID invokerClientID, const char* invokerName, const char* invokerUniqueIdentity);
ALTERNATEVOICE_API int  ts3plugin_onServerPermissionErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, unsigned int failedPermissionID);
ALTERNATEVOICE_API void ts3plugin_onPermissionListGroupEndIDEvent(uint64 serverConnectionHandlerID, unsigned int groupEndID);
ALTERNATEVOICE_API void ts3plugin_onPermissionListEvent(uint64 serverConnectionHandlerID, unsigned int permissionID, const char* permissionName, const char* permissionDescription);
ALTERNATEVOICE_API void ts3plugin_onPermissionListFinishedEvent(uint64 serverConnectionHandlerID);
ALTERNATEVOICE_API void ts3plugin_onPermissionOverviewEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, uint64 channelID, int overviewType, uint64 overviewID1, uint64 overviewID2, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
ALTERNATEVOICE_API void ts3plugin_onPermissionOverviewFinishedEvent(uint64 serverConnectionHandlerID);
ALTERNATEVOICE_API void ts3plugin_onServerGroupClientAddedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char* clientName, const char* clientUniqueIdentity, uint64 serverGroupID, anyID invokerClientID, const char* invokerName, const char* invokerUniqueIdentity);
ALTERNATEVOICE_API void ts3plugin_onServerGroupClientDeletedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char* clientName, const char* clientUniqueIdentity, uint64 serverGroupID, anyID invokerClientID, const char* invokerName, const char* invokerUniqueIdentity);
ALTERNATEVOICE_API void ts3plugin_onClientNeededPermissionsEvent(uint64 serverConnectionHandlerID, unsigned int permissionID, int permissionValue);
ALTERNATEVOICE_API void ts3plugin_onClientNeededPermissionsFinishedEvent(uint64 serverConnectionHandlerID);
ALTERNATEVOICE_API void ts3plugin_onFileTransferStatusEvent(anyID transferID, unsigned int status, const char* statusMessage, uint64 remotefileSize, uint64 serverConnectionHandlerID);
ALTERNATEVOICE_API void ts3plugin_onClientChatClosedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char* clientUniqueIdentity);
ALTERNATEVOICE_API void ts3plugin_onClientChatComposingEvent(uint64 serverConnectionHandlerID, anyID clientID, const char* clientUniqueIdentity);
ALTERNATEVOICE_API void ts3plugin_onServerLogEvent(uint64 serverConnectionHandlerID, const char* logMsg);
ALTERNATEVOICE_API void ts3plugin_onServerLogFinishedEvent(uint64 serverConnectionHandlerID, uint64 lastPos, uint64 fileSize);
ALTERNATEVOICE_API void ts3plugin_onMessageListEvent(uint64 serverConnectionHandlerID, uint64 messageID, const char* fromClientUniqueIdentity, const char* subject, uint64 timestamp, int flagRead);
ALTERNATEVOICE_API void ts3plugin_onMessageGetEvent(uint64 serverConnectionHandlerID, uint64 messageID, const char* fromClientUniqueIdentity, const char* subject, const char* message, uint64 timestamp);
ALTERNATEVOICE_API void ts3plugin_onClientDBIDfromUIDEvent(uint64 serverConnectionHandlerID, const char* uniqueClientIdentifier, uint64 clientDatabaseID);
ALTERNATEVOICE_API void ts3plugin_onClientNamefromUIDEvent(uint64 serverConnectionHandlerID, const char* uniqueClientIdentifier, uint64 clientDatabaseID, const char* clientNickName);
ALTERNATEVOICE_API void ts3plugin_onClientNamefromDBIDEvent(uint64 serverConnectionHandlerID, const char* uniqueClientIdentifier, uint64 clientDatabaseID, const char* clientNickName);
ALTERNATEVOICE_API void ts3plugin_onComplainListEvent(uint64 serverConnectionHandlerID, uint64 targetClientDatabaseID, const char* targetClientNickName, uint64 fromClientDatabaseID, const char* fromClientNickName, const char* complainReason, uint64 timestamp);
ALTERNATEVOICE_API void ts3plugin_onBanListEvent(uint64 serverConnectionHandlerID, uint64 banid, const char* ip, const char* name, const char* uid, uint64 creationTime, uint64 durationTime, const char* invokerName, uint64 invokercldbid, const char* invokeruid, const char* reason, int numberOfEnforcements, const char* lastNickName);
ALTERNATEVOICE_API void ts3plugin_onClientServerQueryLoginPasswordEvent(uint64 serverConnectionHandlerID, const char* loginPassword);
ALTERNATEVOICE_API void ts3plugin_onPluginCommandEvent(uint64 serverConnectionHandlerID, const char* pluginName, const char* pluginCommand);
ALTERNATEVOICE_API void ts3plugin_onIncomingClientQueryEvent(uint64 serverConnectionHandlerID, const char* commandText);
ALTERNATEVOICE_API void ts3plugin_onServerTemporaryPasswordListEvent(uint64 serverConnectionHandlerID, const char* clientNickname, const char* uniqueClientIdentifier, const char* description, const char* password, uint64 timestampStart, uint64 timestampEnd, uint64 targetChannelID, const char* targetChannelPW);

// Client UI callbacks
ALTERNATEVOICE_API void ts3plugin_onAvatarUpdated(uint64 serverConnectionHandlerID, anyID clientID, const char* avatarPath);
ALTERNATEVOICE_API void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID);
ALTERNATEVOICE_API void ts3plugin_onHotkeyEvent(const char* keyword);
ALTERNATEVOICE_API void ts3plugin_onHotkeyRecordedEvent(const char* keyword, const char* key);
ALTERNATEVOICE_API void ts3plugin_onClientDisplayNameChanged(uint64 serverConnectionHandlerID, anyID clientID, const char* displayName, const char* uniqueClientIdentifier);
ALTERNATEVOICE_API const char* ts3plugin_keyDeviceName(const char* keyIdentifier);
ALTERNATEVOICE_API const char* ts3plugin_displayKeyText(const char* keyIdentifier);
ALTERNATEVOICE_API const char* ts3plugin_keyPrefix();

#ifdef __cplusplus
}
#endif
