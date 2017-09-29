#' Simplify 'MsgPack'
#' @description A helper function for simplifying a 'MsgPack' return object.
#' @param x Return object from msgpack_unpack.  
#' @return A simplified return object from msgpack_unpack.  Lists of all the same type are concatenated into an atomic vector.  Maps are simplified to named lists or named vectors as appropriate.  NULLs are converted to NAs if simplified to vector.  
#' @examples
#' x <- msgpack_format(1:10)
#' x_packed <- msgpack_pack(x)
#' x_unpacked <- msgpack_unpack(x_packed)
#' x_simplified <- msgpack_simplify(x_unpacked)
msgpack_simplify <- function(x) {
    if(!is.list(x)) return(x)

    if(class(x)[1] == "map") {
        key = msgpack_simplify(x[["key"]])
        value = msgpack_simplify(x[["value"]])
        if(class(key) == "character") {
            names(value) <- key
            return(value)
        } else {
            x[["key"]] <- key
            x[["value"]] <- value
            return(x)
        }
    }

    len <- length(x)
    xc <- sapply(x, function(xi) class(xi)[1])
    xcu <- unique(xc)

    if(len == 0) {
        return(x)
    } else if(all(xcu %in% c("logical", "NULL"))) {
        x[which(xc == "NULL")] <- NA
        return(unlist(x))
    } else if(all(xcu %in% c("character", "NULL"))) {
        x[which(xc == "NULL")] <- NA_character_
        return(unlist(x))
    } else if(all(xcu %in% c("integer", "NULL"))) {
        x[which(xc == "NULL")] <- NA_integer_
        return(unlist(x))
    } else if(all(xcu %in% c("numeric", "integer", "NULL"))) {
        x[which(xc == "NULL")] <- NA_real_
        return(unlist(x))
    } else {
        for(i in which(xc %in% c("list", "map"))) {
            x[[i]] <- msgpack_simplify(x[[i]])
        }
        return(x)
    }
}

#' @rdname msgpack_simplify
msgpackSimplify <- msgpack_simplify

#' Format data for 'MsgPack'
#' @description A helper function to format R data for input to 'MsgPack'.
#' @param x An r object.
#' @return A formatted R object to use as input to msgpack_pack.  Vectors are converted into Lists.  
#' @examples
#' x <- msgpack_format(1:10)
#' x_packed <- msgpack_pack(x)
#' x_unpacked <- msgpack_unpack(x_packed)
#' x_simplified <- msgpack_simplify(x_unpacked)
msgpack_format <- function(x) {
    xc <- class(x)[1]
    # print(xc)
    if(xc %in% c("logical", "integer", "numeric", "character")) {
        if(length(x) > 1) {
            xna <- which(is.na(x))
            x <- as.list(x)
            x[xna] <- list(NULL)
            return(x)
        } else {
            return(x)
        }
    } else if(xc == "raw") {
        return(x)
    } else if(xc == "list") {
        for(i in seq_along(x)) {
            x[[i]] <- msgpack_format(x[[i]])
        }
        return(x)
    } else if(xc == "map") {
        x <- msgpack_map(key=x[["key"]], value=x[["value"]])
        return(x)
    }
}

#' @rdname msgpack_format
msgpackFormat <- msgpack_format

#' 'MsgPack' Map
#' @description A helper function to create a map object for input to 'MsgPack'.
#' @param key A list or vector of keys (coerced to list).  Duplicate keys are fine (connects to std::multimap in C++).  
#' @param value A list or vector of values (coerced to list).  This should be the same length as key.
#' @return An data.frame also of class "map" that can be used as input to msgpack_pack.  
#' @examples
#' x <- msgpack_map(key=letters[1:10], value=1:10)
#' x_packed <- msgpack_pack(x)
#' x_unpacked <- msgpack_unpack(x_packed)
msgpack_map <- function(key, value) {
    for(a in attributes(key)) {
        attr(key, a) <- NULL
    }
    for(a in attributes(value)) {
        attr(value, a) <- NULL
    }    
    x <- list(key=as.list(key), value=as.list(value))
    attr(x, "row.names") <- seq_len(length(key))
    class(x) <- c("map", "data.frame")
    return(x)
}

#' @rdname msgpack_map
msgpackMap <- msgpack_map

