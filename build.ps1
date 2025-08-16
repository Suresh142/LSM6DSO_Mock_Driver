param([switch]$Tests, [switch]$Run)

$ErrorActionPreference = 'Stop'
$gcc = if ($env:CC) { $env:CC } else { 'gcc' }

# ensure build dir
$build = Join-Path $PSScriptRoot 'build'
if (-not (Test-Path $build)) { New-Item -ItemType Directory -Path $build | Out-Null }

# flags
$inc   = @('-I','include','-I','tests/vendor/unity')
$defs  = @('-DIMU_DRIVER_NO_DEFAULT_I2C')
$copts = @('-std=c11','-O2','-Wall','-Wextra','-Wno-unused-parameter')

# compile & link unit tests
& $gcc $copts $inc $defs `
  'src/imu_driver.c','src/mock_i2c.c','tests/vendor/unity/unity.c','tests/test_imu.c' `
  -o (Join-Path $build 'unit_tests.exe')

# compile & link demo app
& $gcc $copts $inc $defs `
  'src/imu_driver.c','src/mock_i2c.c','src/main.c' `
  -o (Join-Path $build 'app.exe')

if ($Tests) { & (Join-Path $build 'unit_tests.exe') }
if ($Run)   { & (Join-Path $build 'app.exe') }