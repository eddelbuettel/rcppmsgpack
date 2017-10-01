# Tests for testing out the functionality of the package, to make sure it isn't broken

# some references

# //https://github.com/msgpack/msgpack-c/blob/401460b7d99e51adc06194ceb458934b359d2139/include/msgpack/v1/adaptor/boost/msgpack_variant.hpp
# //https://stackoverflow.com/questions/44725299/messagepack-c-how-to-iterate-through-an-unknown-data-structure
# //https://stackoverflow.com/questions/37665361/how-to-determine-the-class-of-object-stored-in-sexp-in-rcpp
# //https://stackoverflow.com/questions/12954852/booststatic-visitor-with-multiple-arguments
# //https://stackoverflow.com/questions/25172419/how-can-i-get-the-sexptype-of-an-sexp-value
# //https://github.com/wch/r-source/blob/48536f1756a88830076023db9566fbb2c1dbb29b/src/include/Rinternals.h#L1178-L1214
# //https://stackoverflow.com/questions/16131462/how-to-use-boost-library-in-c-with-rcpp


library(RcppMsgPack)
# library(Rcpp)
# sourceCpp("src/msgpack_unpack.cpp")
# sourceCpp("src/msgpack_pack.cpp")
# source("R/functions.r")

# stopifnot <- function(...) cat(..., "\n")

# Test atomic types

# integer
xpk <- msgpack_pack(1L)
stopifnot(identical(msgpack_unpack(xpk), 1L))

# double
xpk <- msgpack_pack(1.54)
stopifnot(identical(msgpack_unpack(xpk), 1.54))

# string
xpk <- msgpack_pack("sdfsdf")
stopifnot(identical(msgpack_unpack(xpk), "sdfsdf"))

# raw
xpk <- msgpack_pack(as.raw(c(0x28, 0x4F)))
stopifnot(identical(msgpack_unpack(xpk), as.raw(c(0x28, 0x4F))))

# boolean
xpk <- msgpack_pack(T)
stopifnot(msgpack_unpack(xpk))

# nil
xpk <- msgpack_pack(NULL)
stopifnot(is.null(msgpack_unpack(xpk)))

# ext
x <- as.raw(c(0x28, 0x4F))
attr(x, "EXT") <- 1L
xpk <- msgpack_pack(x)
stopifnot(identical(msgpack_unpack(xpk), x))


if (Sys.info()[['sysname']] != "Windows") {
    ## unicode or something characters - note this doesn't always work if you copy/paste into a terminal because of how terminals encode text, but if you source this file it works
    x <- list('图书，通常在狭义上的理解是带有文字和图像的纸张的集合。书通常由墨水、纸张、羊皮纸或者其他材料固定在书脊上组成。组成书的一张纸称为一张，一张的一面称为一页。但随着科学技术的发展，狭义图书的概念也在扩展，制作书的材料也在改变，如电子格式的电子书。从广义理解的图书，则是一切传播讯息的媒介。书也指文学作品或者其中的一部分。在图书馆信息学中，书被称为专著，以区别于杂志、学术期刊、报纸等连载期刊。所有的书面作品（包括图书）的主体是文学。在小说和一些类型（如传记）中，书可能还要分成卷。对书特别喜爱的人被称为爱书者或藏书家，更随意的称呼是书虫或者书呆子。买书的地方叫书店，图书馆则是可以借阅书籍的地方。2010年，谷歌公司估计，从印刷术发明至今，大概出版了一亿三千万本不同书名的书籍。[1]')
    xpk <- msgpack_pack(x)
    xu <- msgpack_unpack(xpk)
    stopifnot(identical(x, xu))
}

# Complex nested object with lists and map
x <- as.raw(c(0x28, 0x4F))
attr(x, "EXT") <- 1L
xmap <- msgpack_map(key=letters[1:10], value=1:10)
xmap$value[[3]] <- list(NULL)
xmap$value[[4]] <- as.list(1:10)
xmap$value[[4]][[3]] <- xmap
xmap$value[[5]] <- x
y <- 1:10
names(y) <- letters[1:10]
x <- list(1:10, y, "a", list(3,raw(4)), xmap)
x <- msgpack_format(x)
xpk <- msgpack_pack(x)
xu <- msgpack_unpack(xpk)

