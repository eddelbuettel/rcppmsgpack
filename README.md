## RcppMsgPack [![Build Status](https://travis-ci.org/eddelbuettel/rcppmsgpack.svg)](https://travis-ci.org/eddelbuettel/rcppmsgpack) [![License](https://img.shields.io/badge/license-BSL--1.0-brightgreen.svg?style=flat)](http://www.boost.org/users/license.html) [![CRAN](http://www.r-pkg.org/badges/version/RcppMsgPack)](http://cran.rstudio.com/package=RcppMsgPack) [![Downloads](http://cranlogs.r-pkg.org/badges/RcppMsgPack?color=brightgreen)](http://cran.rstudio.com/package=RcppMsgPack)

MsgPack Headers for R

### About

This package provides [R](http://www.r-project.org) with
[MessagePack](http://msgpack.org/) header files.  [MessagePack](http://msgpack.org/) is an
efficient binary serialization format.  It lets you exchange data among multiple languages
like JSON. But it is faster and smaller. Small integers are encoded into a single byte,
and typical short strings require only one extra byte in addition to the strings
themselves.

[MessagePack](http://msgpack.org/) is used by [Redis](http://redis.io) and many other projects.

To use this package, simply add it to the `LinkingTo:` field in the `DESCRIPTION` field of
your R package---and the R package infrastructure tools will then know how to set include
flags correctly on all architectures supported by R.


### See Also

See the [BH](http://dirk.eddelbuettel.com/code/bh.html) package for related (and also
header-only) libraries from [Boost](http://www.boost.org/), and the
[RcppRedis](http://dirk.eddelbuettel.com/code/rcppredis.html) package for simple yet performant
Redis support.

The [issue tracker](https://github.com/eddelbuettel/rcppmsgpack/issues)
can be used for bug reports or feature requests.

### Author 

Dirk Eddelbuettel

### License

This package is provided under the same license as Boost, the BSL-1.0
