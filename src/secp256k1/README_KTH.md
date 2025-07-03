# secp256k1 in Knuth

This directory contains the secp256k1 implementation updated from Bitcoin Cash Node (BCHN).

## Update Information

The latest update was performed on July 2, 2025, taking the source code from:
```
bitcoin-cash-node-master/src/secp256k1/
```

## Configuration Options

The secp256k1 module now uses the following options in conanfile.py:

| Option | Type | Description |
|--------|------|-------------|
| secp256k1_enable_coverage | bool | Enables code coverage generation |
| secp256k1_enable_branch_coverage | bool | Enables branch coverage |
| secp256k1_enable_bignum | bool | Uses GMP bignum implementation |
| secp256k1_use_asm | bool | Uses assembly optimizations |
| secp256k1_enable_module_ecdh | bool | Enables ECDH module |
| secp256k1_enable_module_multiset | bool | Enables MULTISET module |
| secp256k1_enable_module_recovery | bool | Enables recovery module |
| secp256k1_enable_module_schnorr | bool | Enables Schnorr module |
| secp256k1_enable_external_default_callbacks | bool | Enables external default callbacks |
| secp256k1_enable_endomorphism | bool | Enables endomorphism optimization |
| secp256k1_ecmult_window_size | int | Window size for ecmult precomputation (2-24) |
| secp256k1_ecmult_gen_precision | int (2,4,8) | Precision bits to tune precomputed table size for signing |
| secp256k1_ecmult_static_precomputation | bool | Precomputes elliptic curve multiplication tables |
| secp256k1_enable_jni | bool | Enables JNI interface |
| use_field | string | Forces finite field implementation ("", "32bit", "64bit") |
| use_scalar | string | Forces scalar implementation ("", "32bit", "64bit") |

## Code Changes

The update includes:
- Updated CMake build system
- Improved handling of optimizations for different architectures
- Enhanced support for modules: ECDH, MULTISET, Recovery, Schnorr
- Improvements in tests and benchmarks

## Java / JNI

The JNI interface is now disabled by default (`secp256k1_enable_jni`=False). If you need JNI functionality, you can enable this option.

## How to Compile secp256k1 Only

To compile only the secp256k1 module:

```bash
cd src/secp256k1
mkdir build && cd build
cmake ..
make
```

## References

- [Original libsecp256k1 repository](https://github.com/bitcoin-core/secp256k1)
- [Official secp256k1 documentation](https://github.com/bitcoin-core/secp256k1/tree/master/doc)
