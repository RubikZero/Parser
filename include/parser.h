#pragma once
#include <string>
#include <functional>

struct ParserImpl;

struct ParserPlaceHolder
{

};

class Parser {
public:

	Parser();
	Parser(const std::string& fmt);
	Parser(const Parser& parser);
	Parser& operator=(const Parser& parser);
	~Parser();

	void SetPattern(const std::string& fmt);

	template<typename ...Args>
	bool Parse(const std::string& str, Args && ...args);

	template<typename T>
	static void Regist(std::function<void(const std::string&, T&)> func);

	static ParserPlaceHolder place_holder;
private:
	ParserImpl* m_impl;

	template<typename T, typename ...Args>
	bool ParseOnce(int index, T && t, Args && ...args);

	bool ParseOnce(int index);
};

ParserPlaceHolder Parser::place_holder{};

#ifndef PASER_STATIC
#include "parser_inl.h"
#endif // !PASER_STATIC

