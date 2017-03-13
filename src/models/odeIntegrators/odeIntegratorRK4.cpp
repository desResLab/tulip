# include "odeIntegratorRK4.h"

using namespace std;

// Constructor
odeIntegratorRK4::odeIntegratorRK4(odeModel* odeModel, double timeStep, int totalSteps, int totalCycles)
:odeIntegrator(odeModel,timeStep,totalSteps,totalCycles){
}

odeIntegratorRK4::~odeIntegratorRK4(){
}

int odeIntegratorRK4::run(const stdVec& iniVals,const stdVec& params,stdMat& outVals,stdMat& auxOutVals){

  // Intialize Current Time
  double currTime      = 0.0;
  int    stepId        = 0;
  bool   isValidDouble = true;

  // Get Total number of States
  int totalStates  = ode->getStateTotal();
  int totAuxStates = ode->getAuxStateTotal();

  // Allocate Temporary State Variables
  stdVec k1(totalStates,0.0);
  stdVec k2(totalStates,0.0);
  stdVec k3(totalStates,0.0);
  stdVec k4(totalStates,0.0);
  // Allocate Auxiliary Vectors
  stdVec k1AuxOut(totAuxStates,0.0);
  stdVec k2AuxOut(totAuxStates,0.0);
  stdVec k3AuxOut(totAuxStates,0.0);
  stdVec k4AuxOut(totAuxStates,0.0);
  
  stdVec Xk2(totalStates,0.0);
  stdVec Xk3(totalStates,0.0);
  stdVec Xk4(totalStates,0.0);

  stdVec Ind(totalStates,0.0);

  // Initialize State and Derivative Vectors
  stdVec Xn(totalStates,0.0);
  stdVec Xn1(totalStates,0.0);

  // Set Initial Conditions
  for(int loopA=0;loopA<ode->getStateTotal();loopA++){
    Xn[loopA] = iniVals[loopA];
  }

  // Time loop
  for(int loopA=0;loopA<totalSteps;loopA++){

    // Increment Time Step
    stepId++;

    // Eval K1
    ode->eval(currTime,Xn,params,forcing,k1,k1AuxOut);

    // Eval K2
    for(int loopB=0;loopB<totalStates;loopB++){
      Xk2[loopB] = Xn[loopB] + ((1.0/3.0)*timeStep) * k1[loopB];
    }
    ode->eval(currTime + (1.0/3.0) * timeStep,Xk2,params,forcing,k2,k2AuxOut);

    // Eval K3
    for(int loopB=0;loopB<totalStates;loopB++){
      Xk3[loopB] = Xn[loopB] - (1.0/3.0)*timeStep * k1[loopB] + (1.0*timeStep) * k2[loopB];
    }
    ode->eval(currTime + (2.0/3.0) * timeStep,Xk3,params,forcing,k3,k3AuxOut);

    // Eval K4
    for(int loopB=0;loopB<totalStates;loopB++){
      Xk4[loopB] = Xn[loopB] + timeStep*k1[loopB] - timeStep*k2[loopB] + timeStep * k3[loopB];
    }
    ode->eval(currTime + timeStep,Xk4,params,forcing,k4,k4AuxOut);

    // Eval Xn1
    for(int loopB=0;loopB<totalStates;loopB++){
      if(Ind[loopB] > 0) {
         Xn1[loopB] = Xn[loopB] + (1.0/8.0)*timeStep*(k1[loopB] + 3.0 * k2[loopB] + 3.0 * k3[loopB] + k4[loopB]);
      }
      else {
         Xn1[loopB] = 0.0;
      }
    }

    // CHECK IF SOLUTION IS VALID
    if((stepId % 100) == 0){
      for(int loopB=0;loopB<totalStates;loopB++){
        isValidDouble = (Xn1[loopB] != Xn1[loopB]);
        if(isValidDouble){
          return 1;
        }
      }
    }

    // Update Xn
    for(int loopB=0;loopB<totalStates;loopB++){
      Xn[loopB] = Xn1[loopB];
    }

    // Update Current Time
    currTime += timeStep;

    // SAVE OUTPUT FOR EVERY 3D MODEL SOLUTION
    for(int loopB=0;loopB<totAuxStates;loopB++){
      auxOutVals[loopB][stepId] = k4AuxOut[loopB];
    }

    // Copy The solution back for each time step
    for(int loopB=0;loopB<totalStates;loopB++){
      outVals[loopB][stepId] = Xn1[loopB];
    }
  }

  // RETURN OK
  return 0;
}
