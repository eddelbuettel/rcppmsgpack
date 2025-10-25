#define MSGPACK_USE_BOOST

#include "anyvector.h"
#include "msgpack.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <Rcpp.h>
#include <cmath>
#include <cstdint>

const double R_INT_MAX = 2147483647;
const double R_INT_MIN = -2147483648;


SEXP c_unpack(std::vector<unsigned char> char_message);
Rcpp::AnyVector unpackVector(const std::vector<msgpack::object> &obj_vector, bool const simplify);
SEXP unpackVisitor(const msgpack::object &obj, bool const simplify);
Rcpp::RawVector c_pack(SEXP root_obj);
template <typename STREAM> void addToPack(const SEXP &obj, msgpack::packer<STREAM>& pkr);
template <typename STREAM> void packElement(const Rcpp::AnyVector &vec, const Rcpp::LogicalVector & navec, const int j, msgpack::packer<STREAM>& pkr);

template <typename STREAM> void packElement(const Rcpp::AnyVector & vec, const Rcpp::LogicalVector & navec, const int j, msgpack::packer<STREAM>& pkr) {
    bool temp_bool;
    double temp_double;
    int temp_int;
    std::string temp_string;
    
    switch(getType(vec)) {
    case LGLSXP:
        if(navec[j]) {
            pkr.pack_nil();
        } else {
            temp_bool = boost::get<Rcpp::LogicalVector>(vec)[j];
            pkr.pack(temp_bool);
        }
        break;
    case INTSXP:
        temp_int = boost::get<Rcpp::IntegerVector>(vec)[j];
        pkr.pack(temp_int);
        // pkr.pack( as< std::vector<int> >(boost::get<IntegerVector>(vec))[j] );
        break;
    case REALSXP:
        temp_double = boost::get<Rcpp::NumericVector>(vec)[j];
        pkr.pack(temp_double);
        break;
    case STRSXP:
        if(navec[j]) {
            pkr.pack_nil();
        } else {
            temp_string = boost::get<Rcpp::CharacterVector>(vec)[j];
            pkr.pack(temp_string);
        }
        break;
    case VECSXP:
        addToPack(boost::get<Rcpp::List>(vec)[j], pkr);
    }
}

template <typename STREAM> void addToPack(const SEXP &obj, msgpack::packer<STREAM>& pkr) {
    if(Rf_isVectorList(obj)) {
        Rcpp::List temp_list = Rcpp::List(obj);
        // Map
        if(temp_list.hasAttribute("class") && (Rcpp::as< std::vector<std::string> >(temp_list.attr("class")))[0] == "map") {
            // std::cout << "map:" << std::endl;
            Rcpp::AnyVector key = sexpToAnyVector(temp_list[0]);
            Rcpp::AnyVector value = sexpToAnyVector(temp_list[1]);
            Rcpp::LogicalVector nakey = is_na(key);
            Rcpp::LogicalVector navalue = is_na(value);
            int len = size(key);
            pkr.pack_map(len);
            for(int j=0; j<len; j++) {
                packElement(key, nakey, j, pkr);
                packElement(value, navalue, j, pkr);
            }
            // Also Map
        } else if(temp_list.hasAttribute("names")) {
            Rcpp::CharacterVector keys = temp_list.names();
            Rcpp::LogicalVector nakeys = is_na(keys);
            pkr.pack_map(temp_list.size());
            for(int j=0; j<temp_list.size(); j++) {
                if(nakeys[j]) {
                    pkr.pack_nil();
                } else {
                    pkr.pack(Rcpp::as<std::string>(keys[j]));
                }
                addToPack(temp_list[j], pkr);
            }
            // Array
        } else {
            Rcpp::List temp_list = Rcpp::List(obj);
            pkr.pack_array(temp_list.size());
            for(int j=0; j<temp_list.size(); j++) {
                addToPack(temp_list[j], pkr);
            }
        }
    } else if(TYPEOF(obj) == RAWSXP) {
        Rcpp::RawVector rv = Rcpp::RawVector(obj);
        // EXT
        if(rv.hasAttribute("EXT")) {
            std::vector<unsigned char> rvvu = Rcpp::as< std::vector<unsigned char> >(obj);
            std::vector<char> rvv = std::vector<char>(rvvu.begin(), rvvu.end());
            int8_t ext_type = static_cast<int8_t> (Rcpp::as<std::vector<int> >(rv.attr("EXT"))[0]);
            size_t ext_l = rv.size();
            pkr.pack_ext(ext_l, ext_type);
            pkr.pack_ext_body(rvv.data(), ext_l);
            // BIN
        } else {
            pkr.pack(Rcpp::as< std::vector<unsigned char> >(obj));
        }
        // NIL
    } else if(TYPEOF(obj) == NILSXP) {
        pkr.pack_nil();
    } else {
        Rcpp::AnyVector vec = Rcpp::sexpToAnyVector(obj);
        int vec_size = size(vec);
        Rcpp::LogicalVector navec = is_na(vec);
        // Map again
        if(hasAttribute(vec, "names")) {
            Rcpp::CharacterVector key = attr(vec, "names");
            Rcpp::LogicalVector nakey = is_na(key);
            pkr.pack_map(vec_size);
            for(int j=0; j < vec_size; j++) {
                if(nakey[j]) {
                    pkr.pack_nil();
                } else {
                    pkr.pack(Rcpp::as<std::string>(key[j]));
                }
                packElement(vec, navec, j, pkr);
            }
            // Array and atomic types
        } else {
            if(vec_size != 1) {
                pkr.pack_array(vec_size);
            }
            for(int j=0; j < vec_size; j++) {
                packElement(vec, navec, j, pkr);
            }
        }
    }
}

