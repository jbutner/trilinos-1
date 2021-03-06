
//@HEADER
// ************************************************************************
// 
//               Epetra: Linear Algebra Services Package 
//                 Copyright 2011 Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ************************************************************************
//@HEADER

#include "Epetra_ConfigDefs.h"
#include "Epetra_Util.h"
#include "Epetra_Object.h"
#include "Epetra_Comm.h"
#include "Epetra_Directory.h"
#include "Epetra_Map.h"
#include "Epetra_LocalMap.h"
#include "Epetra_CrsGraph.h"
#include "Epetra_CrsMatrix.h"
#include "Epetra_MultiVector.h"
#include "Epetra_IntVector.h"
#include "Epetra_Import.h"

#ifdef HAVE_MPI
#include "Epetra_MpiDistributor.h"
#endif

const double Epetra_Util::chopVal_ = 1.0e-15;

//=========================================================================
double Epetra_Util::Chop(const double & Value){
  if (std::abs(Value) < chopVal_) return 0;
  return Value;
}

//=========================================================================
unsigned int Epetra_Util::RandomInt() {

  const int a = 16807;
  const int m = 2147483647;
  const int q = 127773;
  const int r = 2836;

  int hi = Seed_ / q;
  int lo = Seed_ % q;
  int test = a * lo - r * hi;
  if (test > 0)
    Seed_ = test;
  else
    Seed_ = test + m;
  
  return(Seed_);
}

//=========================================================================
double Epetra_Util::RandomDouble() {
  const double Modulus = 2147483647.0;
  const double DbleOne = 1.0;
  const double DbleTwo = 2.0;

  double randdouble = RandomInt(); // implicit conversion from int to double
  randdouble = DbleTwo * (randdouble / Modulus) - DbleOne; // scale to (-1.0, 1.0)

  return(randdouble);
}

//=========================================================================
unsigned int Epetra_Util::Seed() const {
  return(Seed_);
}

//=========================================================================
int Epetra_Util::SetSeed(unsigned int Seed_in) {
  Seed_ = Seed_in;
  return(0);
}

//=============================================================================
template<typename T>
void Epetra_Util::Sort(bool SortAscending, int NumKeys, T * Keys, 
           int NumDoubleCompanions,double ** DoubleCompanions, 
           int NumIntCompanions, int ** IntCompanions,
           int NumLongLongCompanions, long long ** LongLongCompanions)
{
  int i;

  int n = NumKeys;
  T * const list = Keys;
  int m = n/2;
  
  while (m > 0) {
    int max = n - m;
    for (int j=0; j<max; j++)
      {
  for (int k=j; k>=0; k-=m)
    {
      if ((SortAscending && list[k+m] >= list[k]) || 
    ( !SortAscending && list[k+m] <= list[k]))
        break;
      T temp = list[k+m];
      list[k+m] = list[k];
      list[k] = temp;
      for (i=0; i<NumDoubleCompanions; i++) {
        double dtemp = DoubleCompanions[i][k+m];
      DoubleCompanions[i][k+m] = DoubleCompanions[i][k];
      DoubleCompanions[i][k] = dtemp;
      }
      for (i=0; i<NumIntCompanions; i++) {
        int itemp = IntCompanions[i][k+m];
      IntCompanions[i][k+m] = IntCompanions[i][k];
      IntCompanions[i][k] = itemp;
      }
      for (i=0; i<NumLongLongCompanions; i++) {
        long long LLtemp = LongLongCompanions[i][k+m];
      LongLongCompanions[i][k+m] = LongLongCompanions[i][k];
      LongLongCompanions[i][k] = LLtemp;
      }
    }
      }
    m = m/2;
  }
}

void Epetra_Util::Sort(bool SortAscending, int NumKeys, int * Keys, 
           int NumDoubleCompanions,double ** DoubleCompanions, 
           int NumIntCompanions, int ** IntCompanions,
           int NumLongLongCompanions, long long ** LongLongCompanions)
{
  return Sort<int>(SortAscending, NumKeys, Keys, 
           NumDoubleCompanions, DoubleCompanions, 
           NumIntCompanions, IntCompanions,
           NumLongLongCompanions, LongLongCompanions);
}