xs <- msgpack_simplify(x)
xus <- msgpack_simplify(xu)

stopifnot(identical(xs, xus))

# named list can be used directly as input - should come out to a map, simplify to get a named vector
x <- list(a=1L, b=2L)
xpk <- msgpack_pack(x)
stopifnot(identical(msgpack_simplify(msgpack_unpack(xpk)),c(a=1L, b=2L)))

# multiple objects
xpk <- msgpack_pack(1,2,3,5,"a", msgpack_format(1:10))
xu <- msgpack_unpack(xpk)
stopifnot(identical(msgpack_simplify(xu[[6]]), 1:10))

# speed test
require(microbenchmark)
x <- as.list(1:1e6)
print(microbenchmark(xpk <- msgpack_pack(x), times=10)) # 500 ms
print(microbenchmark(xu <- msgpack_unpack(xpk), times=10)) # 150 ms
stopifnot(identical(xu, x))

# vector input
x <- 1:1e7
print(microbenchmark(xpk2 <- msgpack_pack(x), times=10)) # 50 ms
print(microbenchmark(xu <- msgpack_unpack(xpk2, simplify=T), times=10)) # 50 ms
stopifnot(identical(xu, x))

# packed list and vector should be identical
# stopifnot(identical(msgpack_simplify(xpk), xpk2))

# vector with NAs
x <- c(1:3,NA,5)
xpk <- msgpack_pack(x)
stopifnot(identical(msgpack_simplify(msgpack_unpack(xpk)),x))
stopifnot(identical(msgpack_unpack(xpk, simplify=T),x))

# named vector is serialized to map
x <- c(1:4); names(x) <- c("z",letters[1:3])
xpk <- msgpack_pack(x)
stopifnot(identical(msgpack_simplify(msgpack_unpack(xpk)),x))
stopifnot(identical(msgpack_unpack(xpk, simplify=T),x))

# array length zero
x <- list()
xpk <- msgpack_pack(x)
stopifnot(identical(msgpack_unpack(xpk),x))

# map length zero
x <- msgpack_map(key=list(), value=list())
xpk <- msgpack_pack(x)
stopifnot(identical(msgpack_unpack(xpk),x))

# special numeric values
x <- c(NA_real_, NaN, -NaN, Inf, -Inf, .Machine$double.xmax, .Machine$double.xmin, -0., 0.)
xpk <- msgpack_pack(x)
xu <- msgpack_unpack(xpk, simplify=T)
stopifnot(identical(x, xu, num.eq=F, single.NA=F))

x <- c(.Machine$integer.max,  NA_integer_)
xpk <- msgpack_pack(x)
xu <- msgpack_unpack(xpk, simplify=T)
stopifnot(identical(x, xu))

#timestamps
mt <- Sys.time()
attr(mt, "tzone") <- "UTC"
mp <- msgpack_pack(msgpack_timestamp_encode(mt))
mtu <- msgpack_timestamp_decode(msgpack_unpack(mp))
stopifnot(identical(mt, mtu))

secs <- round(as.numeric(mt))
mp <- msgpack_pack(msgpack_timestamp_encode(seconds=secs, nanoseconds=0))
mtu <- msgpack_timestamp_decode(msgpack_unpack(mp), posix=F)
stopifnot(identical(secs, mtu$seconds))

secs <- -2^50
nanoseconds <- 999999999L
mp <- msgpack_pack(msgpack_timestamp_encode(seconds=secs, nanoseconds=nanoseconds))
mtu <- msgpack_timestamp_decode(msgpack_unpack(mp), posix=F)
stopifnot(identical(secs, mtu$seconds))
stopifnot(identical(nanoseconds, mtu$nanoseconds))

# memory profiling using profvis
# profvis({x <- msgpack_pack(1:1e7)}, torture=0)
# profvis({x <- msgpack_unpack(x, simplify=T)}, torture=0)

