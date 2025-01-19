# About Client class

Even though the subject specifically says that we mustn’t develop a client, still a Client class is necessary because the server needs to maintain and manage details about all connected clients:

- client's associated attributes (nickname, username, hostname, realname,...)
- whether the clients have completed authentication (password provided) and registration (nick and user provided)
- the channels each client has joined
- their socket descriptor for communication
- ...

The class will act like a struct to encapsulate all this information. Server needs this information:

- To track individual clients (their attributes, their authentication/registration status)
- To handle multiple clients by storing each client as an object in a collection (a container)
- To identify every client by their name or socket
- To update the client's state (when they join a channel, change their nick, disconnect...)
- ...

## Authentication vs. Registration

In IRC protocol, **authentication** and **registration** are distinct steps.

1. First, **authentication** is completed when the user provides the correct `password`.

2. Next, user must complete **registration** by providing a `nickname` and a `username` (the real name)

The user will not be able to send messages or join channels until he is properly registered.

## Attribute: _buffer

A `_buffer` attribute in the Client class is needed to temporarily store incomplete or incoming data from the client socket until it can be processed.
Data sent by clients over a network is received in chunks. A single `recv()` call might not always receive a complete command or message, especially if the message is large or the network is slow. `_buffer` allows the server to store incomplete data until the full message or command is received.
In IRC, commands are separated by line endings `(\r\n)`. A client might send multiple commands in a single transmission, or a command might span multiple transmissions. The `_buffer` allows the server to:

- Accumulate data until it encounters a full line (ending with `\r\n`).
- Split the data into multiple commands if multiple `\r\n`-terminated lines are received at once.

### How _buffer Works in Practice

Receiving Data:

- When the server receives data from the client socket, it appends the new data to `_buffer`.
- Example: `_buffer += recv_data`

Checking for Complete Messages:

	- The server checks _buffer for any complete commands by looking for `\r\n`.
	- Example: `if '\r\n' in _buffer`

Processing Commands:

	- Once a complete command is found, it is extracted from _buffer and processed.
	- Any remaining data after the \r\n is left in _buffer for further processing.
        
Example:

```c++
size_t pos = _buffer.find("\r\n");
if (pos != std::string::npos) {
    std::string command = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 2); // Remove the processed command and \r\n
    processCommand(command);
}
```