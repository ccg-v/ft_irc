# Servers [RFC 2812 1.1]

Servers are uniquely identified by their name, which has a maximum length of sixty three (63) characters.  See the protocol grammar rules (section 2.3.1) for what may and may not be used in a server name.

<details>

<summary><h3>1.</h3> <h2>getaddrinfo(), sockaddr()</h2><h3>:  Preparing to launch</h3></summary>
The `getaddrinfo()` function can be used by both clients and servers in networking, but its purpose slightly differs depending on the context:

- **Clients**: 
	
	* Use `getaddrinfo()` to look up the server's address for connecting.
	* The function provides details such as the IP address, port, and socket type (e.g., `SOCK_STREAM` for TCP or `SOCK_DGRAM` for UDP) to use when creating a socket connection to a server.

- **Servers**:

	* Servers use `getaddrinfo()` to configure the address to bind and listen on. It is used to prepare a socket for listening on a specific IP address and port. 
	* Servers usually specify the local address (e.g., NULL for any address, or a specific IP like 127.0.0.1) and the port number to bind to.

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

**1.1 Parameters**

- `node`:

	The hostname to connect ("www.example.com") or an IP address

- `service`:

	Can be a port number or the name of a particular service (found in The IANA Port List1 or the /etc/services file on your Unix machine) like “http” or “ftp” or
“telnet” or “smtp” or whatever.

- `hints`:

	Points to a `struct addrinfo` that you've already filled out with relevant information ('man getaddrinfo' to see the contents of a `addrinfo` struct).
	Here's a sample call if you're a server who wants to listen on your host's IP address, port 3490.

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
	
**1.2 Returned values**

- If `getaddrinfo()` returns an error, we can print it out using the function `gai_strerror()`. 
- If everything works properly `servinfo` will point to a linked list of struct `addrinfos`[^1], each of which contains a struct `sockaddr` of some kind that we
can use later

**1.3 Clean exit**

	When everything is done we must free it all up before leaving with a call to `freeaddrinfo()`.


**1.4 Hardcoding `sockaddr_in`**

> [!TIP]
> If you know exactly what IP address, protocol, and port you want to use, you can directly fill out a struct `sockaddr_in` (for IPv4) or struct `sockaddr_in6` (for IPv6) manually without using getaddrinfo().

>```c++
>struct sockaddr_in server_addr;
>int server_socket = socket(AF_INET, SOCK_STREAM, 0);
>
>if (server_socket < 0) {
>    perror("socket");
>    exit(1);
>}
>
>server_addr.sin_family = AF_INET;
>server_addr.sin_port = htons(8080); // Port 8080
>server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any local address
>
>if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
>    perror("bind");
>    exit(1);
>}
>
>listen(server_socket, 5);
>```
>What does *ft_irc* subject say?
>
>_"Communication between client and server has to be done via TCP/IP (v4 or v6)"._

[^1]: How many `addrinfo` Structs Are Typically Returned?
	- **Single Address, Single Protocol**:
        If the hostname resolves to one IP address and you specify only one protocol (e.g., SOCK_STREAM for TCP), the list will typically have one entry.
    - **Single Address, Multiple Protocols**:
        If the hostname resolves to one IP and you don’t filter protocols, the list may include multiple entries (e.g., one for TCP and one for UDP).
    - **Multiple Addresses, Single Protocol**:
        If the hostname resolves to multiple IPs (e.g., multiple A or AAAA records) and you specify one protocol, you’ll get one addrinfo per IP address.
    - **Multiple Addresses, Multiple Protocols**:
        If the hostname resolves to multiple IPs and you don’t filter protocols, you’ll get one addrinfo per combination of IP address and protocol.
	
	Why resolving a server to several addresses (mapping a single hostname to multiple IP addresses)?

	- A client attempts to connect to example.com. DNS resolves it to multiple IPs: 192.168.1.1, 192.168.1.2, and 192.168.1.3. If 192.168.1.1 is unreachable, the client can try the next IP in the list.
	- Modern clients often support both IPv4 and IPv6. Resolving a hostname to both an IPv4 address (A record) and an IPv6 address (AAAA record) allows the client to choose based on its capabilities and network.
	- In any case, for small-scale applications or services running on a single server, a hostname often resolves to just one IP address.

</details>

<details>

<summary><h3>2.</h3> <h2>socket()</h2><h3>: Getting the file descriptor</h3></summary>

```c++
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

**2.1 Parameters**

The arguments allow you to say what kind of socket you want (IPv4 or IPv6, stream
or datagram, and TCP or UDP).

You can either hardcode those values:

 - `domain` is `PF_INET` or `PF_INET6`[^2],
 - `type` is `SOCK_STREAM` or `SOCK_DGRAM`,
 - and `protocol` can be set to `0` to choose the proper protocol for the given type. Instead, you can call `getprotobyname()` to look up the protocol you want, *“tcp”* or *“udp”*.

Or use the values from the results of the call to `getaddrinfo()`, and feed them into socket() directly like this:

```c++
struct addrinfo hints, *res;

// do the lookup
// [pretend we already filled out the "hints" struct]
getaddrinfo("www.example.com", "http", &hints, &res);

// do error-checking on getaddrinfo(), and walk the "res" linked list
// looking for valid entries instead of just assuming the first one is good

int fdSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```

**2.2 Returned value**

Returns a socket descriptor that you can use in later system calls, or -1 on error.

[^2]: Protocol Family `PF_INET` is a close relative of the Address Family `AF_INET` that you can use when initializing the `sin_family` field in your struct `sockaddr_in`. In fact, they’re so closely related that they actually have the same value, and many programmers will call `socket()` and pass `AF_INET` as the first argument instead of `PF_INET`. However, the most correct thing to do is to use AF_INET in your struct sockaddr_in and PF_INET in your call to socket().

	The original design concept of the socket interface distinguished between protocol types (families) and the specific address types that each may use. It was envisioned that a protocol family may have several address types. Address types were defined by additional symbolic constants, using the prefix AF instead of PF. The AF-identifiers are intended for all data structures that specifically deal with the address type and not the protocol family. However, this concept of separation of protocol and address type has not found implementation support and the AF-constants were defined by the corresponding protocol identifier, leaving the distinction between AF and PF constants as a technical argument of no practical consequence. Indeed, much confusion exists in the proper usage of both forms

</details>

<details>

<summary><h3>3.</h3> <h2>bind()</h2><h3>: What port am I on</h3></summary>

Once you have a socket, in order to listen for incoming connections the server needs to associate the socket with a port on your local machine. The port number is used by the kernel to match an incoming packet to a certain process’s socket descriptor.

```c++
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
```
**3.1 Parameters**

- `sockfd` is the socket file descriptor returned by socket(). 
- `my_addr` is a pointer to a struct `sockaddr` that contains information about your address, namely, port and IP address. 
- `addrlen` is the length in bytes of that address.


</details>

What do I do when bind() reports “Address already in use”?
You have to use setsockopt() with the SO_REUSEADDR option on the listening socket
Check out the section on bind() and the section on select() for an example

SO_REUSEADDR allows other sockets to bind() to this port, unless there is an active
listening socket bound to the port already. This enables you to get around
those “Address already in use” error messages when you try to restart your
server after a crash.