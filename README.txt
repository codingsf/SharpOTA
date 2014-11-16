
There are four parts.

RemoteJar:
JAR(DEX) file to be loaded in remote process.

IpcServer:
All (binder) hook is redirected to IpcServer.
Hook processing can be implement in both remote JAR or IpcServer.

libremote:
Load and run RemoteJar to currently injected process.

injector:
Injects libremote.so into specified processes.

Work flow:
1. IpcServer initialize and wait for requests
    * extract RemoteJar to a world RWX directory
    * start listening in an Android service
2. On Android start, inject zygote and all other app_process
    * zygote/app_process can use ActivityThread to obtain application context
    * system_server
    * hooking ActivityManagerService is WIP, can be done by replacing internal H class in ???
3. For sensitive binder calls, process info & parameters are passed to IpcServer, and result can be controlled by IpcServer
    * when victim process attempts to call some service, start IpcService to contact IpcServer
    * binder transaction data is serialized and passed to IpcServer, and IpcServer send transaction result and optional reply
    * also can be implemented in RemoteJar without communicating with IpcServer

TODO:
1. build.xml to automate DEX packing
2. integrate with SUPERSU
3. current IPC implementation is LocalSocket. No way to pass binders, need to switch to AIDL.
4. Is JAVA method hooking really needed for just location faking, etc.?

