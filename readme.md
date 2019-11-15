# fea_flat_recurse
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
cmake .. -A x64 -DBUILD_TESTING=On && cmake --build . --config debug
bin\fea_flat_recurse_tests.exe

// Optionally
cmake --build . --target install
```

### Unixes
```
mkdir build && cd build
cmake .. -DBUILD_TESTING=On && cmake --build . --config debug
bin\fea_flat_recurse_tests.exe

// Optionally
cmake --build . --target install
```
