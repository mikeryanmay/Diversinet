# Diversinet C++ Library

This repository builds the core native `Diversinet` C++ library.

The Julia binding is owned by `Diversinet.jl`. In particular, the CxxWrap
bridge source and Julia wrapper module now live in:

```text
~/repos/Diversinet/cpp/jlDiversinetInterface.cpp
~/repos/Diversinet/src/DiversinetInterface.jl
```

## Build

Build the core library with Meson:

```sh
meson setup builddir .
meson compile -C builddir
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
cd ~/repos/Diversinet
DIVERSINET_CPP_ROOT=~/repos/phyloploid_lib julia --project=. -e 'import Pkg; Pkg.build("Diversinet")'
```

`Diversinet.jl` uses `DIVERSINET_CPP_ROOT` to find:

```text
~/repos/phyloploid_lib/builddir/src/libdiversinet.dylib
```

and then builds its own CxxWrap bridge dylib.

Alternatively, provide an already-built core library explicitly:

```sh
cd ~/repos/Diversinet
DIVERSINET_CPP_ROOT=~/repos/phyloploid_lib \
DIVERSINET_CORE_LIB=/absolute/path/to/libdiversinet.dylib \
julia --project=. -e 'import Pkg; Pkg.build("Diversinet")'
```

`DIVERSINET_CPP_ROOT` is still required so `Diversinet.jl` can find the C++
headers.
