#include "TSFMPI.h"
#include "TSFError.h"

using namespace TSF;

int TSFMPI::rank_ = 0 ;
int TSFMPI::nProc_ = 1 ;

void TSFMPI::init(int* argc, void*** argv)
{
#ifdef HAVE_MPI
	/* initialize MPI */
	int mpiHasBeenStarted = 0;
	MPI_Initialized(& mpiHasBeenStarted);
	int mpierr = 0 ;
	if (!mpiHasBeenStarted)
		{
			mpierr = ::MPI_Init (argc, (char ***) argv);
			if (mpierr != 0)
				{
					TSFError::raise("Error detected in MPI_Init()");
				}
		}
	
	/* find rank */
	mpierr = ::MPI_Comm_rank (MPI_COMM_WORLD, &rank_);
	if (mpierr != 0)
		{
			TSFError::raise("Error detected in MPI_Comm_rank()");
		}

	/* find number of procs */
	mpierr = ::MPI_Comm_size (MPI_COMM_WORLD, &nProc_);
	if (mpierr != 0)
		{
			TSFError::raise("Error detected in MPI_Comm_size()");
		}
#endif
}

void TSFMPI::finalize()
{
#ifdef HAVE_MPI
	int mpierr = ::MPI_Finalize();
	if (mpierr != 0)
		{
			TSFError::raise("Error detected in MPI_Finalize()");
		}
#endif
}
