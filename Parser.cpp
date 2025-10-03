#include "Parser.h"

Parser::Parser(const std::string& filename) : filename(filename) {
	std::wifstream file(filename);
    if (!file.is_open()) {
        throw BadFileEx(L"Не удалось открыть файл.");
    }

	std::wstring line;
	file.imbue(std::locale("rus_RUS.UTF8"));
	while (std::getline(file, line)) {
		lines.push_back(line);
	}

	file.close();
}

void Parser::cut_spaces(std::wstring& line) {
	//удаление пробелов до первого символа
	size_t pos_first_sym{ line.find_first_not_of(L" \t", 0) };
	if (pos_first_sym != std::wstring::npos)
		line.erase(0, pos_first_sym);

	//удаление пробелов после последнего символа
	size_t pos_last_sym{ line.find_last_not_of(L" \t", line.length() - 1) };
	if (pos_last_sym != std::wstring::npos)
		line.erase(pos_last_sym + 1, line.length());

	//в строке только пробелы
	if (line.length() > 0 && pos_first_sym == std::wstring::npos && pos_last_sym == std::wstring::npos)
		line.clear();
}