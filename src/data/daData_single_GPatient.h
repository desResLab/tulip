#ifndef DADATA_SINGLE_GPATIENT_H
#define DADATA_SINGLE_GPATIENT_H

# include <string>
# include <map>
# include <stdio.h>
# include <stdlib.h>
# include "math.h"

# include "uqTypes.h"
# include "daData.h"
# include "daException.h"
# include "cmUtils.h"

using namespace std;

/*! 
Child class to read patient information through a single CSV file.
Only the information from REDCap source is retained. 
*/
class daData_single_GPatient: public daData{
  protected:    
    // Column in file storing the key
    int keyColumn;
    // Time stamp
    int timeStampColumn;

  public:

  	daData_single_GPatient(int keyColumn,int timeStampColumn);
    virtual ~daData_single_GPatient();

    virtual void   readFromFile(string filaName);
    virtual double evalOBJ(const stdStringVec& keys,const stdVec& values,const stdVec& weights);
    virtual double evalLogLikelihood(const stdStringVec& keys,const stdVec& avValues,const stdVec& stdFactors,const stdVec& weights);
    virtual double evalLikelihood(const stdStringVec& keys,const stdVec& avValues,const stdVec& stdFactors,const stdVec& weights);
    virtual void   printAndCompare(const stdStringVec& keys,const stdVec& values,const stdVec& weigths);
    virtual int    getPatientValue(string key,double& result);
    virtual void   printToScreen();
    virtual void   getTable(stdMat& table);

    void getAvailableKeys(stdStringVec& foundKeys);
};

#endif // DADATA_SINGLE_GPATIENT_H