void Epetra_Util::Sort(bool SortAscending, int NumKeys, long long * Keys, 
           int NumDoubleCompanions,double ** DoubleCompanions, 
           int NumIntCompanions, int ** IntCompanions,
           int NumLongLongCompanions, long long ** LongLongCompanions)
{
  return Sort<long long>(SortAscending, NumKeys, Keys, 
           NumDoubleCompanions, DoubleCompanions, 
           NumIntCompanions, IntCompanions,
           NumLongLongCompanions, LongLongCompanions);
}

void Epetra_Util::Sort(bool SortAscending, int NumKeys, int * Keys, 
           int NumDoubleCompanions,double ** DoubleCompanions, 
           int NumIntCompanions, int ** IntCompanions)
{
  int i;

  int n = NumKeys;
  int * const list = Keys;
  int m = n/2;
  
  while (m > 0) {
    int max = n - m;
    for (int j=0; j<max; j++)
      {
  for (int k=j; k>=0; k-=m)
    {
      if ((SortAscending && list[k+m] >= list[k]) || 
    ( !SortAscending && list[k+m] <= list[k]))
        break;
      int temp = list[k+m];
      list[k+m] = list[k];
      list[k] = temp;
      for (i=0; i<NumDoubleCompanions; i++) {
        double dtemp = DoubleCompanions[i][k+m];
      DoubleCompanions[i][k+m] = DoubleCompanions[i][k];
      DoubleCompanions[i][k] = dtemp;
      }
      for (i=0; i<NumIntCompanions; i++) {
        int itemp = IntCompanions[i][k+m];
      IntCompanions[i][k+m] = IntCompanions[i][k];
      IntCompanions[i][k] = itemp;
      }
    }
      }
    m = m/2;
  }
}

