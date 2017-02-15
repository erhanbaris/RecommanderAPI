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
			main.cpp
			model/Product.h
			model/Rating.h
			core/algoritm/SlopeOne.h
			core/algoritm/SlopeOne.cpp
			core/algoritm/PearsonCorrelationCoefficien.h
			core/algoritm/PearsonCorrelationCoefficien.cpp
			core/data/MongoDbDataSource.h
			core/data/BaseDataInfo.h
			core/data/BaseDataInfo.cpp
			core/data/GeneralDataInfo.h
			core/data/GeneralDataInfo.cpp
			core/data/BaseDataSource.h
			core/data/CvsDataSource.h
			core/data/CvsDataSource.cpp
			core/MimeType.h
			core/MimeType.cpp
			core/Utils.h
			core/Utils.cpp
			core/server/handler/BaseHandler.h
			core/server/ServerConfig.h
			core/server/handler/StaticFileHandler.cpp
			core/server/handler/StaticFileHandler.h
			config.h
			core/server/handler/HtmlHandler.cpp
			core/server/handler/HtmlHandler.h
			core/server/handler/ActionHandler.cpp
			core/server/handler/ActionHandler.h
			core/server/AppServer.cpp
			core/server/AppServer.h
			core/server/action/BaseAction.h
			core/server/action/UserRecommendAction.cpp
			core/server/action/UserRecommendAction.h
			core/server/action/SearchAction.cpp
			core/server/action/SearchAction.h
			core/server/action/BaseAction.cpp
			core/server/controller/BaseController.cpp
			core/server/controller/BaseController.h
			core/algoritm/SymSpell.h
			core/algoritm/SymSpell.cpp
			core/data/Index.h
			core/data/Index.cpp)
			
			foreach(source IN LISTS SOURCE_FILES)
				get_filename_component(source_path "${source}" PATH)
				string(REPLACE "/" "\\" source_path_msvc "${source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
			endforeach()

	add_executable(RecommenderAPI ${SOURCE_FILES})

	target_link_libraries(RecommenderAPI
			${Boost_LIBRARIES}
			${CPPRESTSDK_LIB}
			${OPENSSL_LIBRARIES})

endif()