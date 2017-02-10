cmake_minimum_required(VERSION 3.2)
project(RecommenderAPI)

find_package( Boost 1.50 COMPONENTS system chrono thread REQUIRED )
find_package( OpenSSL COMPONENTS crypto REQUIRED )
find_path(CPPRESTSDK_INCLUDE_DIR NAMES cpprest/http_listener.h)
find_library(CPPRESTSDK_LIB NAMES libcpprest cpprest)


if(Boost_FOUND)
	
	set(Boost_USE_STATIC_LIBS        ON) # only find static libs
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME    OFF)

	find_path(sparsehash_INCLUDE_DIR PATH_SUFFIXES sparse_hash_map )
	include_directories(${Boost_INCLUDE_DIR})
	link_directories(${Boost_LIBRARY_DIR})
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

	add_definitions(-DBOOST_ALL_NO_LIB)
	include_directories(${PROJECT_SOURCE_DIR})
	include_directories(${sparsehash_INCLUDE_DIR})
	include_directories(${CPPRESTSDK_INCLUDE_DIR})
	include_directories(${OPENSSL_INCLUDE_DIR})

	set(SOURCE_FILES
			${PROJECT_SOURCE_DIR}/main.cpp
			${PROJECT_SOURCE_DIR}/model/Product.h
			${PROJECT_SOURCE_DIR}/model/Rating.h
			${PROJECT_SOURCE_DIR}/core/algoritm/SlopeOne.h
			${PROJECT_SOURCE_DIR}/core/algoritm/SlopeOne.cpp
			${PROJECT_SOURCE_DIR}/core/algoritm/PearsonCorrelationCoefficien.h
			${PROJECT_SOURCE_DIR}/core/algoritm/PearsonCorrelationCoefficien.cpp
			${PROJECT_SOURCE_DIR}/core/data/MongoDbDataSource.h
			${PROJECT_SOURCE_DIR}/core/data/BaseDataInfo.h
			${PROJECT_SOURCE_DIR}/core/data/BaseDataInfo.cpp
			${PROJECT_SOURCE_DIR}/core/data/GeneralDataInfo.h
			${PROJECT_SOURCE_DIR}/core/data/GeneralDataInfo.cpp
			${PROJECT_SOURCE_DIR}/core/data/BaseDataSource.h
			${PROJECT_SOURCE_DIR}/core/data/CvsDataSource.h
			${PROJECT_SOURCE_DIR}/core/data/CvsDataSource.cpp
			${PROJECT_SOURCE_DIR}/core/MimeType.h
			${PROJECT_SOURCE_DIR}/core/MimeType.cpp
			${PROJECT_SOURCE_DIR}/core/Utils.h
			${PROJECT_SOURCE_DIR}/core/Utils.cpp
			${PROJECT_SOURCE_DIR}/core/server/handler/BaseHandler.h
			${PROJECT_SOURCE_DIR}/core/server/ServerConfig.h
			${PROJECT_SOURCE_DIR}/core/server/handler/StaticFileHandler.cpp
			${PROJECT_SOURCE_DIR}/core/server/handler/StaticFileHandler.h
			${PROJECT_SOURCE_DIR}/config.h
			${PROJECT_SOURCE_DIR}/core/server/handler/HtmlHandler.cpp
			${PROJECT_SOURCE_DIR}/core/server/handler/HtmlHandler.h
			${PROJECT_SOURCE_DIR}/core/server/handler/ActionHandler.cpp
			${PROJECT_SOURCE_DIR}/core/server/handler/ActionHandler.h
			${PROJECT_SOURCE_DIR}/core/server/AppServer.cpp
			${PROJECT_SOURCE_DIR}/core/server/AppServer.h
			${PROJECT_SOURCE_DIR}/core/server/action/BaseAction.h
			${PROJECT_SOURCE_DIR}/core/server/action/UserRecommendAction.cpp
			${PROJECT_SOURCE_DIR}/core/server/action/UserRecommendAction.h
			${PROJECT_SOURCE_DIR}/core/server/action/SearchAction.cpp
			${PROJECT_SOURCE_DIR}/core/server/action/SearchAction.h
			${PROJECT_SOURCE_DIR}/core/server/action/BaseAction.cpp
			${PROJECT_SOURCE_DIR}/core/server/controller/BaseController.cpp
			${PROJECT_SOURCE_DIR}/core/server/controller/BaseController.h
			${PROJECT_SOURCE_DIR}/core/algoritm/SymSpell.h
			${PROJECT_SOURCE_DIR}/core/algoritm/SymSpell.cpp
			)
	add_executable(RecommenderAPI ${SOURCE_FILES})

	target_link_libraries(RecommenderAPI
			${Boost_LIBRARIES}
			${CPPRESTSDK_LIB}
			${OPENSSL_LIBRARIES})

endif()