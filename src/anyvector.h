#pragma once

#include "boost/variant.hpp"
#include <Rcpp.h>

namespace Rcpp {

using AnyVector = boost::variant<LogicalVector, IntegerVector, NumericVector, CharacterVector, RawVector, List>;

// while boost::get is considered unsafe, it was faster than the recommended apply visitor approach
bool hasAttribute(const AnyVector &vec, const std::string &attr) {
    switch(vec.which()) {
        case 0:
            return boost::get<LogicalVector>(vec).hasAttribute(attr);
        case 1:
            return boost::get<IntegerVector>(vec).hasAttribute(attr);
        case 2:
            return boost::get<NumericVector>(vec).hasAttribute(attr);
        case 3:
            return boost::get<CharacterVector>(vec).hasAttribute(attr);
        case 4:
            return boost::get<RawVector>(vec).hasAttribute(attr);
        case 5:
            return boost::get<List>(vec).hasAttribute(attr);
    }
    return false;
}

CharacterVector attr(const AnyVector &vec, const std::string &attr) {
    switch(vec.which()) {
        case 0:
            return boost::get<LogicalVector>(vec).attr(attr);
        case 1:
            return boost::get<IntegerVector>(vec).attr(attr);
        case 2:
            return boost::get<NumericVector>(vec).attr(attr);
        case 3:
            return boost::get<CharacterVector>(vec).attr(attr);
        case 4:
            return boost::get<RawVector>(vec).attr(attr);
        case 5:
            return boost::get<List>(vec).attr(attr);
    }
    return CharacterVector::create();
}

void setAttr(AnyVector &vec, const std::string &attr, const CharacterVector &attr_value) {
    if(vec.which() == 0) {
        LogicalVector & v = boost::get<LogicalVector>(vec);
        v.attr(attr) = attr_value;
    } else if(vec.which() == 1) {
        IntegerVector & v = boost::get<IntegerVector>(vec);
        v.attr(attr) = attr_value;
    } else if(vec.which() == 2) {
        NumericVector & v = boost::get<NumericVector>(vec);
        v.attr(attr) = attr_value;
    } else if(vec.which() == 3) {
        CharacterVector & v = boost::get<CharacterVector>(vec);
        v.attr(attr) = attr_value;
    } else if(vec.which() == 4) {
        RawVector & v = boost::get<RawVector>(vec);
        v.attr(attr) = attr_value;
    } else if(vec.which() == 5) {
        List & v = boost::get<List>(vec);
        v.attr(attr) = attr_value;
    }
}

int size(const AnyVector &vec) {
    switch(vec.which()) {
        case 0:
            return boost::get<LogicalVector>(vec).size();
        case 1:
            return boost::get<IntegerVector>(vec).size();
        case 2:
            return boost::get<NumericVector>(vec).size();
        case 3:
            return boost::get<CharacterVector>(vec).size();
        case 4:
            return boost::get<RawVector>(vec).size();
        case 5:
            return boost::get<List>(vec).size();
    }
    return 0;
}

LogicalVector is_na(const AnyVector &vec) {
    switch(vec.which()) {
        case 0:
            return is_na(boost::get<LogicalVector>(vec));
        case 1:
            return is_na(boost::get<IntegerVector>(vec));
        case 2:
            return is_na(boost::get<NumericVector>(vec));
        case 3:
            return is_na(boost::get<CharacterVector>(vec));
        case 4:
            return is_na(boost::get<RawVector>(vec));
        case 5:
            return is_na(boost::get<List>(vec)); //does this work?
    }
    return LogicalVector::create();
}

AnyVector sexpToAnyVector(const SEXP & obj) {
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
        case RAWSXP:
            vec = RawVector(obj);
            break;
        case VECSXP:
            vec = List(obj);
            break;
    }
    return vec;
}

SEXP anyVectorToSexp(const AnyVector &vec) {
    switch(vec.which()) {
        case 0:
            return boost::get<LogicalVector>(vec);
        case 1:
            return boost::get<IntegerVector>(vec);
        case 2:
            return boost::get<NumericVector>(vec);
        case 3:
            return boost::get<CharacterVector>(vec);
        case 4:
            return boost::get<RawVector>(vec);
        case 5:
            return boost::get<List>(vec);
    }
    return LogicalVector::create(); // should never reach
}


int getType(const AnyVector &vec) {
    switch(vec.which()) {
        case 0:
            return LGLSXP;
        case 1:
            return INTSXP;
        case 2:
            return REALSXP;
        case 3:
            return STRSXP;
        case 4:
            return RAWSXP;
        case 5:
            return VECSXP;
    }
    return 0; // should never reach
}

// SEXP getElement(const AnyVector &vec, int & j) {
//     switch(vec.which()) {
//         case 0:
//             return boost::get<LogicalVector>(vec)[j];
//         case 1:
//             return boost::get<IntegerVector>(vec)[j];
//         case 2:
//             return boost::get<NumericVector>(vec)[j];
//         case 3:
//             return boost::get<CharacterVector>(vec)[j];
//         case 4:
//             return boost::get<RawVector>(vec)[j];
//         case 5:
//             return boost::get<List>(vec)[j];
//     }
//     return LogicalVector::create(); // should never reach
// }

}
