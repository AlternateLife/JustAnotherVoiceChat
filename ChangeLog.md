# ChangeLog

## 0.3.2

  - Added headphone mute while joining the in-game channel
  - Added additional logging to improve error detection
  - Improve client muting when joining the in-game channel
  - Fixed muting clients when moving into unsubscribed in-game channel

## 0.3.1

 - Added 3D audio reset on disconnect
 - Fixed 3D audio reset when other client moves out of game channel
 - Fixed wrong log message on 3D system settings reset when connecting

## 0.3.0

 - Added 3D audio positioning using teamspeak 3D sound
 - Added volume modifing by client's distance
 - Added client global and per-client muting on server side
 - Added teamspeak identity check to server side
 - Added log message callback to server side to enable custom log filtering
 - Improved the teamspeak client rename response time a lot
 - Fixed a bug checking if the network protocol is compatible
 - Fixed a bug throwing a teamspeak error when renaming the client the already set name

**Attention:** Due to the bug in the protocol version check, client and server do **NOT** properly detect incompatible protocol versions between 0.2.0 and 0.3.0. Please update both the server and client to make ensure everything works as intended. *This should not happen again in the future*.

## 0.2.0

 - Added basic (positioned) based communication
 - Added client renaming
 - Added client moving into custom channel
 - Added protocol version check with server
 - Added http server for game handshake
