# Servers [RFC 2812 1.1]

Servers are uniquely identified by their name, which has a maximum length of sixty three (63) characters.  See the protocol grammar rules (section 2.3.1) for what may and may not be used in a server name.

# getaddrinfo()
The `getaddrinfo()` function can be used by both clients and servers in networking, but its purpose slightly differs depending on the context:

- **Clients**: 
	
	* Use `getaddrinfo()` to look up the server's address for connecting.
	* The function provides details such as the IP address, port, and socket type (e.g., `SOCK_STREAM` for TCP or `SOCK_DGRAM` for UDP) to use when creating a socket connection to a server.

- **Servers**:

	* Servers use `getaddrinfo()` to configure the address to bind and listen on. It is used to prepare a socket for listening on a specific IP address and port. 
	* Servers usually specify the local address (e.g., NULL for any address, or a specific IP like 127.0.0.1) and the port number to bind to.

If you know exactly what IP address, protocol, and port you want to use, you can directly fill out a struct `sockaddr_in` (for IPv4) or struct `sockaddr_in6` (for IPv6) manually without using getaddrinfo().

> ft_irc subject says:
> _Communication between client and server has to be done via TCP/IP (v4 or v6)._