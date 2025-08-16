# IMU Mock Driver (C)

This repository contains a small IMU driver and a mock I2C layer so you can build and unit-test everything on a regular PC (no hardware needed).  
[Unity](https://github.com/ThrowTheSwitch/Unity) is vendored for unit testing.

---

## Folder structure

```
.
├── include/                 # Public headers
│   ├── imu_driver.h
│   └── mock_i2c.h
├── src/                     # Library and demo sources
│   ├── imu_driver.c
│   ├── mock_i2c.c           # Mocked I2C backend used in tests/demo
│   └── main.c               # Tiny demo that prints one sample
├── tests/
│   ├── test_imu.c           # Unit tests (Unity)
│   └── vendor/unity/        # Vendored Unity framework
│       ├── unity.c
│       ├── unity.h
│       └── unity_internals.h
├── Makefile                 # Build rules (bash & PowerShell with mingw32-make)
├── build.ps1                # PowerShell build script (no make needed)
└── .gitignore
```

---

## Prerequisites

- **Linux / macOS**: GCC or Clang
- **Windows**:
  - Option A (recommended): [MSYS2](https://www.msys2.org/) with MinGW-w64, then use `mingw32-make`.
  - Option B: [MinGW-w64 standalone](https://www.mingw-w64.org/) + add `C:\mingw64\bin` to PATH.
  - Option C: Use the included `build.ps1` script directly in PowerShell.

No external libraries are required; Unity is vendored.

---

## Build instructions

### Linux / macOS (bash)

```bash
# Build both app and tests
make

# Run unit tests
make test

# Run demo app
make run
```

### Windows (PowerShell)

#### Using mingw32-make
```powershell
# Build everything
mingw32-make CC=gcc

# Run unit tests
mingw32-make CC=gcc test

# Run demo app
mingw32-make CC=gcc run
```

#### Using build.ps1 (no make required)
```powershell
# Build everything
.\build.ps1

# Build + run unit tests
.\build.ps1 -Tests

# Build + run demo app
.\build.ps1 -Run
```

---

## Expected output

When running tests:

```
tests/test_imu.c:64:test_initialization_sequence:PASS
tests/test_imu.c:65:test_scaling:PASS
tests/test_imu.c:66:test_reading_and_filtering:PASS

-----------------------
3 Tests 0 Failures 0 Ignored
OK
```

---

## Notes

- The driver provides simple init and read helpers for an **LSM6DSO-style IMU** and converts raw sensor values using scale factors defined in `imu_driver.h`.
- On desktop builds, the **mock I2C** is enabled via `IMU_DRIVER_NO_DEFAULT_I2C` (set automatically by the Makefile and build.ps1).
- For real hardware integration, implement the weak `i2c_read_reg` / `i2c_write_reg` functions in `imu_driver.c` (or remove the mock define) to call your platform’s I2C API.
