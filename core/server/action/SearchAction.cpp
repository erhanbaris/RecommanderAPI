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
        returnValue.ContentType = "application/json";

        return returnValue;
    }

    auto searchTerm = web::uri::decode(info->Queries[STR("term")]);
    auto * dataSource = AppServer::instance().DataSource()->Data();

    //todo: use core::getNarrow and split by space to search one by one
    core::clearString(searchTerm);
    auto searchResults = dataSource->symspell.Find(searchTerm);


    size_t page = 1;
    size_t length = 25;


    if (info->Queries.find(STR("page")) != info->Queries.end())
        page = (size_t) stoi(info->Queries[STR("page")]);

    if (info->Queries.find(STR("length")) != info->Queries.end())
        length = (size_t) stoi(info->Queries[STR("length")]);

    STRSTREAM_TYPE stream;

    stream << STR("{");
    stream << STR("\"total\":") << searchResults.size() << ",";
    stream << STR("\"data\": [");
    auto it = searchResults.begin();

    if (searchResults.size() > (page - 1) * length)
    {
        it += (page - 1) * length;

        size_t index = 0;
        auto end = searchResults.end();
        for (; it != end; ++it) {
            if (index > length)
                break;

            ++index;
            stream << STR("{\"Id\":") << it->first << STR(",\"Distance\":") << it->second << STR(",\"Name\":\"") << this->EscapeJsonString(dataSource->productInfos[it->first]) << STR("\"},");
        }

        stream.seekg(0, ios::end);
        int size = stream.tellg();

        if (size > 1)
            stream.seekp(-1, stream.cur);
    }


    stream << STR("]");
    stream<< STR("}");

    ResponseInfo returnValue;
    returnValue.Status = status_codes::OK;
    returnValue.Data = stream.str();
    returnValue.ContentType = "application/json";

    return returnValue;
}

short SearchAction::MethodInfo() {
    return TYPE_GET | TYPE_POST;
}
