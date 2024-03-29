# include "cmApproximant.h"

using namespace std;

cmApproximant::cmApproximant(string fileName){
  // Construct approximant
  approx = new uqApproximant_ME();
  approx->importFromFile(fileName);
}

cmApproximant::~cmApproximant(){
  delete approx;
}

int cmApproximant::getParameterTotal(){
  stdVec limits = approx->getLimits();
  return limits.size()/2;
}

int cmApproximant::getStateTotal(){
  return 0;
}

int cmApproximant::getResultTotal(){
  // All Single-element approximants have the same number of outputs
  // So we take the first
  return approx->approxList[0]->coeffs[0].size();
}

void cmApproximant::getDefaultParams(stdVec& params){
  stdVec limits = approx->getLimits();
  params.clear();
  for(int loopA=0;loopA<limits.size()/2;loopA++){
    params.push_back(0.5*(limits[loopA*2]+limits[loopA*2+1]));
  }
}

void cmApproximant::getDefaultParameterLimits(stdVec& limits){
  limits.clear();
  limits = approx->getLimits();
}

void cmApproximant::getPriorMapping(int priorModelType,int* prPtr){
}

string cmApproximant::getParamName(int parID){
  if((parID<0)&&(parID>=getParameterTotal())){
    throw cmException("ERROR: Invalid parameter ID.\n");
  }
  return string("p") + to_string(parID);
}
string cmApproximant::getResultName(int resID){
  if((resID<0)&&(resID>=getResultTotal())){
    throw cmException("ERROR: Invalid parameter ID.\n");
  }
  return string("r") + to_string(resID);
}

double cmApproximant::evalModelError(const stdVec& inputs,stdVec& outputs,stdIntVec& errorCode){
  stdMat xValues;
  stdMat res;
  xValues.push_back(inputs);
  // Evaluate Surrogate
  res = approx->evaluate(xValues);
  outputs.clear();
  outputs = res[0];
  // Error Code
  errorCode.clear();
  errorCode.push_back(0);
  // Return 0.0
  return 0.0;
}
