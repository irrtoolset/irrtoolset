# IRRToolSet

### Overview

The IRRToolSet is a set of tools to work with Internet routing policies. These
policies are stored in Internet Routing Registries (IRR) in the Routing Policy
Specification Language.

The goal of the IRRToolSet is to make routing information more convenient and
useful for network engineers, by providing tools for automated router
configuration, routing policy analysis, and on-going maintenance.

### Status

IRRToolSet is not actively maintained. If you're looking for a fast, efficient, well-maintained IRRDB client, BGPQ4 is an excellent choice: https://github.com/bgp/bgpq4.

### Website

IRRToolSet's website, up-to-date code and releases can be found at
http://github.com/irrtoolset/irrtoolset.

### Bugs, patches & help

The IRRToolSet community communicates via a mailing list, `irrtoolset@isc.org`.
To subscribe to the mailing list you can visit
`https://lists.isc.org/mailman/listinfo/irrtoolset` and use the form in the
section called "Subscribing to irrtoolset".

It is preferred for bug reports and patches to be reported on GitHub.

### Installation

Standard installation needs a C++ compiler, CMake (3.16 or newer), bison,
flex and, optionally, libreadline. `make bootstrap` installs them on
Debian/Ubuntu, Alpine, Fedora/RHEL, Arch, openSUSE, FreeBSD and macOS (Homebrew).

```sh
$ make bootstrap
$ make build
$ make install
```

To pass options to CMake, use the `CMAKE_ARGS` variable. For example:

```bash
$ make PREFIX=/usr CMAKE_ARGS=-DENABLE_READLINE=OFF
```

### Copyright and licensing

IRRToolSet has a long history and several parties hold copyright to various
parts of IRRToolSet.  The code is licensed under a mixture of MIT, RIPE NCC
and GNU GPLv2+ licenses.  Please consult the `COPYING` file for more
information.
