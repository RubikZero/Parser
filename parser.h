#pragma once
#include <string>
#include <functional>

struct ParserImpl;

class Parser {
public:
	Parser();
	Parser(const std::string& fmt);

	void SetPattern(const std::string& fmt);

	template<typename ...Args>
	bool Parse(const std::string& str, Args && ...args);

	template<typename T>
	void Regist(std::function<T(const std::string&)> func);
private:
	ParserImpl* m_impl;

	template<typename T, typename ...Args>
	bool ParseOnce(int index, T && t, Args && ...args);

	bool ParseOnce(int index);
}; 

#ifndef PASER_STATIC
#include "parser_inl.h"
#endif // !PASER_STATIC

