#
# Build all Secondary Stable CASL VRI add-on Trilnos packages with GCC 4.5.1 compiler
#

INCLUDE("${CTEST_SCRIPT_DIRECTORY}/hybrid-build-12.cmake")
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/TribitsCTestDriverCore.pu241.gcc.4.5.1.ifort12.cmake")
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/SubmitToCaslDev.cmake")
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/casl-vri-packages-coupled.cmake")
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/enable_casl_packages.gcc.serial.cmake")

SET(COMM_TYPE SERIAL)
SET(BUILD_TYPE RELEASE)
SET(BUILD_DIR_NAME SERIAL_RELEASE_HYBRID12_CASLDEV)
SET(CTEST_TEST_TYPE Experimental)
#SET(CTEST_TEST_TIMEOUT 900)
SET(Trilinos_ENABLE_SECONDARY_STABLE_CODE ON)
SET(EXTRA_CONFIGURE_OPTIONS
  ${EXTRA_CONFIGURE_OPTIONS}
  -DDISABLE_ANCVIPREBOA_MILESTONE_REGRESSION_TEST:BOOL=ON
  )

TRILINOS_SYSTEM_SPECIFIC_CTEST_DRIVER()
