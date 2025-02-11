# EXCHANGE

The server will receive commands from clients, process them, and send replies. Here's a breakdown of the requests the server will receive and the responses it should send.

## 1️⃣ Authentication & Connection Setup

**🔹 Client sends:**

```
PASS mypassword
NICK myNick
USER myUsername 0 * :My Real Name
```

- PASS <password> → If required by the server (optional)
- NICK <nickname> → Set the client's nickname
- USER <username> 0 * :<realname> → Set the client's username and real name

**🔹 Server replies:**

```
:server 001 myNick :Welcome to the IRC server!
:server 002 myNick :Your host is server, running version 1.0
:server 003 myNick :This server was created today
:server 004 myNick server 1.0 i
```

- 001 → Welcome message
- 002, 003, 004 → Server info

## 2️⃣ Channel Operations

**🔹 Client joins a channel**

```JOIN #channel```

**✅ Server replies:**

```
:myNick JOIN #channel
:server 331 myNick #channel :No topic is set
:server 353 myNick = #channel :@OperatorUser GuestUser
:server 366 myNick #channel :End of /NAMES list.
```

- JOIN → Notify all members in the channel
- 331 → No topic set (or 332 with topic)
- 353 → Names list (@ indicates operator)
- 366 → End of names list

## 3️⃣ Messaging

**🔹 Private message to a user**

```PRIVMSG John :Hello there!```

**✅ Server forwards the message to John:**

:myNick PRIVMSG John :Hello there!

    The server relays the message without modification.

🔹 Message to a channel

PRIVMSG #channel :Hey everyone!

✅ Server forwards it to all users in the channel

:myNick PRIVMSG #channel :Hey everyone!

4️⃣ Operator Commands (Kick, Invite, Topic, Mode)
🔹 Kick a user

KICK #channel GuestUser :Reason

✅ Server notifies all users in #channel

:myNick KICK #channel GuestUser :Reason

🔹 Invite a user to a channel

INVITE GuestUser #channel

✅ Server notifies the invited user

:myNick INVITE GuestUser #channel

🔹 Change topic

TOPIC #channel :New Topic!

✅ Server updates the topic

:myNick TOPIC #channel :New Topic!

🔹 Change channel mode

MODE #channel +i

✅ Server confirms mode change

:myNick MODE #channel +i

5️⃣ Client Disconnect
🔹 Client sends:

QUIT :Goodbye!

✅ Server notifies all users in their channels

:myNick QUIT :Goodbye!

    The server should remove the client from all channels and free resources.

Summary

📌 Your server will receive:

    Authentication commands (PASS, NICK, USER)
    Channel actions (JOIN, PART, TOPIC, MODE, KICK, INVITE)
    Messaging commands (PRIVMSG, NOTICE)
    Client disconnect (QUIT)

📌 Your server should respond with:

    Numeric replies (001, 002, 353, etc.) for system messages
    Prefixed messages (:Nick PRIVMSG #channel :text) for relayed messages
    Mode and operator commands responses (:Nick MODE #channel +o)

🚀 In short: Your server receives clean commands from clients and sends prefixed replies when needed.
