#ifndef NTNODE_H
#define NTNODE_H

# include "uqTypes.h"
# include "ntNetIO.h"
# include "ntFactor.h"
# include "ntMessage.h"
# include "ntUtils.h"
# include "uqPDF.h"
# include "cmApproximant.h"
# include "acActionUP.h"
# include "acActionUP_MC.h"
# include "acActionDREAMseq.h"

using namespace std;

class ntFactor;

/*! 
Abstract parent class for probabilistic networks in UQ
*/
class ntNode{
  public:
     // Nodes ID
    bool nodeID;
    // Has the node been processed
    bool processed;
    // Number of variables
    int numVariables;
    // Number of variables
    int numSamples;
    // Variables Name strings
    stdStringVec varNames;
    // Variable standard deviations
    stdVec varSTD;
    // Variable limits
    stdVec limits;
    // Variable realizations
    stdMat varSamples;
    // Evidence: variable ID and evidence value
    stdIntVec evidenceVarID;
    stdVec evidenceVarValue;
    // List of pointers to the factors
    vector<ntFactor*> nodeFactors;
    stdBoolVec isDownstreamFactor;
    // List of messages to factors
    vector<ntMessage*> messages;
    // Gaussian sampler
    uqPDF* nSampler;
    uqPDF* uSampler;

    // Approximant 
    cmModel* nodeApproximant;

  	//! Default Constructor
  	ntNode(int nodeID,ntNetIO* netIO);
    //! Virtual Destructor
    virtual ~ntNode();

    stdMat getMsg(int targetFactorID);

    // Check if the node has evidence
    bool hasEvidence();

    // Forward and inverse Uncertainty Propagation
    stdMat forwardUQ(const stdMat& msg);
    stdMat inverseUQ(const stdMat& msg);

    // Update message 
    void updateMsg(int factorID,const stdMat& msg);

    // Check if variable is unobserved
    bool varIsUnobserved(int varID);

    // Random sample the unobserved variables from the limits
    stdMat uniformSampleUnobserved(const stdMat& msg);

    // Propagate from the node to the factors that are connected
    virtual void sendMsgToFactors();

};

#endif // NTNODE_H