//----------------------------------------------------------------------------
#ifndef EPETRA_NO_32BIT_GLOBAL_INDICES // FIXME
// FIXME long long
Epetra_Map
Epetra_Util::Create_OneToOne_Map(const Epetra_Map& usermap,
         bool high_rank_proc_owns_shared)
{
  //if usermap is already 1-to-1 then we'll just return a copy of it.
  if (usermap.IsOneToOne()) {
    Epetra_Map newmap(usermap);
    return(newmap);
  }

  int myPID = usermap.Comm().MyPID();
  Epetra_Directory* directory = usermap.Comm().CreateDirectory(usermap);

  int numMyElems = usermap.NumMyElements();
  const int* myElems = usermap.MyGlobalElements();

  int* owner_procs = new int[numMyElems];

  directory->GetDirectoryEntries(usermap, numMyElems, myElems, owner_procs,
         0, 0, high_rank_proc_owns_shared);

  //we'll fill a list of map-elements which belong on this processor

  int* myOwnedElems = new int[numMyElems];
  int numMyOwnedElems = 0;

  for(int i=0; i<numMyElems; ++i) {
    int GID = myElems[i];
    int owner = owner_procs[i];

    if (myPID == owner) {
      myOwnedElems[numMyOwnedElems++] = GID;
    }
  }

  Epetra_Map one_to_one_map(-1, numMyOwnedElems, myOwnedElems,
       usermap.IndexBase(), usermap.Comm()); // CJ TODO FIXME long long

  delete [] myOwnedElems;
  delete [] owner_procs;
  delete directory;

  return(one_to_one_map);
}
#endif // EPETRA_NO_32BIT_GLOBAL_INDICES
//----------------------------------------------------------------------------
#ifndef EPETRA_NO_32BIT_GLOBAL_INDICES // FIXME
// FIXME long long
Epetra_Map
Epetra_Util::Create_Root_Map(const Epetra_Map& usermap,
         int root)
{
  int numProc = usermap.Comm().NumProc();
  if (numProc==1) {
    Epetra_Map newmap(usermap);
    return(newmap);
  }

  const Epetra_Comm & comm = usermap.Comm();
  bool isRoot = usermap.Comm().MyPID()==root;

  //if usermap is already completely owned by root then we'll just return a copy of it.
  int quickreturn = 0;
  int globalquickreturn = 0;

  if (isRoot) {
    if (usermap.NumMyElements()==usermap.NumGlobalElements64()) quickreturn = 1;
  }
  else {
    if (usermap.NumMyElements()==0) quickreturn = 1;
  }
  usermap.Comm().MinAll(&quickreturn, &globalquickreturn, 1);
  
  if (globalquickreturn==1) {
    Epetra_Map newmap(usermap);
    return(newmap);
  }
  
  // Linear map: Simple case, just put all GIDs linearly on root processor
  if (usermap.LinearMap() && root!=-1) {
    int numMyElements = 0;
    if (isRoot) numMyElements = usermap.MaxAllGID64()+1; // FIXME long long
    Epetra_Map newmap(-1, numMyElements, usermap.IndexBase(), comm); // CJ TODO FIXME long long
    return(newmap);
  }

  if (!usermap.UniqueGIDs()) 
    throw usermap.ReportError("usermap must have unique GIDs",-1);

  // General map

  // Build IntVector of the GIDs, then ship them to root processor
  int numMyElements = usermap.NumMyElements();
  Epetra_Map allGidsMap(-1, numMyElements, 0, comm);
  Epetra_IntVector allGids(allGidsMap);
  for (int i=0; i<numMyElements; i++) allGids[i] = usermap.GID64(i); // CJ TODO FIXME long long
  
  int numGlobalElements = usermap.NumGlobalElements64(); // CJ TODO FIXME long long
  if (root!=-1) {
    int n1 = 0; if (isRoot) n1 = numGlobalElements;
    Epetra_Map allGidsOnRootMap(-1, n1, 0, comm);
    Epetra_Import importer(allGidsOnRootMap, allGidsMap);
    Epetra_IntVector allGidsOnRoot(allGidsOnRootMap);
    allGidsOnRoot.Import(allGids, importer, Insert);
    
    Epetra_Map rootMap(-1, allGidsOnRoot.MyLength(), allGidsOnRoot.Values(), usermap.IndexBase(), comm); // CJ TODO FIXME long long
    return(rootMap);
  }
  else {
    int n1 = numGlobalElements;
    Epetra_LocalMap allGidsOnRootMap(n1, 0, comm);
    Epetra_Import importer(allGidsOnRootMap, allGidsMap);
    Epetra_IntVector allGidsOnRoot(allGidsOnRootMap);
    allGidsOnRoot.Import(allGids, importer, Insert);
    
    Epetra_Map rootMap(-1, allGidsOnRoot.MyLength(), allGidsOnRoot.Values(), usermap.IndexBase(), comm); // CJ TODO FIXME long long

    return(rootMap);
  }
}
#endif // EPETRA_NO_32BIT_GLOBAL_INDICES
//----------------------------------------------------------------------------
#ifndef EPETRA_NO_32BIT_GLOBAL_INDICES // FIXME
// FIXME long long
Epetra_BlockMap
Epetra_Util::Create_OneToOne_BlockMap(const Epetra_BlockMap& usermap,
              bool high_rank_proc_owns_shared)
{
// FIXME long long

  //if usermap is already 1-to-1 then we'll just return a copy of it.
  if (usermap.IsOneToOne()) {
    Epetra_BlockMap newmap(usermap);
    return(newmap);
  }

  int myPID = usermap.Comm().MyPID();
  Epetra_Directory* directory = usermap.Comm().CreateDirectory(usermap);

  int numMyElems = usermap.NumMyElements();
  const int* myElems = usermap.MyGlobalElements();

  int* owner_procs = new int[numMyElems*2];
  int* sizes = owner_procs+numMyElems;

  directory->GetDirectoryEntries(usermap, numMyElems, myElems, owner_procs,
         0, sizes, high_rank_proc_owns_shared);

  //we'll fill a list of map-elements which belong on this processor

  int* myOwnedElems = new int[numMyElems*2];
  int* ownedSizes = myOwnedElems+numMyElems;
  int numMyOwnedElems = 0;

  for(int i=0; i<numMyElems; ++i) {
    int GID = myElems[i];
    int owner = owner_procs[i];

    if (myPID == owner) {
      ownedSizes[numMyOwnedElems] = sizes[i];
      myOwnedElems[numMyOwnedElems++] = GID;
    }
  }

  Epetra_BlockMap one_to_one_map(-1, numMyOwnedElems, myOwnedElems,
         sizes, usermap.IndexBase(), usermap.Comm()); // CJ TODO FIXME long long

  delete [] myOwnedElems;
  delete [] owner_procs;
  delete directory;

  return(one_to_one_map);
}
#endif // EPETRA_NO_32BIT_GLOBAL_INDICES


