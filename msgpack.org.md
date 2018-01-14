# RcppMsgPack
Convert to and from msgpack objects in R using the official msgpack-c API through Rcpp.

![flowchart](https://raw.githubusercontent.com/traversc/msgpack2R/master/vignettes/msgpack_flowchart.png "Conversion flowchart")
*A flowchart describing the conversion of R objects into msgpack objects and back.*

Msgpack EXT types are converted to raw vectors with EXT attributes containing the extension type.  The extension type must be an integer from 0 to 127.  

Maps are converted to data.frames with additional class "map".  Map objects in R contain key and value list columns and can be simplified to named lists or named vectors.  The helper function `msgpack_map` creates map objects that can be serialized into msgpack.  

For more information on msgpack types, see [here](https://github.com/msgpack/msgpack/blob/master/spec.md).  

### Installation:
1. `install.packages("RcppMsgPack")`

### Example:
See `tests/tests.r` for more examples.  
```
library(RcppMsgPack)
library(microbenchmark)

x <- as.list(1:1e6)
microbenchmark(xpk <- msgpack_pack(x), times=3)
microbenchmark(xu <- msgpack_unpack(xpk), times=3)
```