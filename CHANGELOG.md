Changelog
==============

## Version 14.30 (2025/06)
- Added CMake options `ALLOW_SHIPPED_CLN` and `ALLOW_SHIPPED_GINAC` and CMake flag `HAVE_CLN`.
- Improved Ginac CMake support.
- Fixed various compiler warnings.

## Version 14.29 (2025/05)
- Use C++20 and require CMake >= 3.22.
- Significant improvement of CMake build process.
- Support for building carl as a subproject (not a toplevel project).
- Removed unused tests and CMake option `COMPARE_WITH_Z3`.
- Improved CI.

## Version 14.28 (2024/07)
- Support for Apple Silicon
- Improved performance for getHash() of FactorizedPolynomial.
- Updated 3rdparty dependencies.
- Improved CI.

## Version 14.27 (2024/02)
- Fixed CMake support for CLN

## Version 14.26 (2023/12)
- Interval API extended
- Some code cleanup
- Towards support for C++20

## Version  14.25 (2023/05)
- CI support.
- Set default CMake options used by Storm.
- Added Dockerfile.

## Version  14.24 (2023/04)
- Extended README.
- Stripping and support for forward declarations.
- Removed unnecessary code parts.

## Version  14.23 (2023/03)
- Forked the original master14 branch of ths-rwth/carl into a separate project.
