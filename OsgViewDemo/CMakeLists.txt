cmake_minimum_required (VERSION 3.8)

set (PROJECT_NAME "OsgViewDemo")
project (${PROJECT_NAME})

set(OSG_DIR ${CMAKE_SOURCE_DIR}/ThirdPart/OpenSceneGraph-3.6.5-VC2019-64)
message("OSG_DIR ${OSG_DIR}")
include_directories(${OSG_DIR}/include)
link_directories(${OSG_DIR}/lib)

link_directories(${LIBRARY_OUTPUT_PATH})
include_directories(${CMAKE_SOURCE_DIR}/OccCore7.7.0/inc)

set_property(GLOBAL PROPERTY OccViewList)
dirlist(OccViewList ${CMAKE_CURRENT_SOURCE_DIR}/src "src")
get_property(SRCLIST GLOBAL PROPERTY OccViewList)
 
add_executable(${PROJECT_NAME} ${SRCLIST})

target_link_libraries(${PROJECT_NAME} debug OpenThreadsd)
target_link_libraries(${PROJECT_NAME} debug osgd)
target_link_libraries(${PROJECT_NAME} debug osgViewerd)
target_link_libraries(${PROJECT_NAME} debug osgDBd)
target_link_libraries(${PROJECT_NAME} debug osgGAd)
target_link_libraries(${PROJECT_NAME} debug osgUtild)

message("OCC_LIB ${OCC_LIB}")
target_link_libraries(${PROJECT_NAME} debug ${OCC_LIB})

set_target_properties(${PROJECT_NAME}  PROPERTIES  
    VS_DEBUGGER_ENVIRONMENT  "PATH=${OSG_DIR}/bin/$(Configuration);${LIBRARY_OUTPUT_PATH}/$(Configuration);%PATH%"
)