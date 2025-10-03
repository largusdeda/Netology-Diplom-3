#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <vector>

#include "BadIniEx.h"
#include "BadFileEx.h"

class Parser {

private:
	const std::string filename;
	std::vector<std::wstring> lines;
	void cut_spaces(std::wstring& line);

public:
	explicit Parser(const std::string& filename);

	template<typename T>
	T get_value(const std::wstring& sec_var);

};

template<typename T>
T Parser::get_value(const std::wstring& sec_var) {
	std::wstring w_filename(filename.begin(), filename.end());
	T result{};
	std::wstring sec, var;

	size_t pos_dot{ sec_var.find('.') };
	if (pos_dot == std::string::npos) {
		throw BadIniEx(L"Неверный формат аргумента \"" + sec_var + L"\" функции get_value(), наименования секции и переменной должны быть разделены точкой.");
	}
	else {
		sec = sec_var.substr(0, pos_dot);
		var = sec_var.substr(pos_dot + 1, sec_var.length() - pos_dot);

		if (sec.length() == 0 || var.length() == 0) {
			throw BadIniEx(L"Неверный формат аргумента \"" + sec_var + L"\" функции get_value(), не указано наименование секции или переменной. \nКорректный формат: section.variable.");
		}

		//перевод в нижний регистр для возможности регистронезависимого указания наименования секции
		std::transform(sec.begin(), sec.end(), sec.begin(), ::tolower);

		for (auto& c : sec) {
			if (std::ispunct(c) || std::iscntrl(c) || std::isspace(c)) {
				throw BadIniEx(L"В наименовании секции \"" + sec + L"\" аргумента \"" + sec_var + L"\" функции get_value() содержатся недопустимые символы.");
			}
		}

		for (auto& c : var) {
			if (std::ispunct(c) || std::iscntrl(c) || std::isspace(c)) {
				throw BadIniEx(L"В наименовании переменной \"" + var + L"\" аргумента \"" + sec_var + L"\" функции get_value() содержатся недопустимые символы.");
			}
		}
	}

	std::wstring curr_sec, curr_var;
	int line_count{ 0 };
	bool found_sec{ false };
	bool found_var{ false };
	std::set<std::wstring> variables;

	for (auto& line : lines) {
		line_count++;

		//удаление комментариев в строке
		size_t pos_comment{ line.find(L';') };
		if (pos_comment != std::wstring::npos)
			line.erase(pos_comment, line.length());

		//удаление незначащих пробелов в начале и конце наименования секции
		cut_spaces(line);
		if (line == L"")
			continue;

		//поиск символов открытия и закрытия секции []
		size_t pos_open{ line.find(L'[') };
		size_t pos_close{ line.find(L']') };

		//в строке есть хотя бы одна скобка [] - вероятно это заголовок секции
		if (pos_open != std::wstring::npos || pos_close != std::wstring::npos) {

			//найдены обе скобки в строке и они стоят в правильном порядке
			if (pos_open != std::wstring::npos && pos_close != std::wstring::npos && pos_open < pos_close) {
				curr_sec = line.substr(pos_open + 1, pos_close - pos_open - 1);

				//удаление незначащих пробелов в начале и конце наименования секции, т.е. будет допустимо [  section1  ]
				cut_spaces(curr_sec);

				//перевод в нижний регистр для возможности регистронезависимого указания наименования секции
				std::transform(curr_sec.begin(), curr_sec.end(), curr_sec.begin(), ::tolower);
				if (curr_sec == sec)
					found_sec = true; //секция найдена			
				else
					continue;
			}
			//есть только скобка ]
			else if (pos_open != std::wstring::npos && pos_close == std::wstring::npos) {
				throw BadIniEx(L"Синтаксическая ошибка в файле \"" + w_filename + L"\" в строке " + std::to_wstring(line_count) +
					L". Нет закрывающей скобки ].");
			}
			//есть только скобка [
			else if (pos_open == std::wstring::npos && pos_close != std::wstring::npos) {
				throw BadIniEx(L"Синтаксическая ошибка в файле \"" + w_filename + L"\" в строке " + std::to_wstring(line_count) +
					L". Нет открывающей скобки [.");
			}
			//скобки в неправильном порядке ][
			else if (pos_open > pos_close) {
				throw BadIniEx(L"Синтаксическая ошибка в файле \"" + w_filename + L"\" в строке " + std::to_wstring(line_count) +
					L". Скобки ][ в неправильном порядке.");
			}
		}
		//находимся внутри секции
		else if (curr_sec == sec) {
			size_t pos_assign{ line.find(L'=') };
			curr_var = line.substr(0, pos_assign);

			//удаление незначащих пробелов в начале и конце наименования переменной
			cut_spaces(curr_var);

			variables.insert(curr_var);

			if (curr_var == var) {

				//после = ничего не написано или знака = нет в строке с нужной переменной
				if ((curr_var.length() == line.length() - 1) || (pos_assign == std::wstring::npos)) {
					throw BadIniEx(L"Синтаксическая ошибка в файле \"" + w_filename + L"\" в строке " + std::to_wstring(line_count) + L". Не задано значение переменной \"" + curr_var + L"\"");
				}

				std::wstring value{ line.substr(pos_assign + 1, line.length() - pos_assign) };

				//удаление незначащих пробелов в начале и конце значения переменной, предполагается, что пробелы по краям не имеют значения для тектовых переменных
				cut_spaces(value);
				found_var = true;

				//если Т - float, double
				if constexpr (std::is_floating_point<T>::value) {
					try {
						if constexpr (std::is_same<T, float>::value)
							result = std::stof(value);
						else if constexpr (std::is_same<T, double>::value)
							result = std::stod(value);
						else if constexpr (std::is_same<T, long double>::value)
							result = std::stold(value);
					}
					catch (std::invalid_argument) {
						throw BadIniEx(L"Ошибка конвертирования строки \"" + value + L"\" в число c плавающей точкой. invalid_argument");
					}
					catch (std::out_of_range) {
						throw BadIniEx(L"Ошибка конвертирования строки \"" + value + L"\" в число c плавающей точкой. out of range");
					}
				}
				//если Т - int, long, long long, unsigned long, unsigned long long
				else if constexpr (std::is_same<T, int>::value || std::is_same<T, long>::value || std::is_same<T, long long>::value || std::is_same<T, unsigned long>::value || std::is_same<T, unsigned long long>::value) {

					try {
						if constexpr (std::is_same<T, int>::value)
							result = std::stoi(value);
						else if constexpr (std::is_same<T, long>::value)
							result = std::stol(value);
						else if constexpr (std::is_same<T, long long>::value)
							result = std::stoll(value);
						else if constexpr (std::is_same<T, unsigned long>::value)
							result = std::stoul(value);
						else if constexpr (std::is_same<T, unsigned long long>::value)
							result = std::stoull(value);
					}
					catch (std::invalid_argument) {					
						throw BadIniEx(L"Ошибка конвертирования строки \"" + value + L"\" в целое число. invalid_argument");
					}
					catch (std::out_of_range) {
						throw BadIniEx(L"Ошибка конвертирования строки \"" + value + L"\" в целое число. out_of_range");
					}
				}
				else {
					result = line.substr(pos_assign + 1, line.length() - pos_assign);
				}
			}
		}
	}

	if (!found_sec) {
		throw BadIniEx(L"Искомая секция \"" + sec + L"\" не найдена.");
	}
	else if (!found_var) {
		std::wstringstream message{};
		message << L"Искомая переменная \"" << var << L"\" не найдена в секции \"" << sec << L"\".\nНайдены следующие переменные:" << std::endl;
		for (const auto& var : variables) {
			message << var << std::endl;
		}
		throw BadIniEx(message.str());
	}

	return result;
}