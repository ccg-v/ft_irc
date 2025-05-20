# How to test and interact with our IRC server
The *ft_irc* exercise focuses solely on building the server side of the IRC system. Creating a custom IRC client is not required.

Thus, to interact with our IRC server running locally in our own machine we are expected to use existing IRC clients like IRSSI or tools like Netcat. Both are widely used and follow the IRC protocol standards.

We must start running our own server locally, providing as arguments the port and password we chose:

`./ircserv <port> <password>`

Next we can either IRSSI or Netcat as client:

## 1. IRSSI as a Client
Since IRSSI is a fully-featured IRC client, it will allow us to test how our server handles real-world IRC client behavior. It supports standard IRC commands like `/join`, `/part`, `/nick`, and `/msg`.

To connect to our server:

- `/connect localhost <port> <password>`

To join a channel:

- `/join #channel`

If IRSSI works with our server, it's a good indication that our implementation adheres to the IRC protocol specifications (RFC 2812).

## 2. Netcat (NC) as a Client
Netcat is a minimalistic tool for sending and receiving raw text over a network connection. Unlike IRSSI, Netcat does not assume any IRC-specific behaviors. This allows you to test your server in a more controlled way, such as:

- Manually crafting and sending raw IRC messages like `NICK`, `USER`, `JOIN`, or `PRIVMSG`.
- Checking if your server correctly processes IRC commands and sends appropriate responses.
- Debugging specific scenarios by observing the exact text being sent and received.

It's a great tool for low-level testing and debugging, especially during development.

To connect with netcat, type in terminal:

- `nc -C localhost <port>`

> The `-C` flag enables CRLF (Carriage Return Line Feed) line endings when sending data.
> It ensures that every line you send ends with `\r\n` (CRLF), which complies with the IRC protocol requirement for message termination.
> Without the -C flag, netcat may use only `\n` (LF) as the line-ending delimiter, which could lead to protocol errors when communicating with servers that strictly require CRLF, such as IRC (Internet Relay Chat) or HTTP.

Once connected, we must first authenticate:

- `PASS <password>`

and then register:

```
NICK <nick_name>
USER <user_name> 0 * :<real_name>
```

## 3. Implemented commands

**NICK**:

- Displays the user's nickname:
	- `NICK`

**JOIN**:

- Joins an existing channel or creates it if it doesn't exist
	- `JOIN #<channel_name>`

**PRIVMSG**:

- to send a message to a single user:
	+ `PRIVMSG <nickname> :Hello!` (in _irssi_ clients, `/MSG`)
- to broadcast a message to a channel: 
	+ `PRIVMSG #<channel_name> :Hello, world!` (in _irssi_ clients, `/MSG`)

**KICK**:

- Throws a user off a channel
	+ `KICK <#channel_name> <nickname>`

**TOPIC**:

- Displays the current topic, the author and the timestamp it was set
	+ `TOPIC <#channel_name>`

- If the channel is has `MODE` set to `-t`, users can add or edit the topic:
	+ `TOPIC <#channel_name> :Welcome to channel!`

**INVITE**:

- Sends an invitation to a client to a channel that can be joined only by invitation
	+ `INVITE <nickname> <#channel_name>`

**MODE**:

- Switches several channel/user modes:
	+ `MODE <flag> <nickname/#channel_name>`

	Flags:
	+ `o`: Set a user as channel operator: `MODE +o <nickname> <#channel_name>`
	+ `t`: Allow/deny channel users to set/edit the topic: `MODE -t <#channel_name>`
	+ `i`: Admission by invitation only: `MODE -i <#channel_name>`
	+ `k`: Set password to join channel: `MODE -k <#channel_name> <password>`
	+ `l`: Set limited number of channel users: `MODE -l <#channel_name> <limit>`

Here's a brief [command cheatsheet](#https://dl.icdst.org/pdfs/files3/01a99ce6eac6a0c1b18303941c088446.pdf)
