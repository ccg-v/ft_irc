# How to test and interact with our IRC server
The *ft_irc* exercise focuses solely on building the server side of the IRC system. Creating a custom IRC client is not required.

Thus, to interact with our IRC server running locally in our own machine we are expected to use existing IRC clients like IRSSI or tools like Netcat. Both are widely used and follow the IRC protocol standards.

We must start running our own server locally, providing as arguments the port and password we chose:

```./ircserv <port> <password>```

Next we can either IRSSI or Netcat as client:

## 1. IRSSI as a Client
Since IRSSI is a fully-featured IRC client, it will allow us to test how our server handles real-world IRC client behavior. It supports standard IRC commands like `/join`, `/part`, `/nick`, and `/msg`.

To connect to our server:

- ```/connect localhost <port> <password>```

To join a channel:

- ```/join #channel```

If IRSSI works with our server, it's a good indication that our implementation adheres to the IRC protocol specifications (RFC 2812).

## 2. Netcat (NC) as a Client
Netcat is a minimalistic tool for sending and receiving raw text over a network connection. Unlike IRSSI, Netcat does not assume any IRC-specific behaviors. This allows you to test your server in a more controlled way, such as:

- Manually crafting and sending raw IRC messages like `NICK`, `USER`, `JOIN`, or `PRIVMSG`.
- Checking if your server correctly processes IRC commands and sends appropriate responses.
- Debugging specific scenarios by observing the exact text being sent and received.

It's a great tool for low-level testing and debugging, especially during development.

To connect with netcat, type in terminal:

- ```nc -C localhost <port>```

> The `-C` flag enables CRLF (Carriage Return Line Feed) line endings when sending data.
> It ensures that every line you send ends with `\r\n` (CRLF), which complies with the IRC protocol requirement for message termination.
> Without the -C flag, netcat may use only `\n` (LF) as the line-ending delimiter, which could lead to protocol errors when communicating with servers that strictly require CRLF, such as IRC (Internet Relay Chat) or HTTP.

Once connected, we must first authenticate:

- ```PASS <password>```

and then register:

```
NICK <nick_name>
USER <user_name> 0 * :<real_name>
```

Once registration is finished, we can join/create a channel:

- ```JOIN #<channel_name>```

and broadcast messages:

- ```PRIVMSG #<channel_name> :Hello, world!```
