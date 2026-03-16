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

## Architecture
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

## Demo Video

A 2-minute demo showing:

- Client login and registration  
- Room creation and joining  
- Live chat and live room interaction  

Watch the demo on Bilibili: [Demo Video](https://space.bilibili.com/123818544/upload/video)

## Author
Susan Guo
