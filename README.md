# Qt Live Room System

A live room system based on Qt, TCP and MySQL.

## Tech Stack
- C++
- Qt (Qt Widgets)
- TCP Socket
- JSON data communication
- MySQL

## Features
- User registration and login
- Create live room
- Join / leave room
- Online user list
- Real-time chat
- Live room start / end
- Room list synchronization

The system uses a **Client–Server architecture**.

Client (Qt GUI)
- Login interface
- Register interface
- Room list interface
- Live room interface

Server (Qt TCP Server)
- User management
- Room management
- Online user management
- Message forwarding

The client adopts a **central control design**, where all network messages are processed by the main controller and distributed to different UI modules.

## Author
Susan Guo
