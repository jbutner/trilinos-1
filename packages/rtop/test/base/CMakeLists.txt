INCLUDE(TribitsAddExecutableAndTest)


TRIBITS_ADD_EXECUTABLE_AND_TEST(
  RTOpPack_Types_test
  SOURCES RTOpPack_Types_test.cpp
  ARGS
    "--echo-command-line"
    "--echo-command-line --sub-dim=3"
    "--echo-command-line --sub-dim=2"
    "--echo-command-line --sub-dim=4 --stride=-1"
    "--echo-command-line --sub-dim=4 --stride=-2"
    "--echo-command-line --sub-dim=4 --stride=-4"
    "--echo-command-line --sub-dim=2 --leading-dim=3"
    "--echo-command-line --sub-dim=2 --leading-dim=4"
    "--echo-command-line --sub-dim=2 --leading-dim=5"
    "--echo-command-line --num-cols=1"
    "--echo-command-line --num-cols=3"
  COMM serial mpi
  NUM_MPI_PROCS 1
  STANDARD_PASS_OUTPUT
  )


TRIBITS_ADD_EXECUTABLE_AND_TEST(
  baseUnitTests
  SOURCES
    PrimitiveTypeTraits_UnitTests.cpp
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  COMM serial mpi  
  NUM_MPI_PROCS 1
  STANDARD_PASS_OUTPUT
  )
