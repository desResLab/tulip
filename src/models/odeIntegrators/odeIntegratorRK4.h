#ifndef ODEINTEGRATORRK4_H
#define ODEINTEGRATORRK4_H

# include "odeIntegrator.h"
# include "odeModel.h"

// GENERIC CLASS FOR SAMPLES
class odeIntegratorRK4: public odeIntegrator{
  public:

    // Constructor
    odeIntegratorRK4(odeModel* odeModel, double timeStep, int totalSteps, int totalCycles);

    // Destructor
    virtual ~odeIntegratorRK4();

    // Perform numerical integration     
    virtual int run(const stdVec& iniVals,const stdVec& params,stdMat& outVals,stdMat& auxOutVals);
};

#endif // ODEINTEGRATORRK4_H
