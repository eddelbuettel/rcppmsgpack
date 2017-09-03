// This example is based on msgpack-c/example/cpp03/simple.cpp 

#include <msgpack.hpp>
#undef ERROR

#include <Rcpp.h>

enum my_enum {
    elem1,
    elem2,
    elem3
};

MSGPACK_ADD_ENUM(my_enum)

//' Second simple MsgPack Example
//' 
//' @details The function provides a simple illustration of MessagePack.
//' @return A boolean value of \code{TRUE} is returned, but the function exists
//' for its side effect.
//' @seealso The MessagePack documentation, particularly the msgpack-c examples.
// [[Rcpp::export]]
bool enumEx() {
    {
        // pack, unpack
        std::stringstream sbuf;
        msgpack::pack(sbuf, elem1);
        msgpack::pack(sbuf, elem2);
        my_enum e3 = elem3;
        msgpack::pack(sbuf, e3);

        msgpack::unpacked result;
        std::size_t off = 0;

        msgpack::unpack(result, sbuf.str().data(), sbuf.str().size(), off);
        Rcpp::Rcout << result.get().as<my_enum>() << std::endl;
        assert(result.get().as<my_enum>() == elem1);

        msgpack::unpack(result, sbuf.str().data(), sbuf.str().size(), off);
        Rcpp::Rcout << result.get().as<my_enum>() << std::endl;
        assert(result.get().as<my_enum>() == elem2);

        msgpack::unpack(result, sbuf.str().data(), sbuf.str().size(), off);
        Rcpp::Rcout << result.get().as<my_enum>() << std::endl;
        assert(result.get().as<my_enum>() == elem3);
    }
    {   // create object without zone
        msgpack::object obj(elem2);
        Rcpp::Rcout << obj.as<my_enum>() << std::endl;
        assert(obj.as<my_enum>() == elem2);
    }
    {   // create object with zone
        msgpack::zone z;
        msgpack::object objz(elem3, z);
        Rcpp::Rcout << objz.as<my_enum>() << std::endl;
        assert(objz.as<my_enum>() == elem3);
    }
    return true;
}
