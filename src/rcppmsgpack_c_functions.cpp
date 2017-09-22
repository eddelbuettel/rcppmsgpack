#define MSGPACK_USE_BOOST

#include "anyvector.hpp"
#include "msgpack.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <Rcpp.h>

using namespace Rcpp;

bool temp_bool;
double temp_double;
int temp_int;
std::string temp_string;
std::vector<unsigned char> temp_unsigned_char;

const double R_INT_MAX = 2147483647;

SEXP c_unpack(std::vector<unsigned char> char_message);
AnyVector unpackVector(const std::vector<msgpack::object> &obj_vector, bool const &simplify);
SEXP unpackVisitor(const msgpack::object &obj, bool const &simplify);
RawVector c_pack(SEXP root_obj);
void addToPack(const SEXP &obj, msgpack::packer<std::stringstream>& pkr);
void packAtomic(const AnyVector &vec, const int &j, msgpack::packer<std::stringstream>& pkr);

void packAtomic(const AnyVector &vec, const int &j, msgpack::packer<std::stringstream>& pkr) {
    switch(vec.which()) {
        case 0:
            temp_bool = boost::get<LogicalVector>(vec)[j];
            pkr.pack(temp_bool);
            break;
        case 1:
            temp_int = boost::get<IntegerVector>(vec)[j];
            pkr.pack(temp_int);
            break;
        case 2:
            temp_double = boost::get<NumericVector>(vec)[j];
            pkr.pack(temp_double);
            break;
        case 3:
            temp_string = boost::get<CharacterVector>(vec)[j];
            pkr.pack(temp_string);
            break;
    }
}

void addToPack(const SEXP &obj, msgpack::packer<std::stringstream>& pkr) {
    if(Rf_isVectorList(obj)) {
        List temp_list = List(obj);
        if(temp_list.hasAttribute("class") && (as< std::vector<std::string> >(temp_list.attr("class")))[0] == "map") {
            // std::cout << "map:" << std::endl;
            List key = List(temp_list[0]);
            List value = List(temp_list[1]);
            pkr.pack_map(key.size());
            for(int j=0; j<key.size(); j++) {
                addToPack(key[j], pkr);
                addToPack(value[j], pkr);
            }
        } else if(temp_list.hasAttribute("names")) {
            CharacterVector keys = temp_list.names();
            LogicalVector nakeys = is_na(keys);
            pkr.pack_map(temp_list.size());
            for(int j=0; j<temp_list.size(); j++) {
                if(nakeys[j]) {
                    pkr.pack_nil();
                } else {
                    pkr.pack(as<std::string>(keys[j]));
                }
                addToPack(temp_list[j], pkr);
            }
        } else {
            List temp_list = List(obj);
            pkr.pack_array(temp_list.size());
            for(int j=0; j<temp_list.size(); j++) {
                addToPack(temp_list[j], pkr);
            }
        }
    } else if(TYPEOF(obj) == RAWSXP) {
        RawVector rv = RawVector(obj);
        if(rv.hasAttribute("EXT")) {
            std::vector<unsigned char> rvvu = as< std::vector<unsigned char> >(obj);
            std::vector<char> rvv = std::vector<char>(rvvu.begin(), rvvu.end());
            int8_t ext_type = static_cast<int8_t> (as<std::vector<int> >(rv.attr("EXT"))[0]);
            size_t ext_l = rv.size();
            pkr.pack_ext(ext_l, ext_type);
            pkr.pack_ext_body(rvv.data(), ext_l);
        } else {
            pkr.pack(as< std::vector<unsigned char> >(obj));
        }
    } else if(TYPEOF(obj) == NILSXP) {
        pkr.pack_nil();
    } else {
        AnyVector vec = sexpToAnyVector(obj);
        int vec_size = size(vec);
        LogicalVector navec = is_na(vec);
        if(hasAttribute(vec, "names")) {
            CharacterVector keys = attr(vec, "names");
            LogicalVector nakeys = is_na(keys);
            pkr.pack_map(vec_size);
            for(int j=0; j < vec_size; j++) {
                if(nakeys[j]) {
                    pkr.pack_nil();
                } else {
                    pkr.pack(as<std::string>(keys[j]));
                }
                if(navec[j]) {
                    pkr.pack_nil();
                } else {
                    packAtomic(vec, j, pkr);
                }
            }
        } else {
            if(vec_size != 1) {
                pkr.pack_array(vec_size);
            }
            for(int j=0; j < vec_size; j++) {
                if(navec[j]) {
                    pkr.pack_nil();
                } else {
                    packAtomic(vec, j, pkr);
                }
            }
        }
    }
}

// [[Rcpp::export]]
RawVector c_pack(SEXP root_obj) {
    std::stringstream buffer;
    msgpack::packer<std::stringstream> pk(&buffer);
    if(Rf_isVectorList(root_obj)) {
        List root_list = List(root_obj);
        if(root_list.hasAttribute("class") && (as< std::vector<std::string> >(root_list.attr("class")))[0] == "msgpack_set") {
            for(int i=0; i<root_list.size(); i++) {
                addToPack(root_list[i], pk);
            }
            std::string bufstr = buffer.str();
            RawVector bufraw(bufstr.begin(), bufstr.end());
            return bufraw;
        }
    }
    addToPack(root_obj, pk);
    std::string bufstr = buffer.str();
    RawVector bufraw(bufstr.begin(), bufstr.end());
    return bufraw;
}

