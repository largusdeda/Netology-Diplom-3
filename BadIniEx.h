#pragma once

#include <exception>
#include <string>

class BadIniEx : public std::exception {

private:
	std::wstring message;

public:
    BadIniEx(const std::wstring& msg);

    const std::wstring& getMessage() const;
    const char* what() const noexcept override;

};