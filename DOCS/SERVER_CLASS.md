# Servers [RFC 2812 1.1]

Servers are uniquely identified by their name, which has a maximum length of sixty three (63) characters.  See the protocol grammar rules (section 2.3.1) for what may and may not be used in a server name.

<details>

<summary><h3>1.</h3> <h2>getaddrinfo(), sockaddr()</h2></summary>
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

**1.4 Hardcoding `sockaddr()`**

> If you know exactly what IP address, protocol, and port you want to use, you can directly fill out a struct `sockaddr_in` (for IPv4) or struct `sockaddr_in6` (for IPv6) manually without using getaddrinfo().
>
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

</details>

-----------

<details>

<summary><h3>2.</h3> <h2>socket()</h2></summary>

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

<hr></hr>

<details>

<summary><h3>3.</h3> <h2>bind(), setsockopt()</h2></summary>

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

```c++
struct addrinfo hints, *res;
int sockfd;

// first, load up address structs with getaddrinfo():
memset(&hints, 0, sizeof hints);

hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;	// fill in my IP for me

getaddrinfo(NULL, "3490", &hints, &res);

// make a socket:
sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// bind it to the port we passed in to getaddrinfo():
bind(sockfd, res->ai_addr, res->ai_addrlen);
```

**3.2 Returned value**

- On  success, zero is returned. 
- On error, -1 is returned, and `errno` is set to indicate the error.

<h3>3.3 setsockopt()</h3>

Sometimes you try to rerun a server and bind() fails, claiming *“Address already in use”*. That means a little bit of a socket that was connected is still hanging around in the kernel, and it’s hogging the port. You can either wait for it to clear (a minute or so), or add code to your program allowing it to reuse the port.

```c++
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```
**3.3.1 Parameters**

- `sockfd`

	The file descriptor that identifies the socket we are working with.

- `level`

	- When manipulating socket options, the level at which the option resides and the name of  the  option  must  be specified. 
	- To  manipulate options at the sockets API level, level is specified as `SOL_SOCKET`.
	- Other levels are `IPPROTO_TCP` (fof TCP-specific options) and `IPPROTO_IP` (for IPv4-specific options).

- `optname`

	- Optname and any specified options are passed uninterpreted to the appropriate protocol module for interpretation.
	- `SO_REUSEADDR` allows other sockets to bind() to this port, unless there is an active listening socket bound to the port already. This enables you to get around those *“Address already in use”* error messages when you try to restart your server after a crash.
	- On Linux, `SO_REUSEPORT` enables multiple processes to bind to the same port and receive incoming connections evenly distributed by the kernel. It is often used together with `SO_REUSEADDR`, when the plan is running multiple instances of the server on the same machine.

- `optval`

	A pointer to the buffer where the value of the chosen option is specified.

- `optlen` 

	Size, in bytes, to the buffer pointed by previous parameter, `optval`  

**3.3.2 Return values**

- On success, zero is returned.
- On error, -1 is returned, and `errno` is set to indicate the error.

**3.3.3 Example**

```c++
int	yes = 1;

if (setsockopt(fdSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
	perror("setsockopt");
	exit(-1);
}
```

</details>

---

<details>

<summary><h3>4.</h3> <h2>listen()</h2></summary>

```c++
int listen(int sockfd, int backlog);
```

**4.1 Parameters**

- `sockfd`

	The socket file descriptor.

- `backlog`

	* Maximum number of connections allowed on the incoming queue
	* Most systems silently limit this number to about 20; you can probably get away with setting it to 5 or 10.

**4.2 Returned value**

- On success, zero is returned. 

- On error, -1 is returned, and `errno` is set to indicate the error.

</details>

---

<details>

<summary><h3>5.</h3> <h2>accept()</h2></summary>

`accept()` returns a brand new file descriptor to identify the established connection. The original one is still listening for more new connections,
and the newly created one is finally ready to `send()` and `recv()`.

