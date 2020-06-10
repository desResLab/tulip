# include "ntNode.h"

ntNode::ntNode(int nodeID,ntNetIO* netIO){
  // Assign Node ID
  this->nodeID;
  // The node has not been processed yet
  this->processed = false;

  // Read veriable names and samples
  stdStringVec varNames;
  stdMat varSamples;
  stdVec varSTD;
  netIO->readVariableFile(nodeID,numVariables,varNames,varSTD,varSamples);

  // In a processed node the messages have been propagated
  processed = false;

  // Init Gaussian sampler
  uqPDF* nSampler = new uqGaussianPDF();
  // Init Uniform sampler
  uqPDF* uSampler = new uqUniformPDF();

}

ntNode::~ntNode(){
  delete nSampler;
  delete uSampler;
}

stdMat ntNode::getMsg(int targetFactorID){
  bool found = false;
  int count = 0;
  while(!found){
    found = (messages[count]->targetID == targetFactorID);
    if(!found){
      count += 1;  
    }
  }
  // Return the sample table
  return messages[count]->msg;
}

// Check if the node has evidence
bool ntNode::hasEvidence(){
  if(evidenceVarID.size() > 0){
    return true;
  }else{
    return false;
  }
}

// Propagate messages from nodes to factors
void ntNode::sendMsgToFactors(){

  int varID     = 0;
  double varVal = 0.0;
  double std    = 0.0;
  double noise  = 0.0;

  // Loop on all factor connected to the node
  for(int loopA=0;loopA<nodeFactors.size();loopA++){

    // Get Messages from all factors but loopA and aggegate
    stdMat currMsg;
    for(int loopB=0;loopB<nodeFactors.size();loopB++){
      if(loopB != loopA){
        ntUtils::aggregateMsgs(currMsg,nodeFactors[loopB]->getMsg(nodeID));
      } 
    }

    // Add evidence to the message if specified
    if(evidenceVarID.size() > 0){
      // If there is evidence for certain variables, replace these variables with the evidence
      for(int loopA=0;loopA<evidenceVarID.size();loopA++){
        varID = evidenceVarID[loopA];
        varVal = evidenceVarValue[loopA];
        std = varSTD[varID];
        for(int loopB=0;loopB<numSamples;loopB++){
          noise = nSampler->sample(0.0,std);
          currMsg[loopB][varID] = varVal + noise;
        }
      }
    }

    // Pass message to factor loopA
    if(isDownstreamFactor[loopA]){
      // Simply pass the variables with/without evidence downstream
      nodeFactors[loopA]->updateMsg(nodeID,currMsg);  
    }else{
      // Upstream from a leave
      stdMat msgToSend = uniformSampleUnobserved(currMsg);
      nodeFactors[loopA]->updateMsg(nodeID,msgToSend);
    }
  }
}

void ntNode::updateMsg(int factorID,const stdMat& msg){
  for(size_t loopA=0;loopA<messages.size();loopA++){
    if((messages[loopA]->messageType == mtNodeToFactor)&&(messages[loopA]->sourceID == factorID)){
      messages[loopA]->msg = msg;
    }
  }
}

stdMat ntNode::forwardUQ(const stdMat& msg){
  // Create Samples
  uqSamples* samples = new uqSamples(msg);
  // Initialize 
  acActionUP_MC* mc = new acActionUP_MC(samples);
  mc->setModel(nodeApproximant);
  mc->opts.numberOfRepeats = 1;
  mc->opts.sampleGroups.push_back(msg.size());
  mc->opts.storeSamples = true;
  // Process Model
  mc->go();
  // Return resulting samples
  stdMat res = mc->all_outputs;
  // Free memory
  delete mc;
  // Return
  return res;
}

stdMat ntNode::inverseUQ(const stdMat& msg){

  // Set MCMC parameters
  int totChains           = msg[0].size(); // Number of variables
  int totGenerations      = 300;
  int totalCR             = 3;
  int totCrossoverPairs   = 5;
  double dreamGRThreshold = 1.2;
  int dreamJumpStep       = 10;
  int dreamGRPrintStep    = 10;

  string dreamChainFileName("chain_GR_000000.txt");
  string dreamGRFileName("gr_GR.txt");
  
  string dreamRestartReadFileName("");
  string dreamRestartWriteFileName("restart_write_GR.txt");

  bool usePriorFromFile = false;
  string priorFileName("");
  int priorModelType = 0;

  // Create Dream Sampler
  acActionDREAMseq* mcmc = new acActionDREAMseq(totChains,totGenerations,totalCR,totCrossoverPairs,
                                                dreamChainFileName,dreamGRFileName,dreamGRThreshold,
                                                dreamJumpStep,dreamGRPrintStep,
                                                dreamRestartReadFileName,dreamRestartWriteFileName,
                                                usePriorFromFile, priorFileName, priorModelType);
  mcmc->setModel(nodeApproximant);
  // Run MCMC Simulation
  mcmc->go();

  // Post process chains
  bool debugMode = false;
  double burnInPercent = 50.0;
  mcmc->postProcess(debugMode,burnInPercent);
  
  // Extract Parameter Samples
  stdMat res;
  stdIntVec resIDX;
  int mcmcSamples = msg.size();
  cmUtils::subSampleTableData("paramTraces.txt",mcmcSamples,2,2 + limits.size()/2,res,resIDX);
  if(mcmcSamples < msg.size()){
    throw ntException("ERROR: Not enough samples from MCMC.");
  }
  
  // Free memory
  delete mcmc;

  // Return Message
  return res;
}

bool ntNode::varIsUnobserved(int varID){
  bool res = false;
  for(int loopA=0;loopA<evidenceVarID.size();loopA++){
    res = res && (varID == evidenceVarID[loopA]);
  }
  return !res;
}

stdMat ntNode::uniformSampleUnobserved(const stdMat& msg){
  stdMat res = msg;
  double currMin = 0.0;
  double currMax = 0.0;

  for(int loopA=0;loopA<msg[0].size();loopA++){
    if(varIsUnobserved(loopA)){
      
      // Get Limits
      currMin = limits[2*loopA + 0];
      currMax = limits[2*loopA + 0];

      for(int loopB=0;loopB<msg.size();loopB++){
        res[loopB][loopA] = uSampler->sample(currMin,currMax);
      }
    }
  }
}
