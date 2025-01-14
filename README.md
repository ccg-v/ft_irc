# ft_irc

## What is a socket

- It's a way to communicate two processes using standard UNIX file descriptors.
- When the system routine `socket` is called, it returns the socket descriptor, and we communicate through it using the specialized `send()` and `recv`. We could just the normal `read()` and `write()` to communicate through the socket but `send()` and `recv()` offer much more control over the data transmission.
- There are all kinds of sockets, but we will only deal with *Internet Sockets*.

## Types of Internet Sockets

Two main types:
1. **Stream Sockets** (or 'SOCK_STREAM')
	- They are reliable two-way connected communication streams. 
	- Items sent will arrive error-free, and in the ssame order they were sent. This high level of data transmission quality is achieved thanks to the protocol they use: the **"Transmission Control Protocol"** or simply **TCP**[^1] (see RFC 793).
2. **Datagram Sockets** (`SOCK_DGRAM` or 'connectionless sockets')
	- They are not reliable: if you send a datagram, it may arrive out of order or simply do not arrive. The protocol they use is **"User Datagram Protocol"** or **UDP*.
	- Unlike stream sockets, datagram sockets don't need an open connection. A packet is built, an IP headers is slapped on it with destination information, and it is sent out. No connection needed.
	- They are generally used when a TCP stack is unavailable or when dropping a few packets is not important[^2]. The main reason for using an unreliable underlying protocol is **speed**. Making sure that data sent has arrived safely and in order makes transmission slower. When you are sending 40 positional updates per second of a player position in an online game, or 24 frames per second in a videoconference transmission, a few positions/frames dropped don't matter, or at least not as much as granting a fast transmission.


[^1]: The protocol is usually named as 'TCP/IP', where 'IP' stands for *'Internet Protocol'* (see RFC 791), but IP deals primarily with Internet routing and is not generally responsible for data integrity.
[^2]: UDP is the protocol used by some services like tftp ("trivial file transfer protocol", a little brother to FTP) or dhcpcd (a DHCP client) that transfer binary applications between hosts, where data cannot be lost if you expect the application to work when it arrives. The trick is that these programs have their own protocol on top of UDP. For example, the tfcp protocol says that for each packet sent, the recipient has to send back an acknoledgement packet. The sender of the original packet waits for the ACK packet for a reasonable time and, if it does not arrive, will re-transmit the original packet until he finally gets an ACK. This acknoledgment procedure is very important when implementing reliable SOCK_DGRAM applications.
