#include <core/server/action/BaseAction.h>

using namespace core::server::action;

std::string BaseAction::EscapeJsonString(const std::string &input) {
    std::ostringstream ss;
    auto inputEnd = input.cend();
    for (auto iter = input.cbegin(); iter != inputEnd; ++iter) {
        //C++98/03:
        //for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
        switch (*iter) {
            case '\\':
                ss << "\\\\";
                break;
            case '"':
                ss << "\\\"";
                break;
            case '/':
                ss << "\\/";
                break;
            case '\b':
                ss << "\\b";
                break;
            case '\f':
                ss << "\\f";
                break;
            case '\n':
                ss << "\\n";
                break;
            case '\r':
                ss << "\\r";
                break;
            case '\t':
                ss << "\\t";
                break;
            default:
                ss << *iter;
                break;
        }
    }
    return ss.str();
}


BaseAction::~BaseAction() {};