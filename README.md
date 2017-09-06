## RcppMsgPack [![Build Status](https://travis-ci.org/eddelbuettel/rcppmsgpack.svg)](https://travis-ci.org/eddelbuettel/rcppmsgpack) [![License](https://eddelbuettel.github.io/badges/GPL2+.svg)](http://www.gnu.org/licenses/gpl-2.0.html) [![CRAN](http://www.r-pkg.org/badges/version/RcppMsgPack)](https://cran.r-project.org/package=RcppMsgPack) [![Downloads](http://cranlogs.r-pkg.org/badges/RcppMsgPack?color=brightgreen)](http://www.r-pkg.org/pkg/RcppMsgPack)

MsgPack Headers for R

### About

This package provides [R](https://www.r-project.org) with both the
[MessagePack](http://msgpack.org/) (or MsgPack as a shorthand) header files, and the 
ability to access, create and alter [MessagePack](http://msgpack.org/) objects directly from
[R](https://www.r-project.org). 

[MessagePack](http://msgpack.org/) is an efficient binary serialization format.  
It lets you exchange data among multiple languages like JSON. But it is faster and smaller. 
Small integers are encoded into a single byte, and typical short strings require only one 
extra byte in addition to the strings themselves.  [MessagePack](http://msgpack.org/) is 
used by Redis and many other projects.

To use the headers from this package, simply add it to the `LinkingTo:` field in the 
`DESCRIPTION` field of your R package---and the R package infrastructure tools will then 
know how to set include flags correctly on all architectures supported by R.

### Installation

The package is on [CRAN](https://cran.r-project.org) and can be installed via
a standard

```r
install.packages("RcppMsgPack")
```

Pre-releases have been available via the [ghrr drat](http://ghrr.github.io/drat)
repository:

```{.r}
drat::addRepo("ghrr")
install.packages("RcppMsgPack")
```

### Status

The package currently includes the MessagePack headers version 2.1.5. 

### See Also

See the [BH](http://dirk.eddelbuettel.com/code/bh.html) package for related (and also
header-only) libraries from [Boost](http://www.boost.org/), and the
[RcppRedis](http://dirk.eddelbuettel.com/code/rcppredis.html) package for simple yet performant
Redis support.

### Bug Reports

The [issue tracker](https://github.com/eddelbuettel/rcppmsgpack/issues)
can be used for bug reports or feature requests.

### Author 

Dirk Eddelbuettel and Travers Ching

### License

The R package is provided under the GPL (version 2 or later).  The included
MsgPack library is released under the same license as Boost, the BSL-1.0.
