# Connection Registration

Immediately upon establishing a connection the client must attempt registration, without waiting for any banner message from the server.

Until registration is complete, only a limited subset of commands SHOULD be accepted by the server. This is because it makes sense to require a registered (fully connected) client connection before allowing commands such as `JOIN`, `PRIVMSG` and others.

The recommended order of commands during registration is as follows:

1. `CAP LS 302`
2. `PASS`
3. `NICK` and `USER`
4. Capability Negotiation
5. SASL (if negotiated)
6. `CAP END`

The commands specified in steps 1-3 should be sent on connection. If the server supports capability negotiation then registration will be suspended and the client can negotiate client capabilities (steps 4-6). If the server does not support capability negotiation then registration will continue immediately without steps 4-6.

1. If the server supports capability negotiation, the `CAP` command suspends the registration process and immediately starts the capability negotiation process. `CAP LS 302` means that the client supports version 302 of client capability negotiation. The registration process is resumed when the client sends `CAP END` to the server.

2. The `PASS` command is not required for the connection to be registered, but if included it MUST precede the latter of the `NICK` and `USER` commands.

3. The `NICK` and `USER` commands are used to set the user’s nickname, username and “real name”. Unless the registration is suspended by a `CAP` negotiation, these commands will end the registration process.

4. The client should request advertised capabilities it wishes to enable here.

5. If the client supports `SASL` authentication and wishes to authenticate with the server, it should attempt this after a successful `CAP ACK` of the sasl capability is received and while registration is suspended.

6. If the server support capability negotiation, `CAP END` will end the negotiation period and resume the registration.

If the server is waiting to complete a lookup of client information (such as hostname or ident for a username), there may be an arbitrary wait at some point during registration. Servers SHOULD set a reasonable timeout for these lookups.

Additionally, some servers also send a `PING` and require a matching `PONG` from the client before continuing. This exchange may happen immediately on connection and at any time during connection registration, so clients MUST respond correctly to it.

Upon successful completion of the registration process, the server MUST send, in this order:

1. `RPL_WELCOME (001)`,
2. `RPL_YOURHOST (002)`,
3. `RPL_CREATED (003)`,
4. `RPL_MYINFO (004)`,
5. at least one `RPL_ISUPPORT (005)` numeric to the client.
6. The server MAY then send other numerics and messages.
7. The server SHOULD then respond as though the client sent the `LUSERS` command and return the appropriate numerics.
8. The server MUST then respond as though the client sent it the `MOTD` command, i.e. it must send either the successful _Message of the Day_ numerics or the `ERR_NOMOTD (422)` numeric.
9. If the user has client modes set on them automatically upon joining the network, the server SHOULD send the client the `RPL_UMODEIS (221)` reply or a `MODE` message with the client as target, preferably the former.

The first parameter of the `RPL_WELCOME (001)` message is the nickname assigned by the network to the client. Since it may differ from the nickname the client requested with the `NICK` command (due to, e.g. length limits or policy restrictions on nicknames), the client SHOULD use this parameter to determine its actual nickname at the time of connection. Subsequent nickname changes, client-initiated or not, will be communicated by the server sending a `NICK` message.