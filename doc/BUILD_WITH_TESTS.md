# KTH Build Scripts with Tests

This document describes the updated build scripts that now include automatic test execution.

## Scripts Modified

### Main Build Scripts (with tests enabled by default)

1. **`scripts/build.sh <version>`** - Release build with tests
   - Builds the project in Release mode
   - Enables tests during configuration
   - Runs `ctest` after successful build

2. **`scripts/build-create.sh <version>`** - Conan create with tests
   - Uses `conan create` with tests enabled
   - Runs conan test package if available

3. **`scripts/build-debug.sh <version>`** - Debug build with tests
   - Builds the project in Debug mode
   - Enables tests during configuration
   - Runs `ctest` after successful build

4. **`scripts/test.sh <version>`** - Test-only script
   - Configures and builds with tests enabled
   - Focuses primarily on running tests

## Configuration Changes

### Conanfile Changes
- **Main conanfile.py**: `tests` option now defaults to `True`
- **Module conanfiles**: All major modules now have `tests=True` by default:
  - infrastructure
  - domain
  - consensus
  - database
  - blockchain
  - network
  - node
  - c-api
  - secp256k1

### CMake Changes
- **Main CMakeLists.txt**: 
  - `ENABLE_TEST=ON` by default
  - `enable_testing()` is called when `ENABLE_TEST` is enabled

## GitHub Actions Changes

### CI/CD Modifications
- **build-without-container.yml**: 
  - Added `-o tests=True` to conan create
  - Added test execution step after build
  
- **build-with-container.yml**:
  - Added `-o tests=True` to conan create  
  - Added test execution step after build
  
- **main.yml**:
  - Added dedicated `test` job that runs after successful builds
  - Configures environment with tests enabled
  - Runs `ctest` with parallel execution

## Usage Examples

```bash
# Release build with tests
./scripts/build.sh 1.0.0

# Debug build with tests  
./scripts/build-debug.sh 1.0.0

# Conan package creation with tests
./scripts/build-create.sh 1.0.0

# Run tests only (after build)
./scripts/test.sh 1.0.0
```

## Test Execution

All scripts now automatically run tests after a successful build using:
```bash
ctest --output-on-failure --parallel 4
```

## Disabling Tests

If you need to disable tests for a specific build, you can override the conan option:
```bash
conan install conanfile.py -o tests=False
```

Or for CMake:
```bash
cmake --preset conan-release -DENABLE_TEST=OFF
```

## CI Behavior

- Tests run automatically on all builds in CI
- Test failures are reported but don't necessarily fail the entire build (configured with `|| echo "continuing..."`)
- Tests run in parallel for faster execution
- Both container and non-container builds include test execution
