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

void addToPack(SEXP obj, msgpack::packer<std::stringstream>& pkr) {
    if(Rf_isVectorList(obj)) {
        List temp_list = List(obj);
        if(temp_list.hasAttribute("class") && (as< std::vector<std::string> >(temp_list.attr("class")))[0] == "map") {
            // std::cout << "map:" << std::endl;
            List key = temp_list[0];
            List value = temp_list[1];
            pkr.pack_map(key.size());
            for(int j=0; j<key.size(); j++) {
                addToPack(key[j], pkr);
                addToPack(value[j], pkr);
            }
        } else if(temp_list.hasAttribute("names")) {
            std::vector<std::string> names = as< std::vector<std::string> >(temp_list.names());
            pkr.pack_map(temp_list.size());
            for(int j=0; j<temp_list.size(); j++) {
                pkr.pack(names[j]);
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
    } else {
        switch(TYPEOF(obj)) {
            case NILSXP:
                pkr.pack_nil();
                break;
            case LGLSXP:
                pkr.pack(as<bool>(obj));
                break;
            case INTSXP:
                pkr.pack(as<int>(obj));
                break;
            case REALSXP:
                pkr.pack(as<double>(obj));
                break;
            case STRSXP:
                // std::cout << "string:" << std::endl;
                pkr.pack(as<std::string>(obj));
                break;
        }
    }
}

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(BH)]]
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
