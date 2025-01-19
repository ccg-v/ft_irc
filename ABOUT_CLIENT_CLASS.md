# ABOUT CLIENT CLASS

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

## AUTHENTICATION vs. REGISTRATION

In IRC protocol, **authentication** and **registration** are distinct steps.

1. First, **authentication** is completed when the user provides the correct `password`.

2. Next, user must complete **registration** by providing a `nickname` and a `username` (the real name)

The user will not be able to send messages or join channels until he is properly registered.