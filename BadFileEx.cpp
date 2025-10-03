#include "BadFileEx.h"

BadFileEx::BadFileEx(const std::wstring& msg) : message(msg) {}

const std::wstring& BadFileEx::getMessage() const {
    return message;
}

const char* BadFileEx::what() const noexcept {
    return "Can not open file";
}