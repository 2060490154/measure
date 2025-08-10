#ifndef CWISE_H
#define CWISE_H
#include <Eigen/Dense>
#include <Eigen/Core>
#include <QtGlobal>

using namespace Eigen;

//define a costom cwise-function(Greater comparation)
template<typename Scalar> struct GComp{
    EIGEN_EMPTY_STRUCT_CTOR(GComp)
    typedef int result_type;
    int operator()(const Scalar& a, const Scalar& b)const{
        if (a >= b){
            return 1;
        }else{
            return 0;
        }
    }
};


//define a costom cwise-function(Smaller comparation)
template<typename Scalar> struct SComp{
    EIGEN_EMPTY_STRUCT_CTOR(SComp)
    typedef int result_type;
    int operator()(const Scalar& a, const Scalar& b)const{
        if (a <= b){
            return 1;
        }else{
            return 0;
        }
    }
};


//define a costom cwise-function(setzero for nan or inf)
//template<typename Scalar> struct IsNaN_or_Inf{
//    EIGEN_EMPTY_STRUCT_CTOR(IsNaN_or_Inf)
//    typedef void result_type;
//    void operator()(const Scalar& a, const Scalar& b)const{
//        if (qIsNaN(a) || qIsInf(a)){
//            a = 0;
//        }
//    }
//};




#endif // CWISE_H
