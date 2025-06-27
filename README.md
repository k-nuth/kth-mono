# knuth <a target="_blank" href="https://github.com/k-nuth/kth-mono/releases">![Github Releases][badge.release]</a> <a target="_blank" href="https://github.com/k-nuth/kth-mono/actions">![Build status][badge.GithubActions]</a> <a href="#">![CPP][badge.cpp]</a> <a target="_blank" href="https://t.me/knuth_cash">![Telegram][badge.telegram]</a>

> High performance Bitcoin development platform.

Knuth is a high performance implementation of the Bitcoin protocol focused on users requiring extra performance and flexibility, what makes it the best platform for wallets, exchanges, block explorers and miners.

## Not just a node

Knuth is a multi-crypto full node, but it is also a development platform.

Knuth's core is written in C++23, on top of it we provide a set of libraries and modules written in various programming languages that you can use as basis for building your application.

At the moment we have libraries in the following languages: [C++](https://github.com/k-nuth/kth-mono/tree/master/src/node), [C](https://github.com/k-nuth/kth-mono/tree/master/src/c-api), [Javascript](https://github.com/k-nuth/js-api), [TypeScript](https://github.com/k-nuth/js-api), [JS/TS WebAssembly](https://github.com/k-nuth/js-wasm), [C#](https://github.com/k-nuth/cs-api) and [Python](https://github.com/k-nuth/py-api).
You can build your own library in the language of your choice on top of our [C library](https://github.com/k-nuth/kth-mono/tree/master/src/c-api).

## Core Components

Knuth provides multiple ways to use the platform:

### 🏗️ Core Infrastructure (This Repository)

### 🚀 Node Executable
The complete Bitcoin node that you can run out of the box. Perfect for running a full node, mining, or as a backend for applications that need blockchain access.

### 🔧 C++ Library
A high-performance C++23 library that gives you direct access to all Bitcoin protocol functionality. Ideal for building performance-critical applications.

### 🌐 C API
A stable C interface that provides access to all core functionality and serves as the foundation for bindings in other programming languages.

### 🌍 Language Bindings (Separate Repositories)

Built on top of the C API, these bindings allow you to use Knuth from your preferred programming language:

### 🟨 JavaScript & TypeScript
- **[JavaScript API](https://github.com/k-nuth/js-api)** - Full-featured Node.js binding
- **[WebAssembly](https://github.com/k-nuth/js-wasm)** - Browser-compatible WebAssembly binding

### 🟣 C# 
- **[C# API](https://github.com/k-nuth/cs-api)** - .NET binding for Windows, Linux, and macOS

### 🐍 Python
- **[Python API](https://github.com/k-nuth/py-api)** - Pythonic interface to Knuth functionality

## Getting Started

### Prerequisites

All Knuth components are now available through a single unified package:

1. Install and configure the Knuth build helper:
```bash
$ pip install kthbuild --user --upgrade
$ conan config install https://github.com/k-nuth/ci-utils/raw/master/conan/config2023.zip
```

2. Install the unified Knuth package:
```bash
$ conan install --requires=kth/0.68.0 --update --deployer=direct_deploy
```

This single installation provides you with:
- `bin/kth` - The node executable
- `lib/` - All static libraries (libnode.a, libc-api.a, etc.)
- `include/kth/` - C++ headers and C API headers

### 🚀 Running the Node Executable

After installation, you can immediately run the full Bitcoin node:

```bash
# Run the node
$ ./kth/bin/kth
```

### 🔧 Using the C++ Library

For C++ developers, link against the appropriate libraries:

```cpp
// example.cpp
#include <kth/node.hpp>

int main() {
    kth::node::node node{"config.cfg"};
    node.initchain();
    node.run();
    
    auto& chain = node.chain();
    auto height = chain.get_last_height();
    
    std::cout << "Current height: " << height << std::endl;
    return 0;
}
```

When compiling, link against the static libraries found in `lib/`:
```bash
$ g++ -std=c++23 example.cpp -I./kth/include -L./kth/lib -lnode -lblockchain -ldomain -linfrastructure
```

### 🌐 Using the C API

For C developers or language bindings:

```c
// hello_knuth.c
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <kth/capi.h>

int main() {
    // Create and configure node
    kth_node_t node = kth_node_construct("config.cfg", stdout, stderr);
    
    // Initialize blockchain database
    kth_node_initchain(node);
    
    // Run the node
    kth_node_run_wait(node);
    
    // Get blockchain interface
    kth_chain_t chain = kth_node_get_chain(node);
    
    // Query current height
    uint64_t height;
    chain_get_last_height(chain, &height);
    printf("Current height: %" PRIu64 "\n", height);
    
    // Cleanup
    kth_node_destruct(node);
    return 0;
}
```

When compiling, link against the C API library:
```bash
$ gcc hello_knuth.c -I./kth/include -L./kth/lib -lc-api
```

For more detailed instructions, please refer to our [documentation](https://kth.cash/docs/).

## Releases

<img width="50px" src="https://github.com/k-nuth/cs-api/raw/master/docs/images/kth-purple.png" /> <img src="https://img.shields.io/github/v/release/k-nuth/kth-mono?display_name=tag&style=for-the-badge&color=3b009b&logo=bitcoincash" />

<img alt="C++" src="https://kth.cash/images/libraries/cpp.svg" width="35" height="35" /> <img src="https://img.shields.io/github/v/release/k-nuth/kth-mono?display_name=tag&style=for-the-badge&color=00599C&logo=cplusplus" />

<img alt="C" src="https://kth.cash/images/libraries/c.svg" width="35" height="35" /> <img src="https://img.shields.io/github/v/release/k-nuth/kth-mono?display_name=tag&style=for-the-badge&color=A8B9CC&logo=c" />

<img alt="Javascript" src="https://kth.cash/images/libraries/javascript.svg" width="35" height="35" /> <img alt="TypeScript" src="https://kth.cash/images/libraries/typescript.svg" width="35" height="35" /> <img src="https://img.shields.io/npm/v/@knuth/bch?logo=npm&style=for-the-badge" />

<img alt="JS/TS WebAssembly" src="https://kth.cash/images/libraries/wasm.svg" width="35" height="35" /> <img src="https://img.shields.io/npm/v/@knuth/js-wasm?logo=npm&style=for-the-badge" />

<img alt="C#" src="https://kth.cash/images/libraries/csharp.svg" width="35" height="35" /> <img src="https://img.shields.io/nuget/v/kth-bch?logo=nuget&label=release&style=for-the-badge" />

<img alt="Python" src="https://kth.cash/images/libraries/python.svg" width="35" height="35" /> <img src="https://img.shields.io/pypi/v/kth?logo=python&style=for-the-badge&color=3776AB" />

## Key Features

### Performance matters

We designed Knuth to be a high performance node, so our build system has the ability to automatically detect the microarchitecture of your processor and perform an optimized build for it.

For those who don't want to wait for compilation times, we provide pre-built binaries compatible with [Intel's Haswell microarchitecture](https://en.wikipedia.org/wiki/Haswell_(microarchitecture)). But you don't have to worry about that, our build system will do everything for you.

### Modular architecture

Knuth is based on a modular architecture simple to modify, expand and learn.
Any protocol change can be introduced in Knuth much faster and more efficiently than in reference implementations.

### Cross-platform

Knuth can be used in any computer architecture and operating system, it only requires a 64-bit system.

Knuth has been well tested on x86-64 processors and on the following operating systems: FreeBSD, Linux, macOS and Windows. However, it is not limited to these, Knuth can be used in any computer architecture and any operating system, the only requirement is a 64-bit system.

If you find a problem in any other platform, please [let us know](https://github.com/k-nuth/kth-mono/issues).

## Language Bindings

<a href="https://github.com/k-nuth/kth-mono/tree/master/src/node"><img alt="C++" src="https://kth.cash/images/libraries/cpp.svg" width="120" height="120" /></a>
<a href="https://github.com/k-nuth/kth-mono/tree/master/src/c-api"><img alt="C" src="https://kth.cash/images/libraries/c.svg" width="120" height="120" /></a>
<a href="https://github.com/k-nuth/js-api"><img alt="Javascript" src="https://kth.cash/images/libraries/javascript.svg" width="120" height="120" /></a>
<a href="https://github.com/k-nuth/js-api"><img alt="TypeScript" src="https://kth.cash/images/libraries/typescript.svg" width="120" height="120" /></a>
<a href="https://github.com/k-nuth/js-wasm"><img alt="JS/TS WebAssembly" src="https://kth.cash/images/libraries/wasm.svg" width="120" height="120" /></a>
<a href="https://github.com/k-nuth/cs-api"><img alt="C#" src="https://kth.cash/images/libraries/csharp.svg" width="120" height="120" /></a>
<a href="https://github.com/k-nuth/py-api"><img alt="Python" src="https://kth.cash/images/libraries/python.svg" width="120" height="120" /></a>

Any protocol change can be introduced in Knuth much faster and more efficiently than in reference implementations.

## Donation

Knuth is a community backed project developed. Donations received will be used to subsidize development costs for general maintenance and support of our implementation.

Your contributions are greatly appreciated!

`bitcoincash:qrlgfg2qkj3na2x9k7frvcmv06ljx5xlnuuwx95zfn`

See [fund.kth.cash](https://fund.kth.cash/) for active Flipstarter campaigns.

## License

Knuth node is released under the terms of the MIT license. See COPYING for more information or seehttps://opensource.org/licenses/MIT

## Issues

Each of our modules has its own Github repository, but in case you want to create an issue, please do so in our [main repository](https://github.com/k-nuth/kth/issues).

## Contributing

### Branch Naming Conventions

To optimize CI/CD performance, we use specific branch prefixes that determine which workflows are executed:

- **`docs/`** - Documentation-only changes (fast validation, ~30s)
- **`style/`** - Code formatting, copyright updates, comment fixes (fast validation)
- **`chore/`** - Maintenance tasks, dependency updates (fast validation)
- **`noci/`** - Explicitly skip heavy CI (fast validation)

All other branches (`feature/`, `fix/`, `refactor/`, etc.) run the full CI pipeline.

📖 **See [Branch Conventions Guide](doc/BRANCH_CONVENTIONS.md) for detailed usage examples.**

## Contact

You can contact us through our [Telegram](https://t.me/knuth_cash) group or write to us at info@kth.cash.

## Security Disclosures
To report security issues please contact:

Fernando Pelliccioni (fpelliccioni@gmail.com) - GPG Fingerprint: 8C1C 3163 AAE1 0EFA 704C 8A00 FE77 07B7 4C29 E389

## Donation

See [fund.kth.cash](https://fund.kth.cash/) for active Flipstarter campaigns.

Our donation address is:
`bitcoincash:qrlgfg2qkj3na2x9k7frvcmv06ljx5xlnuuwx95zfn`

## Issues

Each of our modules has its own Github repository, but in case you want to create an issue, please do so in our [main repository](https://github.com/k-nuth/kth-mono/issues).

## Contact

You can contact us through our [Telegram](https://t.me/knuth_cash) group or write to us at info@kth.cash.


<!-- Links -->
[badge.Cirrus]: https://api.cirrus-ci.com/github/k-nuth/kth-mono.svg?branch=master
[badge.GithubActions]: https://img.shields.io/endpoint.svg?url=https%3A%2F%2Factions-badge.atrox.dev%2Fk-nuth%2Fkth-mono%2Fbadge&style=for-the-badge
[badge.version]: https://badge.fury.io/gh/k-nuth%2Fkth-kth-mono.svg
[badge.release]: https://img.shields.io/github/v/release/k-nuth/kth-mono?display_name=tag&style=for-the-badge&color=3b009b&logo=bitcoincash
[badge.cpp]: https://img.shields.io/badge/C++-23-blue.svg?logo=c%2B%2B&style=for-the-badge
[badge.telegram]: https://img.shields.io/badge/telegram-badge-blue.svg?logo=telegram&style=for-the-badge

<!-- [badge.Gitter]: https://img.shields.io/badge/gitter-join%20chat-blue.svg -->
