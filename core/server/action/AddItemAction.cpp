#include <core/server/action/AddItemAction.h>
#include <core/server/AppServer.h>
#include <functional>

using namespace core::server::action;

STR_TYPE AddItemAction::Url() {
    return STR("/api/additem");
}

core::server::action::BaseAction* AddItemAction::CreateObject()
{
    return new AddItemAction();
}

core::server::ResponseInfo AddItemAction::Execute(RequestInfo *info) {
    if (info->Queries.find(STR("id")) == info->Queries.end() || info->Queries.find(STR("name")) == info->Queries.end()) {
        web::json::value item = web::json::value::object();
        item[U("Status")] = web::json::value::boolean(false);
        item[U("ErrorMessage")] = web::json::value::string(STR("Operation requeired id and name fields."));

        ResponseInfo returnValue;
        returnValue.Status = status_codes::BadRequest;
		returnValue.Data = item.as_string();
        returnValue.ContentType = "application/json";

        return returnValue;
    }
	
	auto * dataSource = AppServer::instance().DataSource()->Data();

	STR_TYPE productId = web::uri::decode(info->Queries[STR("id")]);
	STR_TYPE productName = web::uri::decode(info->Queries[STR("name")]);

	auto hashFunc = std::hash<STR_TYPE>();
	PRODUCT_TYPE productIdHash = hashFunc(productId);
	auto searchResults = dataSource->symspell.CreateDictionaryEntry(productName, productIdHash);
	dataSource->AddProduct(productIdHash, productId, productName);

    STRSTREAM_TYPE stream;

    stream << STR("{");
    stream << STR("\"status\":true");
    stream<< STR("}");

    ResponseInfo returnValue;
    returnValue.Status = status_codes::OK;
    returnValue.Data = stream.str();
    returnValue.ContentType = "application/json";

    return returnValue;
}

short AddItemAction::MethodInfo() {
    return TYPE_GET | TYPE_POST;
}
