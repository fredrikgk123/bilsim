# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-src")
  file(MAKE_DIRECTORY "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-src")
endif()
file(MAKE_DIRECTORY
  "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-build"
  "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-subbuild/threepp-populate-prefix"
  "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-subbuild/threepp-populate-prefix/tmp"
  "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-subbuild/threepp-populate-prefix/src/threepp-populate-stamp"
  "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-subbuild/threepp-populate-prefix/src"
  "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-subbuild/threepp-populate-prefix/src/threepp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-subbuild/threepp-populate-prefix/src/threepp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/fredrikkarlsaune/Desktop/dev/bilsim/cmake-build-debug-cmake/_deps/threepp-subbuild/threepp-populate-prefix/src/threepp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