#' 'MsgPack' Pack
#' @description Serialize any number of objects into a single message.  Unnamed List is converted into Array, Map/Data.frame and Named Lists are converted into Maps.  Integer, Double, Character, Raw vectors and NULL are converted into Int types (depending on size), Float types, String, Raw and Nil respectively.  Raw vectors with EXT attribute are converted into Extension types.  The EXT attribute should be an integer from 0 to 127.  
#' @param ... Any R objects that have corresponding msgpack types.  
#' @return A raw vector containing the message.  
#' @examples
#' x <- msgpack_format(1:10)
#' x_packed <- msgpack_pack(x)
#' x_unpacked <- msgpack_unpack(x_packed)
#' x_simplified <- msgpack_simplify(x_unpacked)
#' @seealso See examples/tests.r for more examples.  
msgpack_pack <- function(...) {
    obj_list <- list(...)
    if(length(obj_list) == 1) {
        return(c_pack(obj_list[[1]]))
    } else {
        class(obj_list) <- "msgpack_set"
        return(c_pack(obj_list))
    }
}

#' @rdname msgpack_pack
msgpackPack <- msgpack_pack

#' 'MsgPack' Unpack
#' @description De-serialize a 'MsgPack' message.  Array is converted into List. Map is converted into Map/Data.frame. Extension types are converted into raw vectors with EXT attribute.  Integers, Floats, Strings, Raw and Nil are converted into Integer, Float, Character, Raw and NULL respectively.  
#' @param message A raw vector containing the message.  
#' @param simplify Default false.  Should the return object be simplified?  This is generally faster and more memory efficient.  
#' @return The message pack object(s) converted into R types.  If more than one object exists in the message, a list of class "msgpack_set" containing the objects is returned.  
#' @examples
#' x <- msgpack_format(1:10)
#' x_packed <- msgpack_pack(x)
#' x_unpacked <- msgpack_unpack(x_packed)
#' x_simplified <- msgpack_simplify(x_unpacked)
#' @seealso See examples/tests.r for more examples.  
msgpack_unpack <- function(message, simplify=F) {
    c_unpack(message, simplify)
}

#' @rdname msgpack_unpack
msgpackUnpack <- msgpack_unpack

#' 'MsgPack' Timestamp
#' @description Encodes a timestamp to the 'MsgPack' specifications.  
#' @param posix A POSIXct or POSIXlt or anything that can be coerced to a numeric.  
#' @param seconds The number of seconds since 1970-01-01 00:00:00 UTC.  Can be negative.  Don't use seconds and nanoseconds if you use posix (and vice versa).  
#' @param nanoseconds The number of nanoseconds since 1970-01-01 00:00:00 UTC.  Must be less than 1,000,000,000 and greater than 0.  
#' @return A serialized timestamp that can be used as input to msgpack_pack.  Briefly, this is an extension type -1 that is variable length, depending on the desired range and precision.  
#' @examples
#' mt <- Sys.time()
#' attr(mt, "tzone") <- "UTC"
#' mp <- msgpack_pack(msgpack_timestamp_encode(mt))
#' mtu <- msgpack_timestamp_decode(msgpack_unpack(mp))
#' identical(mt, mtu)
msgpack_timestamp_encode <- function(posix=NULL, seconds=NULL, nanoseconds=NULL) {
    if(!is.null(posix)) {
        x <- as.numeric(posix)
        seconds <- floor(x)
        nanoseconds <- round((x-seconds)*1e9)
    }
    stopifnot(nanoseconds < 1e9 & nanoseconds >= 0)
    c_timestamp_encode(seconds, as.integer(nanoseconds))
}

#' @rdname msgpack_timestamp_encode
msgpackTimestampEncode <- msgpack_timestamp_encode

#' 'MsgPack' Timestamp
#' @description Decodes a timestamp from the 'MsgPack' extension specifications.  
#' @param x A raw vector with attriubte EXT = -1, following the 'MsgPack' timestamp specifications.  
#' @param posix Return a POSIXct object.  Otherwise, return a list with seconds and nanoseconds since 1970-01-01 00:00:00.  
#' @param tz If returning a POSIXct, set the timezone.  Note that this doesn't change the underlying value.  
#' @return A POSIXct or list.  
#' mt <- Sys.time()
#' attr(mt, "tzone") <- "UTC"
#' mp <- msgpack_pack(msgpack_timestamp_encode(mt))
#' mtu <- msgpack_timestamp_decode(msgpack_unpack(mp))
#' identical(mt, mtu)
msgpack_timestamp_decode <- function(x, posix=T, tz="UTC") {
    ret <- c_timestamp_decode(x)
    if(posix) {
        return(as.POSIXct(ret$seconds + ret$nanoseconds/1e9, origin = "1970-01-01", tz="UTC"))
    } else {
        return(ret)
    }
}

#' @rdname msgpack_timestamp_decode
msgpackTimestampDecode <- msgpack_timestamp_decode