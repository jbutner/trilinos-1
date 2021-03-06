INCLUDE(TribitsAddExecutableAndTest)


INCLUDE_DIRECTORIES(REQUIRED_DURING_INSTALLATION_TESTING ${CMAKE_CURRENT_BINARY_DIR}/meritfuncs)
INCLUDE_DIRECTORIES(REQUIRED_DURING_INSTALLATION_TESTING ${CMAKE_CURRENT_SOURCE_DIR}/meritfuncs)


TRIBITS_ADD_EXECUTABLE_AND_TEST(
  VersionUnitTests
  SOURCES
    VersionUnitTests.cpp
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  NUM_MPI_PROCS 1
  STANDARD_PASS_OUTPUT
  )


ADD_SUBDIRECTORY(meritfuncs)


TRIBITS_ADD_EXECUTABLE_AND_TEST(
  ArmijoPolyInterpLineSearch_UnitTests
  SOURCES
    ArmijoPolyInterpLineSearch_UnitTests.cpp
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  DEPLIBS globipack_test_meritfuncs
  NUM_MPI_PROCS 1
  STANDARD_PASS_OUTPUT
  )


TRIBITS_ADD_EXECUTABLE_AND_TEST(
  GoldenQuadInterpBracket_UnitTests
  SOURCES
    GoldenQuadInterpBracket_UnitTests.cpp
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  DEPLIBS globipack_test_meritfuncs
  NUM_MPI_PROCS 1
  STANDARD_PASS_OUTPUT
  )


TRIBITS_ADD_EXECUTABLE_AND_TEST(
  Brents1DMinimization_UnitTests
  SOURCES
    Brents1DMinimization_UnitTests.cpp
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  DEPLIBS globipack_test_meritfuncs
  NUM_MPI_PROCS 1
  STANDARD_PASS_OUTPUT
  )


TRIBITS_ADD_EXECUTABLE_AND_TEST(
  BrentsLineSearch_UnitTests
  SOURCES
    BrentsLineSearch_UnitTests.cpp
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
  DEPLIBS globipack_test_meritfuncs
  NUM_MPI_PROCS 1
  STANDARD_PASS_OUTPUT
  )
