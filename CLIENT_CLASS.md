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
  
--------------------------------------------------------------------------------------------

## Attributes needed
- `_socketFd`

	Integer type. It will store the descriptor of the socket.

- `_nickname`

	String type. It will store the nickname provided by the user during registration (i. e. when a client sends the NICK command).

- `_username`

	String type. It will store the username provided by the user during registration (i.e. when a client sends the USER command). Also known as *"ident"*, the username is part of the user's `hostmask`, which identifies the user on the network. The hostmask generally looks like this:

	- `<nickname>!<username>@<hostname>`

	For example:

	- `chuckie68!charles@irc.example.com`

- `_realname`

	String type. It will store the nickname provided by the user during registration.
	The realname is a more human-readable field intended to provide additional information about the user. It is purely informational and does not have a functional role in identifying or authenticating the user. The realname is visible to others when they query information about the user (e.g., through the `WHOIS` command).
	Like the `username`, the realname is also provided in the `USER` command:

	- `USER <username> <hostname> <servername> <realname>`

	For example:

	- `USER charles * * :Charles Doe`

	The * * fields are placeholders for hostname and servername, which are often ignored by modern servers.

- `_isAuthenticated`

	Boolean type. It will store the user's authentication status.

- `_isRegistered`

	Boolean type. It will store the user's registration status.

- `_channelsJoined`

	A map container storing the name of the channels the user has joined (string type, it's the map key) and a boolean to know whetere the user is a channel's administrator.

- `_buffer`

	String type. Allows the server to store incomplete data until the full message or command is received.

-------------------------------------------------------

## Authentication vs. Registration

In IRC protocol, **authentication** and **registration** are distinct steps.

1. First, **authentication** is completed when the user provides the correct `password`.

2. Next, user must complete **registration** by providing a `nickname` and a `username` (the real name)

The user will not be able to send messages or join channels until he is properly registered.

-------------------------------------------------------

## `_buffer` attribute

A `_buffer` attribute in the Client class is needed to temporarily store incomplete or incoming data from the client socket until it can be processed.
Data sent by clients over a network is received in chunks. A single `recv()` call might not always receive a complete command or message, especially if the message is large or the network is slow. `_buffer` allows the server to store incomplete data until the full message or command is received.
IRC messages are line-based and use `\r\n` as the delimiter. A client might send multiple commands in a single transmission, or a command might span multiple transmissions. The `_buffer` allows the server to:

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

- Once a complete command is found, it is extracted from `_buffer` and processed.
- Any remaining data after the `\r\n` is left in `_buffer` for further processing. 
- Example:

```c++
size_t pos = _buffer.find("\r\n");
if (pos != std::string::npos) {
    std::string command = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 2); // Remove the processed command and \r\n
    processCommand(command);
}
```

In a nutshell, the `_buffer` helps:

- Reassembling fragmented messages: It ensures commands like `PRIVMSG` are fully received.
- Handling concatenated commands: When a client sends multiple commands in one packet, `_buffer`can split them correctly.
- Preventing errors: Without a `_buffer`, partial messages might be discarded or cause parsing errors.

### Why set a `_buffer`'s size

Setting a maximum buffer size prevents the server from using too much memory if a client sends a large or malformed message.

In IRC, the maximum line length is defined by the protocol:

- Per **RFC 2812** (IRC protocol standard), the **maximum length of a message**, including the `\r\n`, is **512 characters**.
- This includes the command, parameters, and any prefixes.

How to enforce the buffer size:

- Use a `std::string` (which dynamically grows) but validate its size when appending data.
- If the buffer size exceeds a predefined limit (e.g., 512 characters for IRC), you can:

    * Disconnect the client (common in IRC servers).
    * Log the event or send an error response (e.g., `ERR_INPUTTOOLONG`).

```c++
if (_buffer.size() > MAX_BUFFER_SIZE) {
    sendError("ERR_INPUTTOOLONG");
    disconnect();
}
```

### Where to set the `_buffer`: Server or Client?

| Aspect | Client Sets the Size | Server Sets the Size |
| --- | --- | --- |
| Responsibility | Buffer logic is localized to the client. | Server enforces a global policy. |
| Flexibility | Each client can potentially have unique limits (not common in IRC). | All clients share the same global limit. |
| Complexity | Simpler design; clients handle their own issues. | Server manages buffer limits for all clients. |
| Centralization | Decentralized; limits are hardcoded per client. | Centralized; easier to modify system-wide. |

Example of implementation of `_buffer` in Client:

```c++
class Client {
private:
    std::string _buffer;
    static const size_t MAX_BUFFER_SIZE = 512; // Protocol limit

public:
    void appendToBuffer(const std::string& data) {
        _buffer += data;
        if (_buffer.size() > MAX_BUFFER_SIZE) {
            std::cerr << "Buffer overflow for client: " << /* client ID */ std::endl;
            // Handle overflow (e.g., clear buffer, send error, disconnect client)
        }
    }
};
```

Example of implementation of `_buffer` in Server:

```c++
class Server {
private:
    static const size_t MAX_BUFFER_SIZE = 512; // Protocol limit

public:
    void appendToClientBuffer(Client& client, const std::string& data) {
        client.appendToBuffer(data, MAX_BUFFER_SIZE);
    }
};

class Client {
private:
    std::string _buffer;

public:
    void appendToBuffer(const std::string& data, size_t maxBufferSize) {
        _buffer += data;
        if (_buffer.size() > maxBufferSize) {
            std::cerr << "Buffer overflow for client: " << /* client ID */ std::endl;
            // Handle overflow (e.g., clear buffer, send error, disconnect client)
        }
    }
};
```