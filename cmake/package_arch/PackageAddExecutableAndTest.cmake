
INCLUDE(PackageAddExecutable)
INCLUDE(PackageAddTest)


FUNCTION(PACKAGE_ADD_EXECUTABLE_AND_TEST EXE_NAME)
   
  #
  # A) Parse the input arguments
  #

  PARSE_ARGUMENTS(
     #prefix
     PARSE
     #lists
     "SOURCES;DEPLIBS;NUM_MPI_PROCS;DIRECTORY;KEYWORDS;COMM;ARGS;NAME;PASS_REGULAR_EXPRESSION;HOST;XHOST;HOSTTYPE;XHOSTTYPE;FAIL_REGULAR_EXPRESSION;LINKER_LANGUAGE"
     #options
     "STANDARD_PASS_OUTPUT;ADD_DIR_TO_NAME"
     ${ARGN}
     )

  IF(${PROJECT_NAME}_VERBOSE_CONFIGURE)
    MESSAGE("")
    MESSAGE("PACKAGE_ADD_EXECUTABLE_AND_TEST: ${EXE_NAME}")
  ENDIF()

  #
  # B) PackageAddExecutable(...)
  #

  SET(CALL_ARGS "")
  IF (PARSE_SOURCES)
    SET(CALL_ARGS ${CALL_ARGS} SOURCES ${PARSE_SOURCES}) 
  ENDIF()
  IF (PARSE_DEPLIBS)
    SET(CALL_ARGS ${CALL_ARGS} DEPLIBS ${PARSE_DEPLIBS}) 
  ENDIF()
  IF (PARSE_DIRECTORY)
    SET(CALL_ARGS ${CALL_ARGS} DIRECTORY ${PARSE_DIRECTORY}) 
  ENDIF()
  IF (PARSE_COMM)
    SET(CALL_ARGS ${CALL_ARGS} COMM ${PARSE_COMM}) 
  ENDIF()
  IF (PARSE_ADD_DIR_TO_NAME)
    SET(CALL_ARGS ${CALL_ARGS} ADD_DIR_TO_NAME) 
  ENDIF()
  IF (PARSE_LINKER_LANGUAGE)
    SET(CALL_ARGS ${CALL_ARGS} LINKER_LANGUAGE ${PARSE_LINKER_LANGUAGE}) 
  ENDIF()

  PACKAGE_ADD_EXECUTABLE(${EXE_NAME} ${CALL_ARGS})

  #
  # C) PackageAddTest(...)
  #

  SET(CALL_ARGS "")
  IF (PARSE_DIRECTORY)
    SET(CALL_ARGS ${CALL_ARGS} DIRECTORY ${PARSE_DIRECTORY}) 
  ENDIF()
  IF (PARSE_KEYWORDS)
    SET(CALL_ARGS ${CALL_ARGS} KEYWORDS ${PARSE_KEYWORDS}) 
  ENDIF()
  IF (PARSE_COMM)
    SET(CALL_ARGS ${CALL_ARGS} COMM ${PARSE_COMM}) 
  ENDIF()
  IF (PARSE_NUM_MPI_PROCS)
    SET(CALL_ARGS ${CALL_ARGS} NUM_MPI_PROCS ${PARSE_NUM_MPI_PROCS}) 
  ENDIF()
  IF (PARSE_ARGS)
    SET(CALL_ARGS ${CALL_ARGS} ARGS ${PARSE_ARGS}) 
  ENDIF()
  IF (PARSE_HOST)
    SET(CALL_ARGS ${CALL_ARGS} HOST ${PARSE_HOST}) 
  ENDIF()
  IF (PARSE_XHOST)
    SET(CALL_ARGS ${CALL_ARGS} XHOST ${PARSE_XHOST}) 
  ENDIF()
  IF (PARSE_HOSTTYPE)
    SET(CALL_ARGS ${CALL_ARGS} HOSTTYPE ${PARSE_HOSTTYPE}) 
  ENDIF()
  IF (PARSE_XHOSTTYPE)
    SET(CALL_ARGS ${CALL_ARGS} XHOSTTYPE ${PARSE_XHOSTTYPE}) 
  ENDIF()
  IF (PARSE_PASS_REGULAR_EXPRESSION)
    SET(CALL_ARGS ${CALL_ARGS} PASS_REGULAR_EXPRESSION ${PARSE_PASS_REGULAR_EXPRESSION}) 
  ENDIF()
  IF (PARSE_FAIL_REGULAR_EXPRESSION)
    SET(CALL_ARGS ${CALL_ARGS} FAIL_REGULAR_EXPRESSION ${PARSE_FAIL_REGULAR_EXPRESSION}) 
  ENDIF()
  IF (PARSE_STANDARD_PASS_OUTPUT)
    SET(CALL_ARGS ${CALL_ARGS} STANDARD_PASS_OUTPUT)
  ENDIF()
  IF (PARSE_ADD_DIR_TO_NAME)
    SET(CALL_ARGS ${CALL_ARGS} ADD_DIR_TO_NAME) 
  ENDIF()

  PACKAGE_ADD_TEST(${EXE_NAME} ${CALL_ARGS})

ENDFUNCTION()