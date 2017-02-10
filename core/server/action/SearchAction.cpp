#include <core/server/action/SearchAction.h>
#include <core/server/AppServer.h>

using namespace core::server::action;

STR_TYPE SearchAction::Url() {
    return STR("/api/search");
}

core::server::action::BaseAction* SearchAction::CreateObject()
{
    return new SearchAction();
}

core::server::ResponseInfo SearchAction::Execute(RequestInfo *info) {
    if (info->Queries.find(STR("term")) == info->Queries.end()) {
        web::json::value item = web::json::value::object();
        item[U("Status")] = web::json::value::boolean(false);
        item[U("ErrorMessage")] = web::json::value::string(STR("Please set 'term' parameter"));

        ResponseInfo returnValue;
        returnValue.Status = status_codes::BadRequest;
        returnValue.Data = STR("");
        returnValue.ContentType = STR("application/json");

        return returnValue;
    }

    auto searchTerm = web::uri::decode(info->Queries[STR("term")]);
    auto * dataSource = AppServer::instance().DataSource()->Data();

    //todo: use core::getNarrow and split by space to search one by one
    auto searchResults = dataSource->symspell.Find(searchTerm);

    STRSTREAM_TYPE stream;
    stream << STR("[");

    auto end = searchResults.end();
    for (auto it = searchResults.begin(); it != end; ++it) {
        stream << STR("{\"Id\":") << it->first << STR(",\"Name\":\"") << this->EscapeJsonString(dataSource->productInfos[it->second.productId]) << STR("\"},");
    }

    stream.seekg(0, ios::end);
    int size = stream.tellg();

    if (size > 1)
        stream.seekp(-1, stream.cur);

    stream << STR("]");

    ResponseInfo returnValue;
    returnValue.Status = status_codes::OK;
    returnValue.Data = stream.str();
    returnValue.ContentType = STR("application/json");

    return returnValue;
}

short SearchAction::MethodInfo() {
    return TYPE_GET | TYPE_POST;
}