//----------------------------------------------------------------------------
int Epetra_Util::SortCrsEntries(int NumRows, const int *CRS_rowptr, int *CRS_colind, double *CRS_vals){
  // For each row, sort column entries from smallest to largest.
  // Use shell sort. Stable sort so it is fast if indices are already sorted.
  // Code copied from  Epetra_CrsMatrix::SortEntries() 
  for(int i = 0; i < NumRows; i++){
    int start=CRS_rowptr[i];

    double* locValues = &CRS_vals[start];
    int NumEntries    = CRS_rowptr[i+1] - start;
    int* locIndices   = &CRS_colind[start];
		
    int n = NumEntries;
    int m = n/2;
    
    while(m > 0) {
      int max = n - m;
      for(int j = 0; j < max; j++) {
	for(int k = j; k >= 0; k-=m) {
	  if(locIndices[k+m] >= locIndices[k])
	    break;
	  double dtemp = locValues[k+m];
	  locValues[k+m] = locValues[k];
	  locValues[k] = dtemp;
	  int itemp = locIndices[k+m];
	  locIndices[k+m] = locIndices[k];
	  locIndices[k] = itemp;
	}
      }
      m = m/2;
    }
  }
  return(0);
}


//----------------------------------------------------------------------------
#ifndef EPETRA_NO_32BIT_GLOBAL_INDICES
int Epetra_Util::GetPidGidPairs(const Epetra_Import & Importer,std::vector< std::pair<int,int> > & gpids, bool use_minus_one_for_local){
  // Put the (PID,GID) pair in member of Importer.TargetMap() in gpids.  If use_minus_one_for_local==true, put in -1 instead of MyPID.
  // This only works if we have an MpiDistributor in our Importer.  Otheriwise return an error.
#ifdef HAVE_MPI
  Epetra_MpiDistributor *D=dynamic_cast<Epetra_MpiDistributor*>(&Importer.Distributor());
  if(!D) EPETRA_CHK_ERR(-2);

  int i,j,k;
  int mypid=Importer.TargetMap().Comm().MyPID();
  int N=Importer.TargetMap().NumMyElements();

  // Get the importer's data
  const int *RemoteLIDs  = Importer.RemoteLIDs();

  // Get the distributor's data
  int NumReceives        = D->NumReceives();
  const int *ProcsFrom   = D->ProcsFrom();
  const int *LengthsFrom = D->LengthsFrom();

  // Resize the outgoing data structure
  gpids.resize(N);

  // Start by claiming that I own all the data
  if(use_minus_one_for_local)
    for(i=0;i <N; i++) gpids[i]=std::make_pair(-1,Importer.TargetMap().GID(i));
  else
    for(i=0;i <N; i++) gpids[i]=std::make_pair(mypid,Importer.TargetMap().GID(i));

  // Now, for each remote ID, record who actually owns it.  This loop follows the operation order in the
  // MpiDistributor so it ought to duplicate that effect.
  for(i=0,j=0;i<NumReceives;i++){
    int pid=ProcsFrom[i];
    for(k=0;k<LengthsFrom[i];k++){
      if(pid!=mypid) gpids[RemoteLIDs[j]].first=pid;
      j++;
    }    
  }
  return 0;
#else
  EPETRA_CHK_ERR(-10);
#endif
}
#endif

