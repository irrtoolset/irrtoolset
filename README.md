[![Build Status](https://travis-ci.org/irrtoolset/irrtoolset.svg)](https://travis-ci.org/irrtoolset/irrtoolset)

# IRRToolSet

### Overview

 The IRRToolSet is a set of tools to work with Internet routing policies. These
policies are stored in Internet Routing Registries (IRR) in the Routing Policy
Specification Language.

The goal of the IRRToolSet is to make routing information more convenient and
useful for network engineers, by providing tools for automated router
configuration, routing policy analysis, and on-going maintenance.

### Website

IRRToolSet's historical website can be found at http://irrtoolset.isc.org/.
Nowadays up-to-date code and releases can be found on GitHub,
http://github.com/irrtoolset/irrtoolset.

### Bugs, patches & help

The IRRToolSet community communicates via a mailing list, `irrtoolset@isc.org`.
To subscribe to the mailing list you can visit
`https://lists.isc.org/mailman/listinfo/irrtoolset` and use the form in the
section called "Subscribing to irrtoolset".

It is preferred for bug reports and patches to be reported on GitHub.

### Installation

Standard installation needs a GNU C++ compiler & libreadline. You will also
need Bison and Flex if you're checking out from git or if you need to change
the RPSL grammar.

Under Debian/Ubuntu systems, this should suffice:
```sh
$ apt-get install build-essential libreadline6-dev
```

IRRToolSet uses autotools, so installation is as simple as:

```sh
$ ./configure
$ make
$ make install
```

When working from git instead of a tarball, you will additionally need:
```sh
$ apt-get install autoconf automake libtool bison flex
```
and you should run
```sh
$ autoreconf -vfi
```
before running  `./configure`.

### Copyright and licensing

IRRToolSet has a long history and several parties hold copyright to various
parts of IRRToolSet. The code is licensed under a mixture of MIT & GNU GPLv2+
licenses. Please consult the `COPYING` file for more information.
