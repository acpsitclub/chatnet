# `chatnet`
A network of clients communicating under a common protocol.


# Usage
In the example below, *Muhammad* and *AbdurRahman* are two users located far apart. The commands are arranged in such way that describes the process best. But, as we work to make better, most of the processes will run in background - at will of Allah.
```
Muhammad $ ./chatnet boot

AbdurRahman $ ./chatnet boot

Muhammad $ ./chatnet anyconnreq
[Info] No connection requests.

AbdurRahman $ ./chatnet connect Muhammad
Waiting for Muhammad to accept...

Muhammad $ ./chatnet anyconnreq
[Info] Accept connection request from: AbdurRahman? (y/N) y
[Info] Secure connection established with AbdurRahman.

AbdurRahman $
[Info] Request accepted.

AbdurRahman $ ./chatnet send_msg Muhammad Peace be upon \ 
whoever wants to follow right guidance.

Muhammad $ ./chatnet recv_msg AbdurRahman
Peace be upon whoever wants to follow right guidance.
```


# Information
### The Chatnet Protocol
Server/Host of the protocol is one file: `net.php`. Clients send JSON to `net.php` and receive response, which has the keys:
- `uSend` is the person sending JSON to host.
- `uRecv` is the recipient of JSON.
- `shkey` is a cryptographically secure 100% unique hex string.
- `msgText` is the message, which can also be a command to host.

### All commands receivable to host:
- `!showactive` - List of active users.
- `!anyconnreq` - Checks if anyone sent a connection request.
- `!connect <uRecv>` - Sends connection request to `uRecv`.
- `!connaccept` - Accepts a connection request.
- `!conndenied` - Denies a connection request.
- `!anynewmsg <uRecv>` - Checks for new `msgText` from `uRecv`.

If none of the commands are specified in `msgText`, host considers the JSON as a send of message to specified `uRecv`.

### Source Code: Build
- `libcpyb.h` is a [dependency-library](https://github.com/midnqp/lib-cpython-builtins), containing useful functions and macros in C. Clone that repository, and create a symlink to this project's root named `libcpyb.h`.
- `libcurl.a` and `libcjson.a` are statically compiled libraries, with copyrights to respective authors.
- To build, run: `./make chatnet.c`
- Upon building, a new directory is created named `chatnet_role_change` with the executable copied, which will be of help for testing i.e. chatting with yourself.

### TODO List
- Random `CHATNET` ASCII art.
- Support for Windows.
- `!exit` - Exits the chatroom, cleans up at host.
- `!showactive`
- Make `!anyconnreq` `!anynewmsg` threaded, and run at background.
- A platform-specific background daemon, to recieve bleeding-edge *rolling* releases.
- `!connaccept` and `!conndenied` should show/accpet/deny requests in a queue.
- Integrate sending and receiving of message in the same terminal window.

Developers, make sure you have fun while contributing. This project is distributed under MIT License in the hope that, it may be of use to someone -- and glorified you are Allah, praises are for you, forgive us.
