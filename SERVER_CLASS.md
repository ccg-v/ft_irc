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
>
> _"Communication between client and server has to be done via TCP/IP (v4 or v6)"._

The function prototype is:

```c++
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,
				const char *service,
				const struct addrinfo *hints,
				struct addrinfo **res);
```

1. Parameters:

- `node`:

	The hostname to connect ("www.example.com") or an IP address

- `service`:

	Can be a port number or the name of a particular service (found in The IANA Port List1 or the /etc/services file on your Unix machine) like “http” or “ftp” or
“telnet” or “smtp” or whatever.

- `hints`:

	Points to a `struct addrinfo` that you’ve already filled out with relevant information ('man getaddrinfo' to see the contents of a `addrinfo` struct).
	Here’s a sample call if you’re a server who wants to listen on your host’s IP address, port 3490.

	```c++
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;			// will point to the results

	memset(&hints, 0, sizeof hints);	// make sure the struct is empty
	hints.ai_family = AF_UNSPEC;		// don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
	hints.ai_flags = AI_PASSIVE;		// fill in my IP for me

	if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	// servinfo now points to a linked list of 1 or more struct addrinfos

	// ... do everything until you don't need servinfo anymore ....

	freeaddrinfo(servinfo); // free the linked-list
	```
	* `ai_family` can be `AF_INET` to accept IPv4, `AF_INET6` to accept IPv6 or `AF_UNSPEC` to accept both
	* `ai_flag` is set as `AI_PASSIVE`, the function `getaddrinfo()` will assign the address of my local host to the socket structures. If we want to hardcode it, we can add it straight in the function's call:

		```c++
		status = getaddrinfo("www.example.net", "3490", &hints, &servinfo);
		```
	
2. Returned values:

	If `getaddrinfo()` returns an error, we can print it out using the function `gai_strerror()`. 
	If everything works properly `servinfo` will point to a linked list[^1] of struct `addrinfos`, each of which contains a struct `sockaddr` of some kind that we
can use later

3. Clean exit:

	When everything is done we must free it all up before leaving with a call to `freeaddrinfo()`.

[^1]: How Many addrinfo Structs Are Typically Returned?
    - **Single Address, Single Protocol**:
        If the hostname resolves to one IP address and you specify only one protocol (e.g., SOCK_STREAM for TCP), the list will typically have one entry.

    - **Single Address, Multiple Protocols**:
        If the hostname resolves to one IP and you don’t filter protocols, the list may include multiple entries (e.g., one for TCP and one for UDP).

    - **Multiple Addresses, Single Protocol**:
        If the hostname resolves to multiple IPs (e.g., multiple A or AAAA records) and you specify one protocol, you’ll get one addrinfo per IP address.

    - **Multiple Addresses, Multiple Protocols**:
        If the hostname resolves to multiple IPs and you don’t filter protocols, you’ll get one addrinfo per combination of IP address and protocol.