//----------------------------------------------------------------------------
#ifndef EPETRA_NO_64BIT_GLOBAL_INDICES
int Epetra_Util::GetPidGidPairs(const Epetra_Import & Importer,std::vector< std::pair<int,long long> > & gpids, bool use_minus_one_for_local){
  // Put the (PID,GID) pair in member of Importer.TargetMap() in gpids.  If use_minus_one_for_local==true, put in -1 instead of MyPID.
  // This only works if we have an MpiDistributor in our Importer.  Otheriwise return an error.
#ifdef HAVE_MPI
  Epetra_MpiDistributor *D=dynamic_cast<Epetra_MpiDistributor*>(&Importer.Distributor());
  if(!D) EPETRA_CHK_ERR(-2);

  int i,j,k;
  int mypid=Importer.TargetMap().Comm().MyPID();
  int N=Importer.TargetMap().NumMyElements();

  // Get the importer's data
  const int *RemoteLIDs  = Importer.RemoteLIDs();

  // Get the distributor's data
  int NumReceives        = D->NumReceives();
  const int *ProcsFrom   = D->ProcsFrom();
  const int *LengthsFrom = D->LengthsFrom();

  // Resize the outgoing data structure
  gpids.resize(N);

  // Start by claiming that I own all the data
  if(use_minus_one_for_local)
    for(i=0;i <N; i++) gpids[i]=std::make_pair(-1,Importer.TargetMap().GID64(i));
  else
    for(i=0;i <N; i++) gpids[i]=std::make_pair(mypid,Importer.TargetMap().GID64(i));

  // Now, for each remote ID, record who actually owns it.  This loop follows the operation order in the
  // MpiDistributor so it ought to duplicate that effect.
  for(i=0,j=0;i<NumReceives;i++){
    int pid=ProcsFrom[i];
    for(k=0;k<LengthsFrom[i];k++){
      if(pid!=mypid) gpids[RemoteLIDs[j]].first=pid;
      j++;
    }    
  }
  return 0;
#else
  EPETRA_CHK_ERR(-10);
#endif
}
#endif


//----------------------------------------------------------------------------
int Epetra_Util::GetPids(const Epetra_Import & Importer, std::vector<int> &pids, bool use_minus_one_for_local){
#ifdef HAVE_MPI
  Epetra_MpiDistributor *D=dynamic_cast<Epetra_MpiDistributor*>(&Importer.Distributor());
  if(!D) EPETRA_CHK_ERR(-2);

  int i,j,k;
  int mypid=Importer.TargetMap().Comm().MyPID();
  int N=Importer.TargetMap().NumMyElements();

  // Get the importer's data
  const int *RemoteLIDs  = Importer.RemoteLIDs();

  // Get the distributor's data
  int NumReceives        = D->NumReceives();
  const int *ProcsFrom   = D->ProcsFrom();
  const int *LengthsFrom = D->LengthsFrom();
  
  // Resize the outgoing data structure
  pids.resize(N);

  // Start by claiming that I own all the data
  if(use_minus_one_for_local)
    for(i=0; i<N; i++) pids[i]=-1;
  else
    for(i=0; i<N; i++) pids[i]=mypid;

  // Now, for each remote ID, record who actually owns it.  This loop follows the operation order in the
  // MpiDistributor so it ought to duplicate that effect.
  for(i=0,j=0;i<NumReceives;i++){
    int pid=ProcsFrom[i];
    for(k=0;k<LengthsFrom[i];k++){
      if(pid!=mypid) pids[RemoteLIDs[j]]=pid;
      j++;
    }    
  }
  return 0;
#else
  EPETRA_CHK_ERR(-10);
#endif
}



//----------------------------------------------------------------------------
template<typename T>
int Epetra_Util_binary_search(T item,
                              const T* list,
                              int len,
                              int& insertPoint)
{
  if (len < 1) {
    insertPoint = 0;
    return(-1);
  }

  unsigned start = 0, end = len - 1;

  while(end - start > 1) {
    unsigned mid = (start + end) >> 1;
    if (list[mid] < item) start = mid;
    else end = mid;
  }

  if (list[start] == item) return(start);
  if (list[end] == item) return(end);

  if (list[end] < item) {
    insertPoint = end+1;
    return(-1);
  }

  if (list[start] < item) insertPoint = end;
  else insertPoint = start;

  return(-1);
}

