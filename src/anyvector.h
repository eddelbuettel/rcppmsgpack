#pragma once

#include "boost/variant.hpp"
#include <Rcpp.h>

using namespace Rcpp;

using AnyVector = boost::variant<LogicalVector, IntegerVector, NumericVector, CharacterVector, List>;

bool hasAttribute(AnyVector vec, std::string attr) {
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
            return boost::get<List>(vec).hasAttribute(attr);
    }
    return false;
}

CharacterVector attr(AnyVector vec, std::string attr) {
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
            return boost::get<List>(vec).attr(attr);
    }
    return CharacterVector::create();
}

void setAttr(AnyVector &vec, std::string attr, CharacterVector attr_value) {
    if(vec.which() == 0) {
            LogicalVector v = boost::get<LogicalVector>(vec);
            v.attr(attr) = attr_value;
            vec = v;
    } else if(vec.which() == 1) {
            IntegerVector v = boost::get<IntegerVector>(vec);
            v.attr(attr) = attr_value;
            vec = v;
    } else if(vec.which() == 2) {
            NumericVector v = boost::get<NumericVector>(vec);
            v.attr(attr) = attr_value;
            vec = v;
    } else if(vec.which() == 3) {
            CharacterVector v = boost::get<CharacterVector>(vec);
            v.attr(attr) = attr_value;
            vec = v;
    } else if(vec.which() == 4) {
            List v = boost::get<List>(vec);
            v.attr(attr) = attr_value;
            vec = v;
    }
}

int size(AnyVector vec) {
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
            return boost::get<List>(vec).size();
    }
    return 0;
}

LogicalVector is_na(AnyVector vec) {
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
            return is_na(boost::get<List>(vec)); //does this work?
    }
    return LogicalVector::create();
}

AnyVector RObjectToAnyVector(RObject obj) {
    AnyVector vec;
    switch(obj.sexp_type()) {
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
        case VECSXP:
            vec = List(obj);
            break;
    }
    return vec;
}

RObject anyVectorToRObject(AnyVector vec) {
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
            return boost::get<List>(vec);
    }
    return LogicalVector::create(); // should never reach
}
