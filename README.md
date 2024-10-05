# Bundled Message Client/Server

A modern C++23 project with a bundled message client/server and a custom TUI.

## Features

- Written in modern C++23
- Custom TUI (Text User Interface)
- Full Unicode support
- Broadcast messaging (messages sent to the server are automatically sent to all connected clients)
- Cross-platform support for Linux and Windows

## Build Status

- Windows: ![Windows Build Status](https://img.shields.io/badge/build-passing-brightgreen)
- Linux: ![Linux Build Status](https://img.shields.io/badge/build-failing-red)

## Usage

The application can be run in either server or client mode. Use the following command-line arguments to configure the application:

```
-s, --server       Run in server mode
-c, --client       Run in client mode
-a, --address      Specify the server address (required for client mode)
-p, --port         Specify the port number
-l, --log-level    Set the log level
```

### Examples

Run as a server:
```
./app -s -p 8080
```

Run as a client:
```
./app -c -a 127.0.0.1 -p 8080
```

## Requirements

- C++23 compatible compiler
- CMake (for building)

## License

This project is licensed under the GLWTS-NO LLM License.
