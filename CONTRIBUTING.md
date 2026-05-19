# Contributing to ModBus Protocol

## Getting Started

### Prerequisites
- C++17 or higher compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15+
- Git
- Google Test framework (for testing)

### Development Setup

```bash
# Clone the repository
git clone https://github.com/kanwarazeem/ModBus-protocol.git
cd ModBus-protocol

# Create build directory
mkdir -p build
cd build

# Configure with tests and coverage
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DENABLE_TESTS=ON \
         -DENABLE_COVERAGE=ON

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

## Development Workflow

### 1. Create a Feature Branch

```bash
git checkout -b feature/your-feature-name
```

### 2. Make Changes

- Follow C++17 standards
- Add Doxygen documentation
- Write unit tests for new code
- Update CMakeLists.txt if needed

### 3. Code Quality

```bash
# Static analysis
cppcheck --enable=all src/

# Clang-tidy
clang-tidy src/*.cpp -- -std=c++17

# Format code
clang-format -i src/**/*.{hpp,cpp}

# Memory checking (if available)
valgrind --leak-check=full ./build/tests/test_protocol
```

### 4. Run Tests

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific test
ctest -R test_crc -V

# Generate coverage report
make coverage

# Run tests with verbose output
ctest -V
```

### 5. Commit Changes

```bash
git add .
git commit -m "feat: add modbus tcp support"
```

### 6. Push and Create PR

```bash
git push origin feature/your-feature-name
```

Then open a Pull Request on GitHub.

## C++ Code Style Guide

### Naming Conventions

```cpp
// Classes: PascalCase
class ModBusMaster { };

// Functions: camelCase
void readHoldingRegisters();

// Constants: UPPER_CASE
constexpr uint16_t CRC_POLYNOMIAL = 0xA001;

// Member variables: m_camelCase
class MyClass {
    uint16_t m_slaveID;
};

// Local variables: camelCase
uint16_t registerValue = 0;
```

### Code Format

```cpp
// Header guard
#ifndef MODBUS_PROTOCOL_HPP
#define MODBUS_PROTOCOL_HPP

// Includes (std first, then system, then local)
#include <cstdint>
#include <vector>
#include <unistd.h>
#include "modbus/ModBusProtocol.hpp"

// Namespaces
namespace modbus {

class ModBusMaster {
public:
    // Public interface first
    
    /**
     * @brief Read holding registers from slave device
     * @param slaveID Slave device ID (1-247)
     * @param address Starting register address
     * @param quantity Number of registers to read
     * @param registers Output vector for register values
     * @return true on success, false on error
     */
    bool readHoldingRegisters(
        uint8_t slaveID,
        uint16_t address,
        uint16_t quantity,
        std::vector<uint16_t>& registers
    );

protected:
    // Protected methods
    
private:
    // Private implementation
    uint16_t m_timeout;
    uint8_t m_retries;
};

}  // namespace modbus

#endif  // MODBUS_PROTOCOL_HPP
```

### Documentation with Doxygen

```cpp
/**
 * @class ModBusMaster
 * @brief Master device implementation for ModBus protocol
 * 
 * Implements the ModBus master functionality for communicating
 * with multiple slave devices over serial connection (RS-232/RS-485).
 * 
 * @author Your Name
 * @version 1.0
 * @date 2026-05-19
 */

/**
 * @fn bool readCoils(uint8_t slaveID, uint16_t address, 
 *                    uint16_t quantity, std::vector<bool>& coils)
 * @brief Read coil values from slave device
 * 
 * Reads one or more coils from the slave device using
 * ModBus function code 01.
 * 
 * @param slaveID Target slave device ID (1-247)
 * @param address Starting coil address (0-65535)
 * @param quantity Number of coils to read (1-2000)
 * @param coils Output vector containing coil states
 * @return true if successful, false on error
 * 
 * @throw std::invalid_argument if parameters are invalid
 * @throw std::runtime_error if communication fails
 * 
 * @note Maximum coils per request: 2000
 * @see readDiscreteInputs()
 * 
 * @code
 * ModBusMaster master("/dev/ttyUSB0", 9600, ModBusMode::RTU);
 * std::vector<bool> coils;
 * if (master.readCoils(1, 0, 10, coils)) {
 *     for (auto coil : coils) {
 *         std::cout << (coil ? "ON" : "OFF") << std::endl;
 *     }
 * }
 * @endcode
 */
```

## Testing Guidelines

### Test File Structure

