#define MSGPACK_USE_BOOST

#include "msgpack.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <Rcpp.h>

using namespace Rcpp;

const double R_INT_MAX = 2147483647;

struct unpack_visitor:boost::static_visitor<SEXP> {
    SEXP operator()(msgpack::type::nil_t const v) const {
        // std::cout << "nil" << std::endl;
        return R_NilValue;
    }
    SEXP operator()(bool const v) const {
        // std::cout << "bool:" << v << std::endl;
        return LogicalVector::create(v);
    }
    SEXP operator()(uint64_t const v) const {
        // std::cout << "uint:" << v << std::endl;
        if(static_cast<double>(v) < R_INT_MAX) {
            return IntegerVector::create(v);
        } else {
            return NumericVector::create(static_cast<double>(v));
        }
    }
    SEXP operator()(int64_t const v) const {
        // std::cout << "int:" << v << std::endl;
        if(std::abs(static_cast<double>(v)) < R_INT_MAX) {
            return IntegerVector(v);
        } else {
            return NumericVector::create(static_cast<double>(v));
        }
    }
    SEXP operator()(double const v) const {
        // std::cout << "double:" << v << std::endl;
        return NumericVector::create(v);
    }
    SEXP operator()(std::string const& v) const {
        // std::cout << "string:" << v << std::endl;
        return CharacterVector::create(v);
    }
    SEXP operator()(std::vector<char> const& v) const {
        // std::cout << "bin:" << std::endl;
        // std::vector<unsigned char> vu = std::vector<unsigned char>(v.begin(), v.end());
        return RawVector(v.begin(), v.end());
    }
    SEXP operator()(msgpack::type::ext const& v) const {
        // std::cout << "ext:" << std::endl;
        int8_t vtype = v.type();
        uint32_t vsize = v.size();
        const char* vdata = v.data();
        std::vector<char> vvdata(vdata, vdata + vsize);
        RawVector rv = RawVector(vdata, vdata + vsize);
        rv.attr("EXT") = IntegerVector::create(vtype);
        return rv;
    }
    SEXP operator()(std::vector<msgpack::type::variant> const& v) const {
        // std::cout << "array" << std::endl;
        List L = List(v.size());
        for(unsigned int j=0; j<v.size(); j++) {
            msgpack::type::variant obj = v[j];
            L[j] = boost::apply_visitor(*this, obj);
        }
        return L;
    }
    SEXP operator()(std::multimap<msgpack::type::variant, msgpack::type::variant> const& v) const {
        // std::cout << "map found" << std::endl;
        int vsize = v.size();
        List key = List(vsize);
        List value = List(vsize);
        int j = 0;
        for (auto& e : v) {
            // std::cout << "key:" << std::endl;
            key[j] = boost::apply_visitor(*this, e.first);
            // std::cout << "value:" << std::endl;
            value[j] = boost::apply_visitor(*this, e.second);
            j++;
        }
        List map = List(2);
        map[0] = key;
        map[1] = value;
        map.attr("class") = CharacterVector::create("map", "data.frame");
        map.attr("row.names") = seq_len(j);
        map.names() = CharacterVector::create("key", "value");
        return map;
    }
    template <typename T>
    SEXP operator()(T const&) const {
        return(R_NilValue);
    }
};

// [[Rcpp::export]]
SEXP c_unpack(std::vector<unsigned char> char_message) {
    // std::vector<unsigned char> char_message = as< std::vector<unsigned char> >(raw_message); // cast from RawVector
    std::string message(char_message.begin(), char_message.end());
    // msgpack::object_handle oh = msgpack::unpack(message.data(), message.size());
    //msgpack::object_handle oh;
    std::size_t off = 0;
    std::size_t len = message.size();
    // const char* dat = ss.str().data();
    List L = List(0);
    while(off != len) {
        msgpack::object_handle oh;
        msgpack::unpack(oh, message.data(), len, off);
        msgpack::type::variant v = oh.get().as<msgpack::type::variant>();
        L.push_back(boost::apply_visitor(unpack_visitor(), v));
    }
    if(L.size() != 1) {
        L.attr("class") = "msgpack_set";
        return L;
    }
    return L[0];
    // msgpack::type::variant v = oh.get().as<msgpack::type::variant>();
    // List L = List(1);
    // SEXP ret = boost::apply_visitor(unpack_visitor(), v);
    // return ret;
}


// int main() {
//     // create test data
//     std::stringstream ss;
//     msgpack::packer<std::stringstream> pk(ss);
//     pk.pack_map(1);
//     pk.pack("t");
//     pk.pack_array(2);
//     pk.pack_array(3);
//     pk.pack(1);
//     pk.pack(2);
//     pk.pack(3);
//     pk.pack_array(3);
//     pk.pack(4);
//     pk.pack(5);
//     pk.pack(6);

//     auto oh = msgpack::unpack(ss.str().data(), ss.str().size());
//     std::cout << oh.get() << std::endl;

//     msgpack::type::variant v = oh.get().as<msgpack::type::variant>();
//     boost::apply_visitor(unpack_visitor(), v);
// }
