# IRC SERVER

## Table of Contents
- [About](#about)
- [Installation](#installation)
- [Usage](#usage)
- [Key Concepts Learned](#key-concepts-learned)
- [Skills Developed](#skills-developed)
- [Project Overview](#project-overview)
- [IRC Commands Implemented](#irc-commands-implemented)
- [Server Architecture](#server-architecture)
- [Testing](#testing)
- [42 School Standards](#42-school-standards)


## About

This repository contains my implementation of the **IRC Server** project at 42 School.  
The IRC (Internet Relay Chat) server is a complex network programming project that recreates one of the earliest chat protocols on the internet, originally developed around 1988.  
Built entirely in **C++98**, this server handles multiple client connections simultaneously using **non-blocking I/O** and **socket programming** without threads, providing real-time messaging capabilities across channels and private conversations.

The project demonstrates mastery of network protocols, concurrent programming, and system-level socket management while strictly adhering to the IRC protocol specifications.


## Installation

### Prerequisites
- **C++ Compiler** with C++98 standard support
- **Make** utility
- **UNIX/Linux environment** (socket programming)
- **IRC Client** for testing (irssi(best), HexChat, WeeChat, etc.)

### Compilation
```bash
# Clone the repository
git clone https://github.com/TuroTheReal/IRC.git
cd IRC

# Compile the server
make

# Clean object files
make clean

# Clean everything
make fclean

# Recompile
make re
```


## Usage

### Starting the Server
```bash
# Start the IRC server
./ircserv <port> <password>

# Example
./ircserv 6667 mypassword
```

### Connecting with IRC Client
```bash
# Using irssi
irssi -c localhost -p 6667 -w mypassword

# Using netcat (for testing)
nc localhost 6667
```

### Basic IRC Commands
```irc
# Authenticate
PASS mypassword
NICK mynickname
USER username hostname servername :realname

# Join a channel
JOIN #general

# Send messages
PRIVMSG #general :Hello everyone!
PRIVMSG someuser :Private message

# Other commands
QUIT :Goodbye!
```


## Key Concepts Learned

### Network Programming Fundamentals
- **Socket Programming**: Creating, binding, listening, and accepting TCP connections
- **Non-blocking I/O**: Using select() and poll() for asynchronous operations
- **Protocol Implementation**: Deep understanding of IRC protocol specifications (RFC 1459/2812)
- **Client-Server Architecture**: Managing multiple concurrent connections efficiently

### Advanced C++ Programming
- **Object-Oriented Design**: Clean class hierarchies for clients, channels, and server management
- **STL Containers**: Effective use of maps, vectors, and sets for data organization
- **Exception Handling**: Robust error management and graceful failure recovery
- **Memory Management**: Proper resource allocation and cleanup in network contexts

### Concurrent Programming
- **Multiplexing I/O**: Handling multiple clients without threading
- **Event-Driven Programming**: Responding to network events in real-time
- **State Management**: Tracking client states, channel memberships, and permissions
- **Race Condition Prevention**: Thread-safe operations and data consistency

### System Administration
- **Network Protocols**: Understanding TCP/IP, port management, and network security
- **Server Configuration**: Managing server settings, passwords, and access control
- **Logging and Debugging**: Comprehensive error tracking and system monitoring
- **Performance Optimization**: Efficient algorithms for large-scale client management


## Skills Developed

- **Network Programming Mastery**: Professional-level socket programming and protocol implementation
- **Concurrent Systems Design**: Building scalable, non-blocking server architectures
- **Protocol Engineering**: Deep understanding of internet communication standards
- **Real-time Systems**: Creating responsive, low-latency communication platforms
- **System-Level C++**: Advanced C++ programming for system and network applications
- **Debugging Complex Systems**: Troubleshooting network issues and multi-client scenarios
- **Performance Engineering**: Optimizing server performance for concurrent connections


## Project Overview

The IRC server implements a subset of the IRC protocol, enabling real-time text communication between multiple clients through channels and private messages.
The server manages client authentication, channel operations, and message routing while maintaining protocol compliance.

### Core Components

**Connection Manager**: Handles incoming client connections, authentication, and maintains active client sessions using non-blocking socket operations.
**Channel System**: Manages channel creation, user privileges (operators), topic setting, and channel-specific commands like kicks and bans.
**Message Router**: Processes and forwards messages between clients, handling both channel broadcasts and private messages efficiently.
**Command Parser**: Interprets IRC protocol commands, validates syntax, and executes appropriate server responses.
**User Management**: Tracks client nicknames, user information, channel memberships, and operator privileges.


## IRC Commands Implemented

### Authentication Commands
- **PASS**: Set connection password
- **NICK**: Set/change nickname
- **USER**: Set user information
- **QUIT**: Disconnect from server

### Channel Commands
- **JOIN**: Join one or more channels
- **TOPIC**: View/set channel topic

### Communication Commands
- **PRIVMSG**: Send messages to channels or users

### Channel Management (Operator Commands)
- **KICK**: Remove user from channel
- **INVITE**: Invite user to channel
- **MODE**: Set channel/user modes
  - Channel modes: `+i` (invite-only), `+t` (topic restriction), `+k` (key), `+o` (operator), `+l` (user limit)

### Server Commands
- **PING/PONG**: Keep-alive mechanism
- **WHO**: Query user information
- **WHOIS**: Detailed user information

### Bot Commands
With irssi "privmsg: !<commands>", without "!<commands>"
- **HELP**: Display all the bots commands
- **WEATHER**: Show a fake weather
- **WHOAMI**: Detailed user information
- **TIME**: Give the exact time


## Server Architecture

### Non-blocking I/O Design
The server uses **select()** or **poll()** to monitor multiple file descriptors simultaneously,
allowing efficient handling of hundreds of concurrent connections without threading overhead.

### Protocol Compliance
Strict adherence to IRC protocol standards ensures compatibility with standard IRC clients like irssi, HexChat, and WeeChat.

### Memory Management
Careful resource management prevents memory leaks and handles client disconnections gracefully, maintaining server stability under load.

### Error Handling
Comprehensive error handling covers network failures, malformed commands, authentication errors, and resource exhaustion scenarios.



## Testing

### Manual Testing
```bash
# Test with multiple IRC clients
irssi -c localhost -p 6667 -w password
hexchat # Connect to localhost:6667

# Test channel operations
/join #test
/topic #test "Welcome to test channel"
/msg #test Hello everyone!
/kick someone "Reason for kick"
```


### Load Testing
- Multiple simultaneous connections
- Channel flooding scenarios  
- Rapid connect/disconnect cycles
- Invalid command sequences


## 42 School Standards

### Norm Requirements
- ✅ Maximum 25 lines per function
- ✅ Maximum 5 functions per file
- ✅ Proper indentation and formatting
- ✅ C++98 standard compliance
- ✅ Compilation without warnings (-Wall -Wextra -Werror)

### Project Standards
- ✅ No memory leaks (validated with valgrind)
- ✅ Non-blocking I/O implementation without threads
- ✅ IRC protocol compliance for standard clients
- ✅ Proper error handling and server stability
- ✅ Orthodox Canonical Form for classes
- ✅ Comprehensive command implementation

### Network Requirements
- ✅ Multiple simultaneous client connections
- ✅ Password-protected server access
- ✅ Channel operations and user privileges
- ✅ Private messaging between users
- ✅ Graceful handling of client disconnections

### Bonus Features (Optional)
- ✅ File transfer capabilities
- ✅ Bot functionality
- ✅ Additional IRC commands beyond requirements
- ✅ Advanced channel modes and restrictions
- ✅ Server-to-server communication basics



## Contact

- **GitHub**: [@TuroTheReal](https://github.com/TuroTheReal)
- **Email**: arthurbernard.dev@gmail.com
- **LinkedIn**: [Arthur Bernard](https://www.linkedin.com/in/arthurbernard92/)

---

<p align="center">
  <img src="https://img.shields.io/badge/Made%20with-C++-blue.svg"/>
  <img src="https://img.shields.io/badge/Protocol-IRC-green.svg"/>
  <img src="https://img.shields.io/badge/Networking-Socket%20Programming-red.svg"/>
</p>
