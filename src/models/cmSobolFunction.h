#ifndef CMSOBOLFUNCTION_H
#define CMSOBOLFUNCTION_H

# include <string>

# include "uqTypes.h"
# include "cmModel.h"
# include "cmException.h"

using namespace std;

// GENERIC CLASS FOR MODELS
class cmSobolFunction: public cmModel{
  protected:
    int dims;
    stdVec params;
    int functionType;
  public:

    //! Default constructor
    cmSobolFunction(int numDim, int ftype, const stdVec& pk);
    //! Virtual destructor
    virtual ~cmSobolFunction();

    virtual int getParameterTotal();
    virtual int getStateTotal();
    virtual int getResultTotal();
    virtual void getDefaultParameterLimits(stdVec& limits);
    virtual void getDefaultParams(stdVec& params);
    virtual void getPriorMapping(int priorModelType,int* prPtr);
    virtual string getParamName(int parID);
    virtual string getResultName(int resID);
    virtual double evalModelError(const stdVec& inputs,stdVec& outputs,stdIntVec& errorCode);
};

#endif // CMSOBOLFUNCTION_H