// [[Rcpp::export]]
Rcpp::RawVector c_pack(SEXP root_obj) {
    //std::stringstream buffer;
    // msgpack::packer<std::stringstream> pk(&buffer);
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> pk(&sbuf);
    if(Rf_isVectorList(root_obj)) {
        Rcpp::List root_list = Rcpp::List(root_obj);
        if(root_list.hasAttribute("class") && (Rcpp::as< std::vector<std::string> >(root_list.attr("class")))[0] == "msgpack_set") {
            for(int i=0; i<root_list.size(); i++) {
                addToPack(root_list[i], pk);
            }
            // std::string bufstr = buffer.str();
            //RawVector bufraw(bufstr.begin(), bufstr.end());
            Rcpp::RawVector bufraw(sbuf.data(), sbuf.data()+sbuf.size());
            return bufraw;
        }
    }
    addToPack(root_obj, pk);
    // std::string bufstr = buffer.str();
    // RawVector bufraw(bufstr.begin(), bufstr.end());
    Rcpp::RawVector bufraw(sbuf.data(), sbuf.data()+sbuf.size());
    return bufraw;
}

Rcpp::AnyVector unpackVector(const std::vector<msgpack::object> &obj_vector, bool const simplify) {
    
    // reusable objects for converting from msgpack::object
    bool temp_bool;
    double temp_double;
    int temp_int;
    std::string temp_string;
    // std::vector<unsigned char> temp_unsigned_char;
    
    // variables for determining vector type during unpacking
    bool list_type = false;
    bool numeric_type = false;
    bool integer_type = false;
    bool logical_type = false;
    bool character_type = false;
    bool null_type = false;
    int sum_types = 0;
    
    if(simplify) {
        for (unsigned int j=0; j<obj_vector.size(); j++) {
            switch(obj_vector[j].type) {
            case msgpack::type::NIL:
                null_type = true;
                break;
            case msgpack::type::BOOLEAN:
                logical_type = true;
                break;
            case msgpack::type::POSITIVE_INTEGER:
            case msgpack::type::NEGATIVE_INTEGER:
                obj_vector[j].convert(temp_double);
                if(temp_double <= R_INT_MAX && temp_double >= R_INT_MIN) {
                    integer_type = true;
                } else {
                    numeric_type = true;
                }
                break;
            case msgpack::type::FLOAT32:
            case msgpack::type::FLOAT64:
                numeric_type = true;
                break;
            case msgpack::type::STR:
                character_type = true;
                break;
            default: //bin, ext, array, map
                list_type = true;
            break;
            }
            sum_types = numeric_type + integer_type + logical_type + character_type + list_type;
            if(list_type) break;
            if((numeric_type || integer_type) && null_type) break;
            if(sum_types > 1) break;
        }
        if(sum_types == 1) {
            if(numeric_type && !null_type) {
                Rcpp::NumericVector v = Rcpp::NumericVector(obj_vector.size());
                for (unsigned int j=0; j<obj_vector.size(); j++) {
                    obj_vector[j].convert(temp_double);
                    v[j] = temp_double;
                }
                return v;
            } else if(integer_type && !null_type) {
                Rcpp::IntegerVector v = Rcpp::IntegerVector(obj_vector.size());
                for (unsigned int j=0; j<obj_vector.size(); j++) {
                    obj_vector[j].convert(temp_int);
                    v[j] = temp_int;
                }
                return v;
            } else if(logical_type) {
                Rcpp::LogicalVector v = Rcpp::LogicalVector(obj_vector.size());
                for (unsigned int j=0; j<obj_vector.size(); j++) {
                    if(obj_vector[j].type == msgpack::type::NIL) {
                        v[j] = NA_LOGICAL;
                    } else {
                        obj_vector[j].convert(temp_bool);
                        v[j] = temp_bool;
                    }
                }
                return v;
            } else if(character_type) {
                Rcpp::CharacterVector v = Rcpp::CharacterVector(obj_vector.size());
                for (unsigned int j=0; j<obj_vector.size(); j++) {
                    if(obj_vector[j].type == msgpack::type::NIL) {
                        v[j] = NA_STRING;
                    } else {
                        obj_vector[j].convert(temp_string);
                        v[j] = temp_string;
                    }
                }
                return v;
            }
        }
    }
    // list type
    // vector size 0 also returns empty list
    Rcpp::List L = Rcpp::List(obj_vector.size());
    for (unsigned int j=0; j<obj_vector.size(); j++) {
        L[j] = unpackVisitor(obj_vector[j], simplify);
    }
    return L;
}

