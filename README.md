# Yumina

[![codecov](https://codecov.io/gh/yuminaa/YHC/graph/badge.svg)](https://codecov.io/gh/yuminaa/YHC)
 
Yumina is a collection of high-performance, low-level, and cross-platform libraries for C++.
It is designed to provide efficient and optimized solutions for high-cost computational tasks.

## Features

- **High Performance**: Optimized for speed and efficiency.
- **Low-Level**: Provides fine-grained control over hardware and system resources.
- **Cross-Platform**: Compatible with multiple operating systems and architectures.

## Getting Started

### Prerequisites

- **C++ Compiler**: Ensure you have a modern C++ compiler installed (e.g., GCC, Clang, MSVC).
- **CMake**: Used for building the project. 3.29 or higher is recommended.

### Installation

1. **Clone the Repository**:
    ```sh
    git clone --recurse-submodules https://github.com/yuminaa/yumina.git
    cd yumina
    ```

2. **Build the Project**:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

## Contributing

Contributions are welcome! Please read the [guidelines](.github/CONTRIBUTING.md) on how to contribute to this project.

## License

This project is licensed under the [MIT License](LICENSE).

## Status

This project is currently under development and is not yet ready for production use.
Clang is the primary compiler used for development, but other compilers will be supported in the future.

## Acknowledgements

- [google/googletest](https://github.com/google/googletest)