int Epetra_Util_binary_search(int item,
                              const int* list,
                              int len,
                              int& insertPoint)
{
  return Epetra_Util_binary_search<int>(item, list, len, insertPoint);
}

//----------------------------------------------------------------------------
int Epetra_Util_binary_search(long long item,
                              const long long* list,
                              int len,
                              int& insertPoint)
{
  return Epetra_Util_binary_search<long long>(item, list, len, insertPoint);
}

//----------------------------------------------------------------------------
template<typename T>
int Epetra_Util_binary_search_aux(T item,
                              const int* list,
                              const T* aux_list,
                              int len,
                              int& insertPoint)
{
  if (len < 1) {
    insertPoint = 0;
    return(-1);
  }

  unsigned start = 0, end = len - 1;

  while(end - start > 1) {
    unsigned mid = (start + end) >> 1;
    if (aux_list[list[mid]] < item) start = mid;
    else end = mid;
  }

  if (aux_list[list[start]] == item) return(start);
  if (aux_list[list[end]] == item) return(end);

  if (aux_list[list[end]] < item) {
    insertPoint = end+1;
    return(-1);
  }

  if (aux_list[list[start]] < item) insertPoint = end;
  else insertPoint = start;

  return(-1);
}

//----------------------------------------------------------------------------
int Epetra_Util_binary_search_aux(int item,
                              const int* list,
                              const int* aux_list,
                              int len,
                              int& insertPoint)
{
  return Epetra_Util_binary_search_aux<int>(item, list, aux_list, len, insertPoint);
}

//----------------------------------------------------------------------------
int Epetra_Util_binary_search_aux(long long item,
                              const int* list,
                              const long long* aux_list,
                              int len,
                              int& insertPoint)
{
  return Epetra_Util_binary_search_aux<long long>(item, list, aux_list, len, insertPoint);
}



//=========================================================================
int Epetra_Util_ExtractHbData(Epetra_CrsMatrix * A, Epetra_MultiVector * LHS,
            Epetra_MultiVector * RHS,
            int & M, int & N, int & nz, int * & ptr,
            int * & ind, double * & val, int & Nrhs,
            double * & rhs, int & ldrhs,
            double * & lhs, int & ldlhs) {

  int ierr = 0;
  if (A==0) EPETRA_CHK_ERR(-1); // This matrix is defined
  if (!A->IndicesAreContiguous()) { // Data must be contiguous for this to work
    EPETRA_CHK_ERR(A->MakeDataContiguous()); // Call MakeDataContiguous() method on the matrix
    ierr = 1; // Warn User that we changed the matrix
  }
  
  M = A->NumMyRows();
  N = A->NumMyCols();
  nz = A->NumMyNonzeros();
  val = (*A)[0];        // Dangerous, but cheap and effective way to access first element in 
  
  const Epetra_CrsGraph & Graph = A->Graph();
  ind = Graph[0];  // list of values and indices
  
  Nrhs = 0; // Assume no rhs, lhs

  if (RHS!=0) {
    Nrhs = RHS->NumVectors();
    if (Nrhs>1)
    if (!RHS->ConstantStride()) {EPETRA_CHK_ERR(-2)}; // Must have strided vectors
    ldrhs = RHS->Stride();
    rhs = (*RHS)[0]; // Dangerous but effective (again)
  }
  if (LHS!=0) {
    int Nlhs = LHS->NumVectors();
    if (Nlhs!=Nrhs) {EPETRA_CHK_ERR(-3)}; // Must have same number of rhs and lhs
    if (Nlhs>1)
    if (!LHS->ConstantStride()) {EPETRA_CHK_ERR(-4)}; // Must have strided vectors
  ldlhs = LHS->Stride();
  lhs = (*LHS)[0];
  }
  
  // Finally build ptr vector
  
  if (ptr==0) {
    ptr = new int[M+1];
    ptr[0] = 0;
    for (int i=0; i<M; i++) ptr[i+1] = ptr[i] + Graph.NumMyIndices(i);
  }
  EPETRA_CHK_ERR(ierr);
  return(0);
}