SEXP unpackVisitor(const msgpack::object &obj, bool const simplify) {
    if(obj.type == msgpack::type::ARRAY) {
        std::vector<msgpack::object> temp_vector;
        obj.convert(temp_vector);
        return anyVectorToSexp(unpackVector(temp_vector, simplify));
    } else if(obj.type == msgpack::type::MAP) {
        int msize = obj.via.map.size;
        std::vector<msgpack::object> key_vector(msize);
        std::vector<msgpack::object> value_vector(msize);
        msgpack::object_kv* p = obj.via.map.ptr;
        msgpack::object_kv* const pend = obj.via.map.ptr + obj.via.map.size;
        int i = 0;
        for (; p < pend; ++p) {
            key_vector[i] = p->key.as<msgpack::object>();
            value_vector[i] = p->val.as<msgpack::object>();
            i++;
        }
        Rcpp::AnyVector keys = unpackVector(key_vector, simplify);
        Rcpp::AnyVector values = unpackVector(value_vector, simplify);
        if(simplify && getType(keys) == STRSXP) {
            setAttr(values, "names", boost::get<Rcpp::CharacterVector>(keys));
            // map[1].attr("names") = CharacterVector(map[0]);
            return anyVectorToSexp(values);
        } else {
            Rcpp::List map = Rcpp::List(2);
            map[0] = anyVectorToSexp(keys);
            map[1] = anyVectorToSexp(values);
            map.attr("class") = Rcpp::CharacterVector::create("map", "data.frame");
            map.attr("row.names") = Rcpp::seq_len(msize);
            map.names() = Rcpp::CharacterVector::create("key", "value");
            return map;
        }
    } else if(obj.type == msgpack::type::EXT) {
        int8_t vtype = obj.via.ext.type();
        uint32_t vsize = obj.via.ext.size;
        const char* vdata = obj.via.ext.data();
        Rcpp::RawVector rv = Rcpp::RawVector(vdata, vdata + vsize);
        rv.attr("EXT") = Rcpp::IntegerVector::create(vtype);
        return rv;
    } else {
        
        // reusable objects for converting from msgpack::object
        bool temp_bool;
        double temp_double;
        int temp_int;
        std::string temp_string;
        std::vector<unsigned char> temp_unsigned_char;
        
        switch (obj.type) {
        case msgpack::type::NIL:
            // std::cout << "nil" << std::endl;
            return R_NilValue;
        case msgpack::type::BOOLEAN:
            // std::cout << "boolean" << std::endl;
            obj.convert(temp_bool);
            return Rcpp::wrap(temp_bool);
        case msgpack::type::POSITIVE_INTEGER:
        case msgpack::type::NEGATIVE_INTEGER:
            // std::cout << "integer" << std::endl;
            obj.convert(temp_double);
            if(temp_double <= R_INT_MAX && temp_double >= R_INT_MIN) {
                obj.convert(temp_int);
                return Rcpp::wrap(temp_int);
            } else {
                return Rcpp::wrap(temp_double);
            }
        case msgpack::type::FLOAT32:
        case msgpack::type::FLOAT64:
            // std::cout << "float" << std::endl;
            obj.convert(temp_double);
            return Rcpp::wrap(temp_double);
        case msgpack::type::STR:
            // std::cout << "string" << std::endl;
            obj.convert(temp_string);
            return Rcpp::wrap(temp_string);
        case msgpack::type::BIN:
            obj.convert(temp_unsigned_char);
            return Rcpp::RawVector(temp_unsigned_char.begin(), temp_unsigned_char.end());
        default:
            break;
        }
    }
    return Rcpp::LogicalVector::create(); //should never reach
}


