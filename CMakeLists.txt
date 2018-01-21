cmake_minimum_required(VERSION 3.5)
project(guardius)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_COMPILER g++)

#Set lib directories
#Darknet-cpp-sharedlib include
set(DARKNET_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/darknet")
message(DARKNET_INCLUDE_DIR: ${DARKNET_INCLUDE_DIR})

include_directories(include)
include_directories("${DARKNET_INCLUDE_DIR}")

#CUDA
FIND_PACKAGE(CUDA)
if (CUDA_FOUND)
    find_package(CUDA REQUIRED)
    message(STATUS "CUDA Version: ${CUDA_VERSION_STRINGS}")
    message(STATUS "CUDA Libararies: ${CUDA_LIBRARIES}")
    set(
            CUDA_NVCC_FLAGS
            ${CUDA_NVCC_FLAGS};
            -O3
            -gencode arch=compute_30,code=[sm_30,compute_30] #For GTX 7XX
    )
    add_definitions(-DGPU)
else()
    list(APPEND LIBRARIES "m")
    message(STATUS "No CUDA")
endif()

#OpenCV
find_package(OpenCV REQUIRED)
# Enable OPENCV in darknet
add_definitions(-DOPENCV)
add_definitions(-O3 -g)
include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if(CMAKE_VERSION VERSION_LESS "2.8.11")
    # Add OpenCV headers location to your include paths
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

#restclient-cpp
include_directories(/usr/local/Cellar/restclient-cpp/0.4.4/include/restclient-cpp/)
set(RESTCLIENT_LIB     "/usr/local/Cellar/restclient-cpp/0.4.4/lib/librestclient-cpp.dylib")

#Add source for CUDA or not
if (CUDA_FOUND)

    if (${APPLE})
        link_directories(
                /usr/local/cuda/lib #mac
        )
        set(DARKNET_LIB     "${CMAKE_SOURCE_DIR}/lib/libdarknet-cpp-shared-mac.so")
        message(DARKNET_LIB: ${DARKNET_LIB})

    else()
        link_directories(
                /usr/local/cuda/lib64 #linux
        )
        set(DARKNET_LIB     "${CMAKE_SOURCE_DIR}/lib/libdarknet-cpp-shared-ubuntu-cuda.so")
        message(DARKNET_LIB: ${DARKNET_LIB})

    endif ()

    cuda_add_executable(${PROJECT_NAME}
            arapaho/arapaho.cpp
            arapaho/arapaho.hpp

            src/entity.cpp
            src/Vector2D.cpp

            src/main.cpp
            )

    target_link_libraries(${PROJECT_NAME}
            cuda
            cudart
            cublas
            curand
            )

else()

    if (${APPLE})
        #?!?
    else()
        set(DARKNET_LIB     "${CMAKE_SOURCE_DIR}/lib/libdarknet-cpp-shared-ubuntu-sans-cuda.so")
        message(DARKNET_LIB: ${DARKNET_LIB})

    endif ()

    #Darknet source
    add_executable(${PROJECT_NAME}
            arapaho/arapaho.cpp
            arapaho/arapaho.hpp

            src/entity.cpp
            src/Vector2D.cpp

            src/main.cpp
            )

endif()

#Link general libs
target_link_libraries(${PROJECT_NAME}
        m
        pthread
        stdc++
        ${OpenCV_LIBS}
        ${DARKNET_LIB}
        ${RESTCLIENT_LIB}
        )

#Copy lib
configure_file(${DARKNET_LIB} libdarknet-cpp-shared.so COPYONLY)

#Copy
#"input/input.data";
#"input/input.cfg";
#"input/input.weights";
#"input/input.mp4";
#configure_file(input/input.data input/input.data COPYONLY)
#configure_file(input/input.cfg input/input.cfg COPYONLY)
#configure_file(input/input.weights input/input.weights COPYONLY)
#configure_file(input/input.names input/input.names COPYONLY)
#configure_file(input/input.mp4 input/input.mp4 COPYONLY)
