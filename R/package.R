#' RcppMsgPack
#' 
#' Package for converting to and from 'MsgPack' objects.  'MessagePack' is an efficient binary serialization format. It lets you exchange data among multiple languages like 'JSON'. But it is faster and smaller. Small integers are encoded into a single byte, and typical short strings require only one extra byte in addition to the strings themselves. This package provides headers from the 'msgpack-c' implementation for C and C++(11) for use by R, particularly 'Rcpp'. The included 'msgpack-c' headers are licensed under the Boost Software License (Version 1.0); the code added by this package as well the R integration are licensed under the GPL (>= 2). See the files 'COPYRIGHTS' and 'AUTHORS' for a full list of  copyright holders and contributors to 'msgpack-c'.  
#' 
#' @docType package
#' @author Travers Ching; Dirk Eddelbuettel; the authors and contributors of MsgPack
#' @import Rcpp
#' @importFrom Rcpp evalCpp
#' @useDynLib RcppMsgPack
#' @name RcppMsgPack
NULL  