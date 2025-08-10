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

#endif // CWISE_H