```cpp
#include <gtest/gtest.h>
#include "modbus/ModBusMaster.hpp"

class ModBusMasterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup before each test
        master = std::make_unique<ModBusMaster>(
            "/dev/ttyUSB0", 9600, ModBusMode::RTU
        );
    }
    
    void TearDown() override {
        // Cleanup after each test
        master.reset();
    }
    
    std::unique_ptr<ModBusMaster> master;
};

TEST_F(ModBusMasterTest, ReadCoilsSuccess) {
    // Arrange
    std::vector<bool> coils;
    
    // Act
    bool result = master->readCoils(1, 0, 10, coils);
    
    // Assert
    EXPECT_TRUE(result);
    EXPECT_EQ(coils.size(), 10);
}

TEST_F(ModBusMasterTest, InvalidSlaveID) {
    // Slave ID must be 1-247
    std::vector<bool> coils;
    bool result = master->readCoils(248, 0, 10, coils);
    EXPECT_FALSE(result);
}

TEST_F(ModBusMasterTest, ExcessiveQuantity) {
    // Cannot read more than 2000 coils
    std::vector<bool> coils;
    bool result = master->readCoils(1, 0, 2001, coils);
    EXPECT_FALSE(result);
}
```

### Test Fixtures

```cpp
// Use test fixtures for complex scenarios
class ModBusCRCTest : public ::testing::Test {
protected:
    // Test data
    struct TestCase {
        std::vector<uint8_t> data;
        uint16_t expected_crc;
    };
    
    std::vector<TestCase> test_cases = {
        {{0x01, 0x03, 0x00, 0x00, 0x00, 0x0A}, 0x8408},
        // ... more test cases
    };
};
```

### Test Coverage Goals
- Overall: > 90%
- Critical functions: 100%
- New features: 100%

## Building and Testing

### Local Build

```bash
# Debug build with tests
mkdir -p build-debug
cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
make -j$(nproc)
ctest --output-on-failure -V

# Release build
mkdir -p build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Cross-Compilation

```bash
# ARM compilation (e.g., Raspberry Pi)
cmake .. -DCMAKE_TOOLCHAIN_FILE=cmake/armv7.cmake

# Windows cross-compilation
cmake .. -G "MinGW Makefiles"
```

### Code Coverage

```bash
cd build
cmake .. -DENABLE_COVERAGE=ON
make coverage
# Open coverage/index.html in browser
```

## Commit Message Format

Follow conventional commits:

```
type(scope): subject

body

footer
```

### Types
- **feat**: New feature
- **fix**: Bug fix
- **docs**: Documentation
- **style**: Code style (no logic change)
- **refactor**: Code refactoring
- **perf**: Performance improvement
- **test**: Add/update tests
- **build**: Build system changes
- **ci**: CI/CD changes

### Examples

```
feat(master): add batch read operations

Implement batch reading of multiple register
blocks in a single transaction. This reduces
communication overhead.

Closes #45
```

```
fix(crc): correct CRC16 polynomial

The CRC16 polynomial was incorrectly using
0xA002 instead of 0xA001, causing checksum
mismatches.
```

## Pull Request Process

1. **Before submitting:**
   - Run tests: `ctest -V`
   - Check coverage: `make coverage`
   - Static analysis: `cppcheck --enable=all src/`
   - Format code: `clang-format -i src/**/*.{hpp,cpp}`

2. **PR Description should include:**
   - What problem does it solve?
   - How does it solve it?
   - Breaking changes?
   - Performance impact?

3. **PR Template:**

```markdown
## Description
Describe your changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation

## Testing
Describe tests added/modified

## Performance Impact
Any performance implications?

## Checklist
- [ ] Tests pass
- [ ] Coverage >= 90%
- [ ] Code formatted
- [ ] Documentation updated
- [ ] No compiler warnings
- [ ] Static analysis passes
```

## Issue Reporting

### Bug Report Template

```markdown
## Description
Clear description of the bug

## Steps to Reproduce
1. Step 1
2. Step 2
3. Step 3

## Expected Behavior
What should happen

## Actual Behavior
What actually happens

## Environment
- OS: Ubuntu 22.04
- Compiler: GCC 11
- CMake: 3.22
- Version: 1.0.0

## Debug Info
```
error message here
```
```

### Feature Request Template

```markdown
## Description
What feature would you like?

## Use Case
Why is this feature needed?

## Proposed Solution
How would you implement it?

## Alternatives
Other approaches?

## Performance Considerations
Any impact on performance?
```

## Code Review Checklist

Reviewers should verify:

- [ ] Code follows style guide
- [ ] Functions have Doxygen docs
- [ ] Tests added for new code
- [ ] All tests pass
- [ ] Coverage maintained/improved
- [ ] No compiler warnings
- [ ] No static analysis issues
- [ ] No breaking changes
- [ ] No memory leaks
- [ ] Performance acceptable

## Documentation

### Doxygen Generation

```bash
# Generate documentation
doxygen Doxyfile

# View documentation
open docs/html/index.html
```

### Protocol Documentation

Update `docs/PROTOCOL.md` with:
- Protocol changes
- New function codes
- Compatibility notes

## Release Process

1. Update version in `CMakeLists.txt` and `include/modbus/Version.hpp`
2. Update CHANGELOG.md
3. Create git tag: `git tag v1.0.0`
4. Push tag: `git push origin v1.0.0`
5. GitHub Actions builds release binaries

## Questions?

- Check documentation in docs/
- Review example code in examples/
- Open an issue for discussion
- Read existing PRs for context

## License

By contributing, you agree that your contributions will be licensed under the Boost Software License 1.0.
