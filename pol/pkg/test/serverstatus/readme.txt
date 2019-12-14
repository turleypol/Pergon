/////////////////////////////////////////////////////////////////////////////////
//
//   ServerStatus PacketHook - Supports UO Gateway ServerStatus for Pol Servers
//
//     Author: Shinigami
//
//   Modifications:
//     2004/07/13 Shinigami: Los gehts
//     2004/07/18 Shinigami: Nach ewigen Tests siehts erstmal gut aus
//
/////////////////////////////////////////////////////////////////////////////////

This Package will return a string like

  Pol, Name=Unknown, Age=123, Ver=POL096-2004-05-20, TZ=1, EMail=removed, URL=removed, Clients=1

to UO Gateway and his Server Status Page (refreshed every 30 Minutes) if you choose
Pol as server.

Just extract this package into Pol-Package-Structure (like pol\pkg\opt\serverstatus),
open packethook.src and modify some constants like the name of your Server etc.
Compile the file and thats it. This package requires Pol 096 because of PacketHook.

Shinigami
