cmake_minimum_required(VERSION 3.2)
project(RecommanderAPI)

message(STATUS "Merhaba dünya")

find_package( Boost 1.55 COMPONENTS system REQUIRED )
find_path(CPPRESTSDK_INCLUDE_DIR NAMES cpprest/http_listener.h)
find_library(CPPRESTSDK_LIB NAMES libcpprest cpprest)

if(Boost_FOUND)
	
	set(Boost_USE_STATIC_LIBS        ON) # only find static libs
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME    OFF)

	find_path(sparsehash_INCLUDE_DIR PATH_SUFFIXES sparsehash )
	include_directories(${Boost_INCLUDE_DIR})
	link_directories(${Boost_LIBRARY_DIR})
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
	
	add_definitions(-DBOOST_ALL_NO_LIB)
	include_directories(${PROJECT_SOURCE_DIR}/include)
	include_directories(sparsehash_INCLUDE_DIR)
	include_directories(CPPRESTSDK_INCLUDE_DIR)

	set(SOURCE_FILES
			main.cpp
			${PROJECT_SOURCE_DIR}/model/Product.h
			${PROJECT_SOURCE_DIR}/model/Rating.h
			${PROJECT_SOURCE_DIR}/core/algoritm/SlopeOne.h
			${PROJECT_SOURCE_DIR}/core/algoritm/PearsonCorrelationCoefficien.h
			${PROJECT_SOURCE_DIR}/core/data/MongoDbDataSource.h
			${PROJECT_SOURCE_DIR}/core/data/BaseDataInfo.h
			${PROJECT_SOURCE_DIR}/core/data/BaseDataSource.h
			${PROJECT_SOURCE_DIR}/core/data/CvsDataSource.h
			${PROJECT_SOURCE_DIR}/core/types.h
			${PROJECT_SOURCE_DIR}/config.h

			)
	add_executable(RecommanderAPI ${SOURCE_FILES})
	target_link_libraries(RecommanderAPI ${Boost_LIBRARIES} ${CPPRESTSDK_LIB})

endif()