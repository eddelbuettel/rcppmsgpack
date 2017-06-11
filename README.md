## RcppMsgPack [![Build Status](https://travis-ci.org/eddelbuettel/rcppmsgpack.svg)](https://travis-ci.org/eddelbuettel/rcppmsgpack) [![License](https://eddelbuettel.github.io/badges/GPL2+.svg)](http://www.gnu.org/licenses/gpl-2.0.html) 

MsgPack Headers for R

### About

This package provides [R](https://www.r-project.org) with
[MessagePack](http://msgpack.org/) header files.  [MessagePack](http://msgpack.org/) is an
efficient binary serialization format.  It lets you exchange data among multiple languages
like JSON. But it is faster and smaller. Small integers are encoded into a single byte,
and typical short strings require only one extra byte in addition to the strings
themselves.

[MessagePack](http://msgpack.org/) is used by Redis and many other projects.

To use this package, simply add it to the `LinkingTo:` field in the `DESCRIPTION` field of
your R package---and the R package infrastructure tools will then know how to set include
flags correctly on all architectures supported by R.

### Installation

The package is available via the [ghrr drat](http://ghrr.github.io/drat)
repository:

```{.r}
drat::addRepo("ghrr")
install.packages("RcppMsgPack")
```

### Status

The package currently includes the MsgPack headers. We may alter it to
depend on external msgpack headers once Debian contains release 1.4.0
or later in its main distribution (as opposed to the experimental one).

### See Also

See the [BH](http://dirk.eddelbuettel.com/code/bh.html) package for related (and also
header-only) libraries from [Boost](http://www.boost.org/), and the
[RcppRedis](http://dirk.eddelbuettel.com/code/rcppredis.html) package for simple yet performant
Redis support.

### Bug Reports

The [issue tracker](https://github.com/eddelbuettel/rcppmsgpack/issues)
can be used for bug reports or feature requests.

### Author 

Dirk Eddelbuettel

### License

The R package is provided under the GPL (version 2 or later).  The included
MsgPack library is released same license as Boost, the BSL-1.0
