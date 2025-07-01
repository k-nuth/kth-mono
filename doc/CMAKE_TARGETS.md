# Kth Mono Repository - CMake Targets

This Kth mono repository exposes multiple CMake targets that allow users to import only the components they need.

## Available Targets

After calling `find_package(kth REQUIRED)`, the following targets are available:

### Individual Component Targets

- `kth::infrastructure` - Basic utilities, logging, fundamental data structures
- `kth::secp256k1` - Elliptic curve cryptographic library
- `kth::domain` - Domain models and blockchain business logic
- `kth::consensus` - Consensus rules and validation
- `kth::database` - Data access layer and persistence
- `kth::blockchain` - Blockchain management and operations
- `kth::network` - Network layer (not available in Emscripten builds)
- `kth::node` - Complete node implementation

### Optional Targets

- `kth::node-exe` - Node executable (console application) - only if built
- `kth::c-api` - C API for interoperability - only if built with BUILD_C_API=True

### Meta Target

- `kth::kth` - Target that includes all core components (equivalent to previous behavior)

## Usage Examples

### Using Specific Components Only

If you only need to work with domain models and validation:

```cmake
find_package(kth REQUIRED)

target_link_libraries(my_application 
    PRIVATE 
    kth::domain 
    kth::consensus
)
```

### Using the Complete Network Stack

For an application that needs full network functionality:

```cmake
find_package(kth REQUIRED)

target_link_libraries(my_application 
    PRIVATE 
    kth::node
    kth::network
    kth::blockchain
)
```

### Using All Kth (Traditional Behavior)

To use the entire Kth ecosystem as before:

```cmake
find_package(kth REQUIRED)

target_link_libraries(my_application 
    PRIVATE 
    kth::kth
)
```

### Cryptography Only

If you only need cryptographic functions:

```cmake
find_package(kth REQUIRED)

target_link_libraries(my_application 
    PRIVATE 
    kth::secp256k1
    kth::infrastructure  # For basic utilities
)
```

## Conanfile.py

In your `conanfile.py`:

```python
def requirements(self):
    self.requires("kth/0.68.2", transitive_headers=True, transitive_libs=True)
```

## Benefits

1. **Modularity**: You can import only what you need, reducing compilation time and binary size
2. **Clarity**: It's explicit which components your application uses
3. **Flexibility**: You can use different combinations of components according to your needs
4. **Compatibility**: The `kth::kth` target maintains compatibility with previous usage

## Dependencies Between Components

```
kth::secp256k1          (independent)
kth::infrastructure    ← kth::secp256k1
kth::domain            ← kth::infrastructure
kth::consensus         ← kth::secp256k1
kth::database          ← kth::domain
kth::blockchain        ← kth::database
kth::network           ← kth::domain
kth::node              ← kth::blockchain, [kth::network]
kth::node-exe          ← kth::node
kth::c-api             ← kth::node
```

**Note**: `kth::network` is not available in Emscripten builds.