// [[Rcpp::export]]
SEXP c_unpack(Rcpp::RawVector char_message, bool simplify) {
    // std::vector<unsigned char> char_message = as< std::vector<unsigned char> >(raw_message); // cast from RawVector
    // std::string message(char_message.begin(), char_message.end());
    // msgpack::object_handle oh = msgpack::unpack(message.data(), message.size());
    //msgpack::object_handle oh;
    
    std::size_t off = 0;
    std::size_t len = char_message.size();
    char* mdata = reinterpret_cast<char*>(RAW(char_message));
    // char* mdata = reinterpret_cast<char*>(char_message.data());
    // const char* dat = ss.str().data();
    std::vector<SEXP> L(0);
    while(off != len) {
        msgpack::object_handle oh;
        msgpack::unpack(oh, mdata, len, off);
        msgpack::object obj = oh.get();
        L.push_back(unpackVisitor(obj, simplify));
    }
    if(L.size() != 1) {
        Rcpp::List LL = Rcpp::List(L.size());
        for(size_t i=0; i<L.size(); i++) {
            LL[i] = L[i];
        }
        LL.attr("class") = "msgpack_set";
        return LL;
    }
    return L[0];
    // msgpack::type::variant v = oh.get().as<msgpack::type::variant>();
    // List L = List(1);
    // SEXP ret = boost::apply_visitor(unpack_visitor(), v);
    // return ret;
}

// 0xd6 | -1 | seconds in 32-bit unsigned int
// 0xd7 | -1 | nanoseconds in 30-bit unsigned int | seconds in 34-bit unsigned int
// 0xc7 | 12 | -1 | nanoseconds in 32-bit unsigned int | seconds in 64-bit signed int
//Bit operations: https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
// [[Rcpp::export]]
Rcpp::RawVector c_timestamp_encode(double seconds, uint32_t nanoseconds) {
    int64_t secint = round(seconds);
    Rcpp::RawVector rv;
    if ((nanoseconds == 0) & (seconds <= 4294967295) & (seconds >= 0)) { //2^32-1
        std::vector<unsigned char> msg(4);
        for(int i=0; i<32; i++) {
            if((secint >> i) & 1) msg[(31-i)/8] |= 1 << (i % 8);
        }
        rv = Rcpp::RawVector(msg.begin(), msg.end());
    } else if ((seconds <= 17179869183) & (seconds >= 0)) { //2^34-1
        std::vector<unsigned char> msg(8);
        for(int i=0; i<34; i++) {
            if((secint >> i) & 1) msg[(63-i)/8] |= 1 << (i % 8);
        }
        for(int i=0; i<30; i++) {
            if((nanoseconds >> i) & 1) msg[(29-i)/8] |= 1 << ((i+2) % 8);
        }
        rv = Rcpp::RawVector(msg.begin(), msg.end());
    } else {
        std::vector<unsigned char> msg(12);
        for(int i=0; i<64; i++) {
            if((secint >> i) & 1) msg[(95-i)/8] |= 1 << (i % 8);
        }
        for(int i=0; i<32; i++) {
            if((nanoseconds >> i) & 1) msg[(31-i)/8] |= 1 << (i % 8);
        }
        rv = Rcpp::RawVector(msg.begin(), msg.end());
    }
    rv.attr("EXT") = Rcpp::IntegerVector::create(-1);
    return rv;
}

// [[Rcpp::export]]
Rcpp::List c_timestamp_decode(std::vector<unsigned char> v) {
    int64_t seconds;
    int nanoseconds;
    if(v.size() == 4) {
        seconds = (v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
        nanoseconds = 0;
    } else if(v.size() == 8) {
        nanoseconds = (v[0] << 22) | (v[1] << 14) | (v[2] << 6) | (v[3] & 252);
        seconds = ((static_cast<int64_t>(v[3]) & 3) << 32) |
            (static_cast<int64_t>(v[4]) << 24) |
            (static_cast<int64_t>(v[5]) << 16) |
            (static_cast<int64_t>(v[6]) << 8) |
            static_cast<int64_t>(v[7]);
    } else {
        nanoseconds = (v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
        seconds = (static_cast<int64_t>(v[4]) << 56) |
            (static_cast<int64_t>(v[5]) << 48) |
            (static_cast<int64_t>(v[6]) << 40) |
            (static_cast<int64_t>(v[7]) << 32) |
            (static_cast<int64_t>(v[8]) << 24) |
            (static_cast<int64_t>(v[9]) << 16) |
            (static_cast<int64_t>(v[10]) << 8) |
            static_cast<int64_t>(v[11]);
    }
    Rcpp::List l = Rcpp::List(2);
    l[0] = static_cast<double>(seconds);
    l[1] = nanoseconds;
    l.attr("names") = Rcpp::CharacterVector::create("seconds", "nanoseconds");
    return l;
}
