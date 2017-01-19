#include <core/server/action/SearchAction.h>
#include <core/server/AppServer.h>

using namespace core::server::action;

string SearchAction::Url() {
    return "/api/search";
}

core::server::action::BaseAction* SearchAction::CreateObject()
{
    return new SearchAction();
}

core::server::ResponseInfo SearchAction::Execute(RequestInfo *info) {
    if (info->Queries.find("term") == info->Queries.end()) {
        web::json::value item = web::json::value::object();
        item[U("Status")] = web::json::value::boolean(false);
        item[U("ErrorMessage")] = web::json::value::string("Please set 'term' parameter");

        ResponseInfo returnValue;
        returnValue.Status = status_codes::BadRequest;
        returnValue.Data = "";
        returnValue.ContentType = "application/json";

        return returnValue;
    }

    auto searchTerm = web::uri::decode(info->Queries["term"]);
    auto * dataSource = AppServer::instance().DataSource()->Data();

    //todo: use core::getNarrow and split by space to search one by one
    auto searchResults = dataSource->symspell.Find(searchTerm);

    std::stringstream stream;
    stream << "[";

    auto end = searchResults.end();
    for (auto it = searchResults.begin(); it != end; ++it) {
        stream << "{\"Id\":" << it->first << ",\"Name\":\"" << this->EscapeJsonString(core::getString(dataSource->productInfos[it->second.productId])) << "\"},";
    }

    stream.seekg(0, ios::end);
    int size = stream.tellg();

    if (size > 1)
        stream.seekp(-1, stream.cur);

    stream << "]";

    ResponseInfo returnValue;
    returnValue.Status = status_codes::OK;
    returnValue.Data = stream.str();
    returnValue.ContentType = "application/json";

    return returnValue;
}

short SearchAction::MethodInfo() {
    return TYPE_GET | TYPE_POST;
}
