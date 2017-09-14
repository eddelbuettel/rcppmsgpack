#define MSGPACK_USE_BOOST

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

using AnyVector = boost::variant<LogicalVector, IntegerVector, NumericVector, CharacterVector>;

// packs atomic types, e.g. vector of length 1; this function assumes not NA
void packAtomic(AnyVector vec, int j, msgpack::packer<std::stringstream>& pkr) {
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

bool anyVectorHasAttribute(AnyVector vec, std::string attr) {
    switch(vec.which()) {
        case 0:
            return boost::get<LogicalVector>(vec).hasAttribute(attr);
            break;
        case 1:
            return boost::get<IntegerVector>(vec).hasAttribute(attr);
            break;
        case 2:
            return boost::get<NumericVector>(vec).hasAttribute(attr);
            break;
        case 3:
            return boost::get<CharacterVector>(vec).hasAttribute(attr);
            break;
    }
    return false;
}

CharacterVector anyVectorAttr(AnyVector vec, std::string attr) {
    switch(vec.which()) {
        case 0:
            return boost::get<LogicalVector>(vec).attr(attr);
            break;
        case 1:
            return boost::get<IntegerVector>(vec).attr(attr);
            break;
        case 2:
            return boost::get<NumericVector>(vec).attr(attr);
            break;
        case 3:
            return boost::get<CharacterVector>(vec).attr(attr);
            break;
    }
    return CharacterVector::create();
}

int anyVectorSize(AnyVector vec) {
    switch(vec.which()) {
        case 0:
            return boost::get<LogicalVector>(vec).size();
            break;
        case 1:
            return boost::get<IntegerVector>(vec).size();
            break;
        case 2:
            return boost::get<NumericVector>(vec).size();
            break;
        case 3:
            return boost::get<CharacterVector>(vec).size();
            break;
    }
    return 0;
}

LogicalVector anyVectorIs_na(AnyVector vec) {
    switch(vec.which()) {
        case 0:
            return is_na(boost::get<LogicalVector>(vec));
            break;
        case 1:
            return is_na(boost::get<IntegerVector>(vec));
            break;
        case 2:
            return is_na(boost::get<NumericVector>(vec));
            break;
        case 3:
            return is_na(boost::get<CharacterVector>(vec));
            break;
    }
    return 0;
}

void addToPack(SEXP obj, msgpack::packer<std::stringstream>& pkr) {
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
        AnyVector vec;
        switch(TYPEOF(obj)) {
            case LGLSXP:
                vec = LogicalVector(obj);
                break;
            case INTSXP:
                vec = IntegerVector(obj);
                break;
            case REALSXP:
                vec = NumericVector(obj);
                break;
            case STRSXP:
                vec = CharacterVector(obj);
                break;
        }
        int vec_size = anyVectorSize(vec);
        LogicalVector navec = anyVectorIs_na(vec);
        if(anyVectorHasAttribute(vec, "names")) {
            CharacterVector keys = anyVectorAttr(vec, "names");
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
            if(vec_size > 1) {
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

    // if not msgpack_set
    addToPack(root_obj, pk);

    std::string bufstr = buffer.str();
    RawVector bufraw(bufstr.begin(), bufstr.end());
    return bufraw;
}
