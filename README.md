# HTTP Server

## Overview
This project is a simple HTTP server implemented in C. It is designed to handle basic HTTP requests and serve static files efficiently.

## Features
- Handles GET requests.
- Serves static files from a specified directory.
- Multi-threaded for handling multiple client connections.
- Lightweight and fast.

## Requirements
- GCC or any C compiler.
- POSIX-compliant operating system (Linux, macOS, etc.).

## Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/your-username/http_server.git
    cd http_server
    ```
2. Create and navigate to build directory:
    ```bash
    mkdir build && cd build
    ```
3. Configure and build the project:
    ```bash
    cmake ..
    make
    ```



## Usage
1. Run the server:
    ```bash
    ./http_server <port> <directory>
    ```
    - `<port>`: Port number to listen on.
    - `<directory>`: Directory to serve files from.

2. Access the server in your browser:
    ```
    http://localhost:<port>
    ```

## Example
```bash
./http_server 8080 /var/www/html
```

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.