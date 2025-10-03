#include <iostream>
#include <exception>
#include <locale>
#include <codecvt>

#include "Parser.h"

int main() {
	std::wcout.imbue(std::locale("rus_rus.866"));

	try {
		Parser parser("ini-file.ini");

		std::wcout << std::fixed << parser.get_value<double>(L"section1.var1") << std::endl;
		std::wcout << std::fixed << parser.get_value<std::wstring>(L"section1.var2") << std::endl;
		//std::wcout << std::fixed << parser.get_value<double>(L"section1.var2") << std::endl;
		//std::wcout << std::fixed << parser.get_value<double>(L"section1.var4") << std::endl;
		//std::wcout << std::fixed << parser.get_value<int>(L"section3.var1") << std::endl;
		std::wcout << std::fixed << parser.get_value<double>(L"section4.Vid") << std::endl;
	}
	catch (const BadIniEx& e) {
		std::wcout << e.getMessage();
	}
	catch (const BadFileEx& e) {
		std::wcout << e.getMessage();
	}

	return EXIT_SUCCESS;
}