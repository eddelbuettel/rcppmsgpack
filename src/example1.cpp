// This example is based on msgpack-c/example/cpp03/simple.cpp 

#include <msgpack.hpp>
#undef ERROR

#include <Rcpp.h>

//' Simple MsgPack Example
//' 
//' @details The function provides a simple illustration of MessagePack.
//' @return A boolean value of \code{TRUE} is returned, but the function exists
//' for its side effect.
//' @seealso The MessagePack documentation, particularly the msgpack-c examples.
// [[Rcpp::export]]
bool arrayEx() {
    msgpack::type::tuple<int, bool, std::string> src(1, true, "example");

    // serialize the object into the buffer.
    // any classes that implements write(const char*,size_t) can be a buffer.
    std::stringstream buffer;
    msgpack::pack(buffer, src);

    // send the buffer ...
    buffer.seekg(0);

    // deserialize the buffer into msgpack::object instance.
    std::string str(buffer.str());

    msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());

    // deserialized object is valid during the msgpack::unpacked instance alive.
    msgpack::object deserialized = oh.get();

    // msgpack::object supports ostream.
    Rcpp::Rcout << deserialized << std::endl;

    // convert msgpack::object instance into the original type.
    // if the type is mismatched, it throws msgpack::type_error exception.
    msgpack::type::tuple<int, bool, std::string> dst;
    deserialized.convert(dst);

    return true;
}
