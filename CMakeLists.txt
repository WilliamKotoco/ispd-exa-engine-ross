PROJECT(ISPD_EXA_ENGINE LANGUAGES CXX C)
SET(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(${ROSS_BINARY_DIR} ./include)
IF(USE_DAMARIS)
    INCLUDE_DIRECTORIES(${DAMARIS_INCLUDE}) 
ENDIF(USE_DAMARIS)
IF(BGPM)
	INCLUDE_DIRECTORIES(${ROSS_SOURCE_DIR} ${BGPM_LIB})
ELSE(NOT(BGPM))
	INCLUDE_DIRECTORIES(${ROSS_SOURCE_DIR})
ENDIF(BGPM)

SET(ispd_srcs
  # Main entry point.
  ./src/main.cpp
  
  # Logging-related files.
  ./src/log/log.cpp
  
  # Model-building and model-loading related files.
  ./src/model/builder.cpp
  ./src/model_loader/model_loader.cpp
  
  # Routing-related files.
  ./src/routing/routing.cpp
  
  # Metric-related files.
  ./src/metrics/metrics.cpp
  
  # Workload-related files.
  ./src/workload/workload.cpp
  ./src/workload/interarrival.cpp
)

# Add the -g flag to the CMAKE_CXX_FLAGS variable
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


ADD_EXECUTABLE(ispd ${ispd_srcs})
ADD_EXECUTABLE(ispd_test ${ispd_srcs})

IF(BGPM)
	TARGET_LINK_LIBRARIES(ispd ROSS imp_bgpm m)
	TARGET_LINK_LIBRARIES(ispd_test ROSS imp_bgpm m)
ELSE(NOT(BGPM))
    IF(USE_DAMARIS)
        TARGET_LINK_LIBRARIES(ispd ROSS ROSS_Damaris m)
        TARGET_LINK_LIBRARIES(ispd_test ROSS ROSS_Damaris m)
    ELSE(NOT(USE_DAMARIS))
        TARGET_LINK_LIBRARIES(ispd ROSS m)
        TARGET_LINK_LIBRARIES(ispd_test ROSS m)
    ENDIF(USE_DAMARIS)
ENDIF(BGPM)

ROSS_TEST_SCHEDULERS(ispd)
ROSS_TEST_INSTRUMENTATION(ispd)

SET_TARGET_PROPERTIES(ispd_test PROPERTIES COMPILE_DEFINITIONS TEST_COMM_ROSS)
ROSS_TEST_SCHEDULERS(ispd_test)
ROSS_TEST_INSTRUMENTATION(ispd_test)

INSTALL(FILES ${ROSS_BINARY_DIR}/../models/ispd/ispd DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
