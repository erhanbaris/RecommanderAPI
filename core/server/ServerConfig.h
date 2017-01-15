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
        extern string StaticFolder;
        extern string HtmlFolder;


        typedef struct tResponseInfo {
            int Status;
            string Data;
            string ContentType = "text/plain; charset=utf-8";
        } ResponseInfo;

        typedef struct tRequestInfo {
            tRequestInfo(std::map<utility::string_t, utility::string_t> pQueries, string &pUrl)
                    : Queries(pQueries), Url(pUrl) { }

            string MethodType;
            std::map<utility::string_t, utility::string_t> Queries;
            string Url;
            ResponseInfo Response;
        } RequestInfo;
    }
}