# fea_flat_recurse
[![Build Status](https://travis-ci.org/p-groarke/fea_flat_recurse.svg?branch=master)](https://travis-ci.org/p-groarke/fea_flat_recurse)
[![Build status](https://ci.appveyor.com/api/projects/status/kw99c4io4f50n6en/branch/master?svg=true)](https://ci.appveyor.com/project/p-groarke/fea-flat-recurse/branch/master)

Flat breadth and depth graph traversal/gathering functions.

## Examples

```c++

```

## Build
`fea_flat_recurse` is a single header with no dependencies other than the stl.

The unit tests depend on gtest. They are not built by default. Use conan to install the dependencies when running the test suite.

Install recent conan, cmake and compiler.

### Windows
```
mkdir build && cd build
cmake .. -A x64 -DFEAFLATRECURSE_TESTS=On && cmake --build . --config debug
bin\fea_flat_recurse_tests.exe

// Optionally
cmake --build . --target install
```

### Unixes
```
mkdir build && cd build
cmake .. -DFEAFLATRECURSE_TESTS=On && cmake --build . --config debug
bin\fea_flat_recurse_tests.exe

// Optionally
cmake --build . --target install
```
