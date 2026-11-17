# IRRToolSet

### Summary

IRRToolSet is obsolete.  No further commits will be accepted and the project
is now formally archived.

Any people or organisations that still use IRRToolSet should migrate to
other tools for handling their IRR policy description requirements.

At the time of archiving, [bgpq4](https://github.com/bgp/bgpq4) is a good
choice for anyone needing to consume RPSL.

#### Why is IRRToolSet obsolete?

The IRR ecosystem was designed in the early 1990s around the principle that
external BGP interconnection relationships could be documented in publicly
visible Internet Routing Registries.  Once documented in a commonly
understood language, client libraries could parse this information to
recreate live router configurations, which would allow reliable
configuration automation.

IRRToolSet was written at the same time to handle this requirement.  At the
time it was revolutionary.

But complex inter-domain routing required complex inter-domain routing
policies, and it was quickly found that RPSL was too difficult to easily
express inter-domain routing policies of moderate complexity, while not
being expressive enough to document routing policies of greater complexity.

The codebase has several components including parsers, a regular expression
management engine, a normalisation engine, and router-specific output
generators.

The regular expression engine is complex and difficult to maintain.  The
normalisation engine is also complicated and breaks badly in a number of
situations.  Anecdotally, the need for the `-fno-tree-vrp` optimiser setting
in GCC is telling: the normalisation code is complex enough that it has
triggered bugs in the GCC optimiser since the 1990s.

The output generators are coded in C++, which is the wrong language for the
job.  Ideally text output should be handled using templating libraries, but
those didn't exist at the time that the code was written.  As a result,
extending IRRToolSet to support new router NOS grammars is difficult.

The design methodology behind rtconfig is too broken for anyone else to want
to port it to other router NOS grammars.  This could be fixed, except that
the RPSL and normalisation back-end libraries are too difficult to work with
from the point of view of creating other scripting language bindings so that
the parsed output could be injected into standard templating engines.  This
too could be fixed except that the RPSL language specification is impossible
to extend in a backwards-compatible way.  RPSL is also too complicated to
justify the effort of building a completely new parser from scratch and is
too limited to easily describe even relatively straightforward contemporary
inter-domain routing relationships.

Addressing these problems adequately would require a complete rethink of
what RPSL is trying to describe, followed by a ground-up redesign of the
language based on the perspective of 2020s routing policy management rather
than an early 1990s viewpoint that lacked 35 years of hindsight about
inter-domain routing.

Although various plans were floated at various stages to do just this, none
of them produced concrete results, and the IRRToolSet project languished.

The last substantial code development in IRRToolSet took place in 2002 when
the codebase was updated to support ipv6.  Since then, most commits have
been either refactoring or bugfixes, and by the time the project was
archived in 2026, there had been almost no commits for a decade.

The project is now dead.

### Website

IRRToolSet's website and code releases can be found at
http://github.com/irrtoolset/irrtoolset.

The code archive is now frozen and there are no plans for future
development.

### Copyright and licensing

IRRToolSet has a long history and several parties hold copyright to various
parts of IRRToolSet.  The code is licensed under a mixture of MIT, RIPE NCC
and GNU GPLv2+ licenses.  Please consult the `COPYING` file for more
information.
