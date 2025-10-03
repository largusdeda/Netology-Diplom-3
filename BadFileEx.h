#pragma once

#include <exception>
#include <string>

class BadFileEx : public std::exception {

private:
    std::wstring message;

public:
    BadFileEx(const std::wstring& msg);

    const std::wstring& getMessage() const;
    const char* what() const noexcept override;

};