#include <iostream>
#include <fstream>

#pragma warning(disable:4503)

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#define GetCurrentDir _getcwd
#else

#include <unistd.h>

#define GetCurrentDir getcwd
#endif

#include <config.h>
#include <sstream>
#include <map>

#include <core/server/AppServer.h>
#include <core/server/action/UserRecommendAction.h>
#include <core/server/action/SearchAction.h>
#include <core/server/action/AddItemAction.h>
#include <core/data/impl/BlockStorage.h>
#include <core/data/impl/RecordStorage.h>
#include <core/data/impl/Block.h>

#pragma execution_character_set("utf-8")

using namespace std;
using namespace std::chrono;
using namespace model;
using namespace core::algoritm;
using namespace core::server;
using namespace core::server::handler;
using namespace core::server::action;

using namespace utility;
using namespace concurrency::streams;
using namespace web::http;

using namespace web;
using namespace web::http;
using namespace web::http::client;

ResponseInfo refreshDataSource(RequestInfo *info) {
    //dataSource->LoadData();

    ResponseInfo returnValue;
    returnValue.Status = status_codes::OK;
    returnValue.Data = STR("Loaded All Data");

    return returnValue;
}



template<typename T>
void float_to_bytes(T f, unsigned char *& result, int & size) {

	result = new unsigned char[sizeof(T)];
	size = sizeof(T);

	typedef unsigned char float_byte_array[sizeof(T)];
	union {
		T x;
		float_byte_array res;
	} u;
	u.x = f;
	std::copy(u.res, u.res + sizeof(T), result);
}

template<typename T>
void bytes_to_int(T & f, unsigned char * result, int size) {
	typedef unsigned char float_byte_array[sizeof(T)];
	union {
		int x;
		float_byte_array res;
	} u;

	std::copy(result, result + sizeof(T), u.res);
	f = u.x;
}

int main(int argc, char **args) {
    char currentPathChars[FILENAME_MAX];
    if (!GetCurrentDir(currentPathChars, sizeof(currentPathChars)))
        return errno;

    currentPathChars[sizeof(currentPathChars) - 1] = '\0';
    string currentPath(currentPathChars);
    string dataPath;

    if (argc > 1)
        dataPath = args[1];
    else
        dataPath = currentPath;

	core::data::impl::BlockStorage * storage = new core::data::impl::BlockStorage(currentPath + "/" + "data.edb");
    core::data::impl::Block * block = NULL;

    if (storage->HasBlock(0))
        block = storage->Find(0);
    else
        block = storage->Create();


	size_t number = 19239392;
	size_t number2 = 0;
	unsigned char * items = new unsigned char[sizeof(size_t)]{ 0 };
	int itemsLen = 0;
	float_to_bytes(number, items, itemsLen);

	bytes_to_int(number2, items, itemsLen);

	char data2[sizeof(int)];
	*reinterpret_cast<int*>(data2) = 19239392;

	size_t asdasd;

    auto headerItem = block->GetHeader(1);
    size_t id = block->Id();
    block->SetHeader(1, 19239392);
	headerItem = block->GetHeader(1);
    auto * text = "hello world";
	block->Write(text, strlen(text), 0, 0, strlen(text));
	block->Flush();

	char * data = new char[12]{ 0 };
    block->Read(data, 12, 0, 0, 12);

    core::data::impl::RecordStorage recordStorage(storage);

    try {
        AppServer::instance()
                .AddAction(new UserRecommendAction())
                .AddAction(new SearchAction())
                .SetExecutionPath(currentPath)
                .SetDataPath(dataPath)
                .SetHtmlPath("www")
                .SetStaticPath("www/static");
        AppServer::instance().Start();
    }
    catch (std::exception &e) {
        ERROR_WRITE(e.what());

    }

    getchar();
    LOG_WRITE(STR("RECOMMENDER EXIT"));
    return 0;
}

