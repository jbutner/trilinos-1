//**************************************************************************
//
//                                 NOTICE
//
// This software is a result of the research described in the report
//
// " A comparison of algorithms for modal analysis in the absence 
//   of a sparse direct method", P. Arbenz, R. Lehoucq, and U. Hetmaniuk,
//  Sandia National Laboratories, Technical report SAND2003-1028J.
//
// It is based on the Epetra, AztecOO, and ML packages defined in the Trilinos
// framework ( http://software.sandia.gov/trilinos/ ).
//
// The distribution of this software follows also the rules defined in Trilinos.
// This notice shall be marked on any reproduction of this software, in whole or
// in part.
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Code Authors: U. Hetmaniuk (ulhetma@sandia.gov), R. Lehoucq (rblehou@sandia.gov)
//
//**************************************************************************

#ifndef ARPACK_MODE3_H
#define ARPACK_MODE3_H

#include "Epetra_ConfigDefs.h"

#include "Epetra_Comm.h"
#include "Epetra_Operator.h"
#include "Epetra_Time.h"

#ifdef EPETRA_MPI
#include "Epetra_MpiComm.h"
#endif

#include "FortranRoutines.h"
#include "ModalAnalysisSolver.h"
#include "MyMemory.h"
#include "CheckingTools.h"

class ARPACKm3 : public ModalAnalysisSolver {

  private:

    const CheckingTools myVerify;
    const FortranRoutines callFortran;

    const Epetra_Comm &MyComm;
    const Epetra_Operator *K;
    const Epetra_Operator *M;
    const Epetra_Time MyWatch;

    double tolEigenSolve;
    int maxIterEigenSolve;

    int verbose;

    double memRequested;
    double highMem;

    int massOp;
    int orthoOp;
    int outerIter;
    int stifOp;

    double timeMassOp;
    double timeOuterLoop;
    double timePostProce;
    double timeStifOp;

    // Don't define these functions
    ARPACKm3(const ARPACKm3 &ref);
    ARPACKm3& operator=(const ARPACKm3 &ref);

  public:

    ARPACKm3(const Epetra_Comm &_Comm, const Epetra_Operator *KK,
              double _tol = 1.0e-08, int _maxIter = 100, int _verb = 0);

    ARPACKm3(const Epetra_Comm &_Comm, const Epetra_Operator *KK, const Epetra_Operator *MM,
              double _tol = 1.0e-08, int _maxIter = 100, int _verb = 0);

    ~ARPACKm3() { }

    int solve(int numEigen, Epetra_MultiVector &Q, double *lambda);

    int minimumSpaceDimension(int nev) const         { return nev+1; }

    void initializeCounters();

    void algorithmInfo() const;
    void memoryInfo() const;
    void operationInfo() const;
    void timeInfo() const;

};

#endif
