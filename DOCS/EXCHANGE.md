# Communication exchange and error messages

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

```:myNick PRIVMSG John :Hello there!```

- The server relays the message without modification.

**🔹 Message to a channel**

```PRIVMSG #channel :Hey everyone!```

**✅ Server forwards it to all users in the channel**

```:myNick PRIVMSG #channel :Hey everyone!```

## 4️⃣ Operator Commands (Kick, Invite, Topic, Mode)

**🔹 Kick a user**

```KICK #channel GuestUser :Reason```

**✅ Server notifies all users in #channel**

```:myNick KICK #channel GuestUser :Reason```

**🔹 Invite a user to a channel**

```INVITE GuestUser #channel```

**✅ Server notifies the invited user**

```:myNick INVITE GuestUser #channel```

**🔹 Change topic**

```TOPIC #channel :New Topic!```

**✅ Server updates the topic**

```:myNick TOPIC #channel :New Topic!```

**🔹 Change channel mode**

```MODE #channel +i```

**✅ Server confirms mode change**

```:myNick MODE #channel +i```

## 5️⃣ Client Disconnect

**🔹 Client sends:**

```QUIT :Goodbye!```

**✅ Server notifies all users in their channels**

```:myNick QUIT :Goodbye!```

- The server should remove the client from all channels and free resources.

## Summary

📌 The server will receive:

- Authentication commands (PASS, NICK, USER)
- Channel actions (JOIN, PART, TOPIC, MODE, KICK, INVITE)
- Messaging commands (PRIVMSG, NOTICE)
- Client disconnect (QUIT)

📌 The server should respond with:

- Numeric replies (001, 002, 353, etc.) for system messages
- Prefixed messages (:Nick PRIVMSG #channel :text) for relayed messages
- Mode and operator commands responses (:Nick MODE #channel +o)

In short: The server receives clean commands from clients and sends prefixed replies when needed.

----

# Server numeric replies

IRC servers follow standard numeric replies for authentication and registration failures. We must use these standard responses so that IRC clients like IRSSI and HexChat properly understand them.

Here are the correct numeric replies for common authentication/registration failures:

## 1️⃣ Wrong Password (PASS command fails)

**Standard Reply:**

```464 * :Password incorrect```

🔹 **464 ERR_PASSWDMISMATCH** → Sent when the provided password is incorrect.

## 2️⃣ Nickname Already in Use (NICK command fails)

**Standard Reply:**

```433 * myNick :Nickname is already in use```

🔹 **433 ERR_NICKNAMEINUSE** → Sent when the chosen nickname is already taken by another user.

## 3️⃣ Invalid Nickname (NICK command fails)

**Standard Reply:**

```432 * badNick :Erroneous nickname```

🔹 **432 ERR_ERRONEUSNICKNAME** → Sent when the provided nickname is not valid (e.g., contains invalid characters).

## 4️⃣ Invalid Username (USER command fails)

🔹 There is no official error code for an invalid username, but some servers use:

```461 * USER :Not enough parameters```

🔹 **461 ERR_NEEDMOREPARAMS** → Used when a command is missing required parameters.

If the username fails for another reason (like being blacklisted), you can define a custom error, but most servers do not enforce strict username rules.

## Other Standard Registration Errors

🔹 **436 ERR_NICKCOLLISION** → If the nickname collides with another user's nickname during registration.

🔹 **451 ERR_NOTREGISTERED** → If the client tries to use commands before completing registration.


Official list of numeric replies (error codes and responses):

https://defs.ircdocs.horse/defs/numerics#err-nicknameinuse-433

More information here: 

https://modern.ircdocs.horse/#cap-message

---

## Compatibility with incorrect software

Servers SHOULD handle single \n character, and MAY handle a single \r character, as if it was a \r\n pair, to support existing clients that might send this. However, clients and servers alike MUST NOT send single \r or \n characters.

Servers and clients SHOULD ignore empty lines.

**Servers SHOULD gracefully handle messages over the 512-bytes limit**. They may:

- Send an error numeric back, preferably `ERR_INPUTTOOLONG (417)`
- Truncate on the 510th byte (and add \r\n at the end) or, preferably, on the last UTF-8 character or grapheme that fits.
- Ignore the message or close the connection – but this may be confusing to users of buggy clients.

Finally, clients and servers SHOULD NOT use more than one space (\x20) character as SPACE as defined in the grammar above.

