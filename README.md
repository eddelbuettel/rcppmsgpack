## RcppMsgPack: MsgPack Headers for R and interface functions

[![Build Status](https://travis-ci.org/eddelbuettel/rcppmsgpack.svg)](https://travis-ci.org/eddelbuettel/rcppmsgpack) 
[![CI](https://github.com/eddelbuettel/rcppmsgpack/workflows/ci/badge.svg)](https://github.com/eddelbuettel/rcppmsgpack/actions?query=workflow%3Aci)
[![License](https://eddelbuettel.github.io/badges/GPL2+.svg)](http://www.gnu.org/licenses/gpl-2.0.html) 
[![CRAN](http://www.r-pkg.org/badges/version/RcppMsgPack)](https://cran.r-project.org/package=RcppMsgPack) 
[![Dependencies](https://tinyverse.netlify.com/badge/RcppGetconf)](https://cran.r-project.org/package=RcppGetconf) 
[![Downloads](http://cranlogs.r-pkg.org/badges/RcppMsgPack?color=brightgreen)](http://www.r-pkg.org/pkg/RcppMsgPack)
[![Last Commit](https://img.shields.io/github/last-commit/eddelbuettel/rcppmsgpack)](https://github.com/eddelbuettel/rcppmsgpack)
[![RJournal](https://img.shields.io/badge/RJournal-10.32614%2FRJ--2018--068-brightgreen)](https://doi.org/10.32614/RJ-2018-068)

### About

This package provides [R](https://www.r-project.org) with both the
[MessagePack](http://msgpack.org/) (or MsgPack as a shorthand) header files, and the ability to
access, create and alter [MessagePack](http://msgpack.org/) objects directly from
[R](https://www.r-project.org).

[MessagePack](http://msgpack.org/) is an efficient binary serialization format.  It lets you
exchange data among multiple languages like JSON. But it is faster and smaller.  Small integers
are encoded into a single byte, and typical short strings require only one extra byte in
addition to the strings themselves.  [MessagePack](http://msgpack.org/) is used by Redis and
many other projects.

The [R Journal paper](https://doi.org/10.32614/RJ-2018-068) describes both the `RcppMsgPack`
package and [MessagePack](http://msgpack.org/).

### Usage
#### C++ headers

To use the headers from this package, simply add it to the `LinkingTo:` field in the
`DESCRIPTION` field of your R package---and the R package infrastructure tools will then know
how to set include flags correctly on all architectures supported by R.

#### Interface functions

The functions `msgpack_pack` and `msgpack_unpack` allow you to serialize and de-serialize R
objects respectively.  `msgpack_format` is a helper function to properly format R objects for
input.  `msgpack_simplify` is a helper function to simplify output from MsgPack conversion.

MsgPack EXT types are converted to raw vectors with EXT attributes containing the extension
type.  The extension type must be an integer from 0 to 127.  MsgPack Timestamps are an EXT with
type -1.  Timestamps can be encoded and decoded from `POSIXct` objects in R to MsgPack format
with `msgpack_timestamp_encode` and `msgpack_timestamp_decode`.

Msgpack Maps are converted to data.frames with additional class "map".  Map objects in R
contain key and value list columns and can be simplified to named lists or named vectors.  The
helper function `msgpack_map` creates map objects that can be serialized into msgpack.

![flowchart](https://github.com/eddelbuettel/rcppmsgpack/raw/master/vignettes/msgpack_flowchart.png
"Conversion flowchart") *A flowchart describing the conversion of R objects into msgpack
objects and back.*

For more information on msgpack types, see [here](https://github.com/msgpack/msgpack/blob/master/spec.md).  

#### Example:
```
x <- as.list(1:1e6)
x_packed <- msgpack_pack(x)
x_unpacked <- msgpack_unpack(x_packed)
```

### Installation

The package is on [CRAN](https://cran.r-project.org) and can be installed via a standard

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

Dirk Eddelbuettel and [Travers Ching](https://github.com/traversc).

### Acknowledgements

Special thanks to [Xun Zhu](https://github.com/w9).

### License

The R package is provided under the GPL (version 2 or later).  The included
MsgPack library is released under the same license as Boost, the BSL-1.0.
