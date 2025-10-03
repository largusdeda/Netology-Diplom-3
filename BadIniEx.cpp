#include "BadIniEx.h"

BadIniEx::BadIniEx(const std::wstring& msg) : message(msg) {}

const std::wstring& BadIniEx::getMessage() const {
    return message;
}

const char* BadIniEx::what() const noexcept {
    return "Custom exception occurred in ini-file.";
}