AnyVector unpackVector(const std::vector<msgpack::object> &obj_vector, bool const &simplify) {
    bool list_type = false;
    bool numeric_type = false;
    bool integer_type = false;
    bool logical_type = false;
    bool character_type = false;
    bool null_type = false;
    int sum_types = 0;
    if(simplify) {
        for(int j=0; j<obj_vector.size(); j++) {
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
                    if(fabs(temp_double) < R_INT_MAX) {
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
                default:
                    list_type = true;
                    break;
            }
            if(list_type) break;
            sum_types = (numeric_type || integer_type) + logical_type + character_type;
            if(sum_types > 1) break;
        }
    }
    // vector size 0 - return empty list
    if(!simplify || list_type || sum_types > 1 || sum_types == 0 || obj_vector.size() == 0) {
        List L = List(obj_vector.size());
        for(int j=0; j<obj_vector.size(); j++) {
            L[j] = unpackVisitor(obj_vector[j], simplify);
        }
        return L;
    } else if(numeric_type) {
        NumericVector v = NumericVector(obj_vector.size());
        for(int j=0; j<obj_vector.size(); j++) {
            if(obj_vector[j].type == msgpack::type::NIL) {
                v[j] = NA_REAL;
            } else {
                obj_vector[j].convert(temp_double);
                v[j] = temp_double;
            }
        }
        return v;
    } else if(integer_type) {
        IntegerVector v = IntegerVector(obj_vector.size());
        for(int j=0; j<obj_vector.size(); j++) {
            if(obj_vector[j].type == msgpack::type::NIL) {
                v[j] = NA_INTEGER;
            } else {
                obj_vector[j].convert(temp_int);
                v[j] = temp_int;
                // v[j] = obj_vector[j].as<int>();
            }
        }
        return v;
    } else if(logical_type) {
        LogicalVector v = LogicalVector(obj_vector.size());
        for(int j=0; j<obj_vector.size(); j++) {
            if(obj_vector[j].type == msgpack::type::NIL) {
                v[j] = NA_LOGICAL;
            } else {
                obj_vector[j].convert(temp_bool);
                v[j] = temp_bool;
            }
        }
        return v;
    } else if(character_type) {
        CharacterVector v = CharacterVector(obj_vector.size());
        for(int j=0; j<obj_vector.size(); j++) {
            if(obj_vector[j].type == msgpack::type::NIL) {
                v[j] = NA_LOGICAL;
            } else {
                obj_vector[j].convert(temp_string);
                v[j] = temp_string;
            }
        }
        return v;
    }
    return LogicalVector::create(); //should never reach
}

SEXP unpackVisitor(const msgpack::object &obj, bool const &simplify) {
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
        AnyVector keys = unpackVector(key_vector, simplify);
        AnyVector values = unpackVector(value_vector, simplify);
        if(simplify && keys.which() == 3) { //CharacterVector is 3
            setAttr(values, "names", boost::get<CharacterVector>(keys));
            // map[1].attr("names") = CharacterVector(map[0]);
            return anyVectorToSexp(values);
        } else {
            List map = List(2);
            map[0] = anyVectorToSexp(keys);
            map[1] = anyVectorToSexp(values);
            map.attr("class") = CharacterVector::create("map", "data.frame");
            map.attr("row.names") = seq_len(msize);
            map.names() = CharacterVector::create("key", "value");
            return map;
        }
    } else if(obj.type == msgpack::type::EXT) {
        int8_t vtype = obj.via.ext.type();
        uint32_t vsize = obj.via.ext.size;
        const char* vdata = obj.via.ext.data();
        RawVector rv = RawVector(vdata, vdata + vsize);
        rv.attr("EXT") = IntegerVector::create(vtype);
        return rv;
    } else {
        switch (obj.type) {
            case msgpack::type::NIL:
                // std::cout << "nil" << std::endl;
                return R_NilValue;
            case msgpack::type::BOOLEAN:
                // std::cout << "boolean" << std::endl;
                obj.convert(temp_bool);
                return wrap(temp_bool);
            case msgpack::type::POSITIVE_INTEGER:
            case msgpack::type::NEGATIVE_INTEGER:
                // std::cout << "integer" << std::endl;
                obj.convert(temp_double);
                if(fabs(temp_double) < R_INT_MAX) {
                    obj.convert(temp_int);
                    return wrap(temp_int);
                } else {
                    return wrap(temp_double);
                }
            case msgpack::type::FLOAT32:
            case msgpack::type::FLOAT64:
                // std::cout << "float" << std::endl;
                obj.convert(temp_double);
                return wrap(temp_double);
            case msgpack::type::STR:
                // std::cout << "string" << std::endl;
                obj.convert(temp_string);
                return wrap(temp_string);
            case msgpack::type::BIN:
                obj.convert(temp_unsigned_char);
                return RawVector(temp_unsigned_char.begin(), temp_unsigned_char.end());
            default:
                break;
        }
    }
    return LogicalVector::create(); //should never reach
}


// [[Rcpp::export]]
SEXP c_unpack(std::vector<unsigned char> char_message, bool simplify) {
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
        msgpack::object obj = oh.get();
        L.push_back(unpackVisitor(obj, simplify));
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
