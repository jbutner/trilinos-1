#include "Epetra_BlockMap.h"
#include "Epetra_Map.h"
#include "Epetra_LocalMap.h"
#include "Epetra_FEVector.h"
#include "../epetra_test_err.h"

int MatrixTests(const Epetra_BlockMap & map, const Epetra_LocalMap & LocalMap, int NumVectors,
		    bool verbose);

int MultiVectorTests(const Epetra_BlockMap & Map, int NumVectors, bool verbose);

