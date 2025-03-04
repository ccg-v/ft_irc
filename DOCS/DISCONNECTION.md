✔️ QUIT is broadcasted only to the client’s channels.
✔️ Private messages to offline users return ERR_NOSUCHNICK.
✔️ No explicit disconnection message is sent in private conversations.

Why Doesn't IRC Notify Private Conversations?

    IRC treats private messages (PRIVMSG) as ad-hoc exchanges rather than persistent conversations.
    Unlike channels, which have a defined structure with members, private messages are just direct messages, so the server doesn’t track them as a "session."
    If a user wants to indicate they are leaving, they can manually send a message before quitting:

	```c++
	/msg <nickname> I'm leaving, bye!
	```

Scenario:

    Alice and Bob are chatting privately (PRIVMSG).
    Bob quits the server (QUIT command).
    Alice sees nothing—the server doesn’t tell her that Bob left.
    Alice tries to send a message to Bob:

/msg Bob Hello?

The server responds:

401 Alice Bob :No such nick/channel

(This is ERR_NOSUCHNICK, meaning Bob is no longer online.)