```c++
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

**5.1 Parameters**

- `sockfd`

	The `listen()`ing socket descriptor.

- `addr`

	Pointer to a local struct `sockaddr_storage`, where the information about the incoming connection will be stored.

- `addrlen`

	Local integer variable that should be set to sizeof(struct `sockaddr_storage`) before its address is passed to `accept()`.

**5.2 Returned value**

- On success, returns a file  descriptor  for  the  accepted socket (a nonnegative integer). 

- On error, -1 is returned, `errno` is set to indicate the error, and `addrlen` is left unchanged.

</details>

---

<details>

<summary><h3>6.</h3> <h2>send() and recv()</h2></summary>

<h3>6.1 send</h3>

```c++
int send(int sockfd, const void *msg, int len, int flags);
```

**6.1.1 Parameters**

- `sockfd` 

	It's the socket descriptor you want to send data to (whether it’s the one returned by socket() or the one you got with accept()). 

- `msg`

	A pointer to the data you want to send.
	
- `len`

	The length of that data in bytes. 

- `flags`

	Just set flags to 0 (?).

**6.1.2 Returned value**

- `send()` returns the number of bytes actually sent out. If the value returned doesn’t match the value in `len`, it’s up to you to send the rest of the string.

- On error -1 is returned, and `errno` is set to the error number.

<h3>6.2 recv</h3>

```c++
int recv(int sockfd, void *buf, int len, int flags);
```

**6.2.1 Parameters**

- `sockfd`

	Socket descriptor to read from.

- `buf`

	Buffer to read the information into.

- `len`

	Maximum length of the buffer.
	
- `flags`

	Just set flags to 0 (?).

**6.2.2 Returned value**

- `recv()` returns the number of bytes actually read into the buffer.

- **`recv()` will return 0 if the remote side (client) has closed the connection gracefully (sent a FIN signal)**.

- On error -1 is returned, and `errno` is set to the error number.

</details>

---

<details>

<summary><h3>7.</h3> <h2>fcntl(), poll()</h2></summary>

<h3>7.1 fcntl()</h3>

```c++
#include <fcntl.h>

fcntl(sockfd, F_SETFL, O_NONBLOCK);
```

When a socket descriptor is created with `socket()`, by default the kernel sets it to blocking. This means every time a blocking command (like `accept()`, `recv()` or `send()`) is called, the program will stay _frozen_ or _asleep_ on that system call until some data is received.

`fcntl()` makes a single socket non-blocking, preventing any blocking system call from freezing the entire process if no data is available.

If the program tries to read from a non-blocking socket and there’s no data there, `recv` will return -1 and `errno` will be set to `EAGAIN` or `EWOULDBLOCK`.

You can use only `fcntl()` to set sockets as non-blocking and manually “poll” them by repeatedly calling `recv()` in a loop to check if data has arrived. However, this approach (_polling by looping_) is inefficient because it leads to busy-waiting, which wastes CPU cycles.

<h3>7.2 poll()</h3>

`poll()` monitors multiple sockets (_multiplexing_) at once to determine which ones need attention (ready for read/write/etc.) without needing multiple threads. It does not make sockets non-blocking but rather lets you check which sockets are ready.

Instead of constantly checking each socket, `poll()` does that dirty work for us and sleeps the server until at least one socket is ready for an operation (read, write, error, etc.). This means the server only wakes up when necessary and the CPU is not busy-waiting.

The general gameplan is to keep an **array of struct `pollfd`s** with information about which socket descriptors we want to monitor, and what kind of events we want to monitor for.

``` c++
struct pollfd {
	int fd;			// the socket descriptor
	short events;	// bitmap of events we're interested in
	short revents;	// when poll() returns, bitmap of events that occurred
};
```

We will create a `pollfd` struct for every socket we want to monitor, storing in `fd` the socket descriptor and in `events` the type of event we are interested in:

- `POLLIN`:  Alert me when data is ready to recv() on this socket.
- `POLLOUT`: Alert me when I can send() data to this socket without blocking.
- `POLLHUP`: Alert me when the remote closed the connection.



To sum up, a multi-client server should combine `fcntl()` to prevent blocking with `poll()` to handle multiple sockets efficiently.

</details>
