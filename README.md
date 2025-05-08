# VGLoader
Vanguard Library Loader

What does VGLoader do?
* already contains the certification patch of each modules.
* no need to manualy inject vanguard dll's to the modules.
* ability to also load your custom dll for each modules.
* ability to hide vanguard console window.

Loading your custom dll:
* Make a Plugin folder and place your dll, your dll name should be the same as your target module.
  E.g. MachineManager.dll it will be loaded on MachineManager.exe.

Hide Console window of Vanguard
* Simply add HideConsole key with value TRUE or FALSE on each .ini of vanguard.
  E.g. GatewayServer.ini
  ```ini
  [GatewayServer]
  Key=
  PacketDebugging=0
  HideConsole=true
  ConnectionString=
  ```

** REMINDER **
Vanguard is attached to this modules (AgentServer, DownloadServer, GatewayServer, SR_GameServer, SR_ShardManager).
if ever you have a custom hooks on these mentioned modules, ask bim first for the address you hooking to, if bim say's he is hooking on it then you should find another way to hook your own stuff. Avoid double hooking!
