# SI library
[![GCC Build x64](https://github.com/TsvetelinKostadinv/strong_si_units/actions/workflows/gcc-build.yml/badge.svg)](https://github.com/TsvetelinKostadinv/strong_si_units/actions/workflows/gcc-build.yml)

### Main goals
Make a type-safe c++ library with high expressive power when it comes to physics units and the maths associated. Opt-in features and compatibility.

### Features
- [ ] Static big integer implementation
- [ ] Strong named types
- [ ] Expose the basic SI units and the governing constants
- [ ] Framework for creating composite types - strong naming of fractional types, e.g. `m/s`
- [ ] Procedural generation of literals for the units
- [ ] Elegant `std` integration

### Technical details
Ensured compilation with `msvc`, `gcc` and `clang` with most warnings turned on(and warnings as errors - as you should use). Ensured compilation with C++17(haven't tested with C++14)