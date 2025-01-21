# Channels 

## [RFC-2812 1.3]

Channels names are strings (beginning with a '&', '#', '+' or '!' requirement that the first character is either '&', '#', '+' or '!', the only restriction on a channel name is that it SHALL NOT contain any spaces (' '), a control G (^G or ASCII 7), a comma (',').  Space is used as parameter separator and command is used as a list item separator by the protocol).  A colon (':') can also be used as a delimiter for the channel mask.  Channel names are case insensitive.

## [RFC-1459]

### 1.2.1 Operators

To allow a reasonable amount of order to be kept within the IRC network, a special class of clients (operators) is allowed to perform general maintenance functions on the network.  Although the powers granted to an operator can be considered as 'dangerous', they are nonetheless required.  Operators should be able to perform basic
network tasks such as disconnecting and reconnecting servers as needed to prevent long-term use of bad network routing.  In recognition of this need, the protocol discussed herein provides for operators only to be able to perform such functions.  See sections 4.1.7 (SQUIT) and 4.3.5 (CONNECT).

A more controversial power of operators is the ability  to  remove  a user  from  the connected network by 'force', i.e. operators are able to close the connection between any client and server.   The justification for  this  is delicate since its abuse is both destructive and annoying.  For further details on this type of
action, see section 4.6.1 (KILL).

### 1.3 Channels

A channel is a named group of one or more clients which will all receive messages addressed to that channel. 
The channel is created implicitly when the first client joins it, and the channel ceases to exist when the last client leaves it.
While channel exists, any client can reference the channel using the name of the channel.

Channels names are strings (beginning with a '&' or '#' character) of length up to 200 characters.  Apart from the the requirement that the first character being either '&' or '#'; the only restriction on a channel name is that it may not contain any spaces (' '), a control G (^G or ASCII 7), or a comma (',') which is used as a list item separator by the protocol.

There are two types of channels allowed by this protocol.  One is a distributed channel which is known to all the servers that are connected to the network. These channels are marked by the first character being a only clients on the server where it exists may join it.  These are distinguished by a leading '&' character.  On top of these two types, there are the various channel modes available to alter the characteristics of individual channels.  See section 4.2.3 (MODE command) for more details on this.

To create a new channel or become part of an existing channel, a user is required to JOIN the channel.  If the channel doesn't exist prior to joining, the channel is created and the creating user becomes a channel operator.  If the channel already exists, whether or not your request to JOIN that channel is honoured depends on the current modes of the channel. For example, if the channel is invite-only, (+i), then you may only join if invited.  As part of the protocol, a user may be a part of several channels at once, but **a limit of ten (10) channels is recommended** as being ample for both experienced and novice users. See section 8.13 for more information on this.

If the IRC network becomes disjoint because of a split between two servers, the channel on each side is only composed of those clients which are connected to servers on the respective sides of the split, possibly ceasing to exist on one side of the split.  When the split is healed, the connecting servers announce to each other who they think is in each channel and the mode of that channel.  If the channel exists on both sides, the JOINs and MODEs are interpreted in an inclusive manner so that both sides of the new connection will agree about which clients are in the channel and what modes the channel has.

# 1.3.1 Channel operators

The channel operator (also referred to as a "chop" or "chanop") on a given channel is considered to 'own' that channel.  In recognition of this status, channel operators are endowed with certain powers which enable them to keep control and some sort of sanity in their channel. As an owner of a channel, a channel operator is not required to have reasons for their actions, although if their actions are generally antisocial or otherwise abusive, it might be reasonable to ask an IRC operator to intervene, or for the usersjust leave and go elsewhere and form their own channel.

The commands which may only be used by channel operators are:

	KICK    - Eject a client from the channel
	MODE    - Change the channel's mode
	INVITE  - Invite a client to an invite-only channel (mode +i)
	TOPIC   - Change the channel topic in a mode +t channel

A channel operator is identified by the '@' symbol next to their nickname whenever it is associated with a channel (ie replies to the NAMES, WHO and WHOIS commands).