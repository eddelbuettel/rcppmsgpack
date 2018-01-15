\name{NEWS}
\title{News for Package \pkg{RcppMsgPack}}
\newcommand{\ghpr}{\href{https://github.com/eddelbuettel/rcppmsgpack/pull/#1}{##1}} 
\newcommand{\ghit}{\href{https://github.com/eddelbuettel/rcppmsgpack/issues/#1}{##1}} 

\section{Changes in version 0.2.1 (2018-01-15)}{
  \itemize{
    \item Some corrections and update to DESCRIPTION, README.md,
    msgpack.org.md and vignette (\ghpr{6}).
    \item Update to \code{c_pack.cpp} and tests (\ghpr{7}).
    \item More efficient packing of vectors (\ghpr{8}).
    \item Support for timestamps and NAs (\ghpr{9}).
    \item Conditional use of \pkg{microbenchmark} in \code{tests/} 
    as required for Suggests: package [CRAN request] (\ghpr{10}).
    \item Minor polish to tests relaxing comparison of timestamp, and
    avoiding a few \code{g++} warnings (\ghpr{12} addressing \ghit{11}).
  }
}

\section{Changes in version 0.2.0 (2017-09-07)}{
  \itemize{
    \item Added support for building on Windows
    \item Upgraded to MsgPack 2.1.5 (\ghpr{3})
    \item New R functions to manipulate MsgPack objects: msgpack_format,
    msgpack_map, msgpack_pack, msgpack_simplify, mgspack_unpack (\ghpr{4})
    \item New R functions also available as msgpackFormat, msgpackMap,
    msgpackPack, msgpackSimplify, mgspackUnpack (\ghpr{4})
    \item New vignette and new tests (\ghpr{4}).
    \item Some post-processing and polishing (\ghpr{5}).
  }
}

\section{Changes in version 0.1.1 (2017-06-12)}{
  \itemize{
    \item Updated included MsgPack headers to version 1.4.2
    \item Updated RcppExports, README.md, DESCRIPTION.
  }
}

\section{Changes in version 0.1.0 (2016-04-24)}{
  \itemize{
    \item First Version, released only to drat repository
  }
}

