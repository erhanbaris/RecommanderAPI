#pragma once

#include <stdio.h>
#include <iostream>
#include <string>

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/producerconsumerstream.h>
#include <cpprest/json.h>
#include <cpprest/interopstream.h>

using namespace std;
using namespace std::chrono;

using namespace utility;
using namespace concurrency::streams;
using namespace web::http;

using namespace web;
using namespace web::http;
using namespace web::http::client;

namespace core
{
    namespace server {
        typedef struct tResponseInfo {
            int Status;
            STR_TYPE Data;
			STR_TYPE ContentType = STR("text/plain; charset=utf-8");
        } ResponseInfo;

        typedef struct tRequestInfo {
            tRequestInfo(std::map<STR_TYPE, STR_TYPE> &pQueries, STR_TYPE &pUrl)
                    : Queries(pQueries), Url(pUrl) { }

			STR_TYPE MethodType;
            std::map<STR_TYPE, STR_TYPE> Queries;
			STR_TYPE Url;
            ResponseInfo Response;
        } RequestInfo;
    }
}