#include <core/server/action/BaseAction.h>

using namespace core::server::action;

STR_TYPE BaseAction::EscapeJsonString(const STR_TYPE &input) {
    OSTRSTREAM_TYPE ss;
    auto inputEnd = input.cend();
    for (auto iter = input.cbegin(); iter != inputEnd; ++iter) {
        //C++98/03:
        //for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
        switch (*iter) {
            case '\\':
                ss << STR("\\\\");
                break;
            case '"':
                ss << STR("\\\"");
                break;
            case '/':
                ss << STR("\\/");
                break;
            case '\b':
                ss << STR("\\b");
                break;
            case '\f':
                ss << STR("\\f");
                break;
            case '\n':
                ss << STR("\\n");
                break;
            case '\r':
                ss << STR("\\r");
                break;
            case '\t':
                ss << STR("\\t");
                break;
            default:
                ss << *iter;
                break;
        }
    }
    return ss.str();
}


BaseAction::~BaseAction() {};