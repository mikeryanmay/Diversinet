# Diversinet

[![C++ CI](https://github.com/mikeryanmay/Diversinet/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/mikeryanmay/Diversinet/actions/workflows/ci.yml?query=branch%3Amain)

Diversinet is a C++20 library for likelihood calculation and simulation on
phylogenetic networks. This repository is the canonical source for the native
library and its public C++ API.

The related repositories have separate roles:

- [Diversinet.jl](https://github.com/mikeryanmay/Diversinet.jl) provides the
  Julia API.
- [Diversinet_jll](https://github.com/mikeryanmay/Diversinet_jll) distributes
  prebuilt libraries for Julia users.
- [DiversinetJLLBuilder](https://github.com/mikeryanmay/DiversinetJLLBuilder)
  contains the BinaryBuilder recipe.
- [DiversinetRegistry](https://github.com/mikeryanmay/DiversinetRegistry)
  provides prerelease Julia package discovery.
- [DiversinetDocker](https://github.com/mikeryanmay/DiversinetDocker)
  provides a ready-to-run Docker image containing Julia and Diversinet.

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
meson setup builddir . --buildtype=release -Dprograms=false -Dtests=false
meson compile -C builddir
```

## Build from source

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

## Install the C++ library

Choose an installation prefix when configuring Meson, then install with
Ninja through Meson:

```sh
meson setup builddir . --prefix=/absolute/install/prefix --buildtype=release -Dprograms=false -Dtests=false
meson compile -C builddir
meson install -C builddir
```

The installation contains:

```text
/absolute/install/prefix/include/Diversinet/DiversinetInterface.h
/absolute/install/prefix/lib/libdiversinet.*
/absolute/install/prefix/lib/pkgconfig/diversinet.pc
```

Julia users do not need to build or install this repository. Follow the
[Diversinet.jl installation instructions](https://github.com/mikeryanmay/Diversinet.jl#installation)
to obtain the prebuilt native library through `Diversinet_jll`.

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
The complete minimal consumer is in `examples/installed_consumer`.

## Optional programs

Setting `-Dprograms=true` builds `calcDiversinet` and `simDiversinet`. These
are development-oriented command-line front ends; the supported reusable
interface is the installed C++ library.

## License

Diversinet is licensed under the GNU General Public License, version 3 or, at
your option, any later version (`GPL-3.0-or-later`). See [LICENSE](LICENSE).

Existing copyright and license notices in individual source files remain in
effect.
