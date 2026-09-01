# Diversinet C++ Library

This repository builds the core native `Diversinet` C++ library.

The Julia binding is owned by `Diversinet.jl`. In particular, the CxxWrap
bridge source and Julia wrapper module now live in:

```text
~/repos/Diversinet.jl/cpp/jlDiversinetInterface.cpp
~/repos/Diversinet.jl/src/DiversinetInterface.jl
```

## Build Requirements

Building Diversinet requires:

- A C++20-compatible compiler
- Meson
- Ninja
- Eigen3 headers
- Boost headers and the Boost.Random library

The optional command-line programs and test executable additionally require
the Boost.Filesystem and Boost.Program_options libraries. Meson discovers all
of these dependencies during configuration and stops with an error if a
required dependency is unavailable.

For a minimal library-only build, disable the optional programs and tests:

```sh
meson setup builddir . \
    --buildtype=release \
    -Dprograms=false \
    -Dtests=false
meson compile -C builddir
```

## Build

Build the core library with Meson:

```sh
meson setup builddir .
meson compile -C builddir
```

Command-line programs and tests are disabled by default. Enable them explicitly
for a development build:

```sh
meson setup builddir . -Dprograms=true -Dtests=true
meson compile -C builddir
meson test -C builddir --print-errorlogs
```

The local development build produces the shared library at:

```text
builddir/src/libdiversinet.dylib
```

On Linux, the extension will be `.so` instead of `.dylib`.

If a previous Meson configuration needs to be reset:

```sh
meson setup builddir . --wipe
meson compile -C builddir
```

## Using From `Diversinet.jl`

After building this C++ library, configure the Julia package with an explicit
path to this repository:

```sh
cd ~/repos/Diversinet.jl
DIVERSINET_CPP_ROOT=~/repos/Diversinet julia --project=. -e 'import Pkg; Pkg.build("Diversinet")'
```

`Diversinet.jl` uses `DIVERSINET_CPP_ROOT` to find:

```text
~/repos/Diversinet/builddir/src/libdiversinet.dylib
```

and then builds its own CxxWrap bridge dylib.

Alternatively, provide an already-built core library explicitly:

```sh
cd ~/repos/Diversinet.jl
DIVERSINET_CPP_ROOT=~/repos/Diversinet \
DIVERSINET_CORE_LIB=/absolute/path/to/libdiversinet.dylib \
julia --project=. -e 'import Pkg; Pkg.build("Diversinet")'
```

`DIVERSINET_CPP_ROOT` is still required so `Diversinet.jl` can find the C++
headers.

## Using an Installed Diversinet Library

The installed public API is available as:

```cpp
#include <Diversinet/DiversinetInterface.h>
```

A consuming Meson project can discover Diversinet through `pkg-config`.
Because the public header currently uses `boost::shared_ptr`, Boost is also a
public compile dependency:

```meson
project('diversinet-consumer', 'cpp', default_options : ['cpp_std=c++20'])

diversinet_dep = dependency('diversinet')
boost_dep = dependency('boost')

executable(
  'diversinet-consumer',
  'main.cpp',
  dependencies : [diversinet_dep, boost_dep],
)
```

If Diversinet is installed under a nonstandard prefix, add its pkg-config
directory to `PKG_CONFIG_PATH` before configuring the consuming project.
