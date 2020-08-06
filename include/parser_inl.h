#pragma once
#include <regex>
#include <map>
#include <any>
#include <vector>
#include "parser.h"

struct ParserImpl
{
	std::regex m_reg;
	std::match_results<std::string::iterator> m_result;
	ParserImpl() {

	}

	ParserImpl(const std::string& fmt) {
		SetRegex(fmt);
	}

	ParserImpl(const std::regex& reg) :m_reg(reg) {

	}

	void SetRegex(const std::string& fmt) {
		std::string cmd = fmt;
		for (auto it = m_replace_char.begin(); it != m_replace_char.end(); ++it) {
			cmd = std::regex_replace(cmd, std::regex(*it), *it);
		}
		cmd = std::regex_replace(cmd, std::regex("\\{\\}"), "(.+)");
		m_reg = std::regex(cmd);
	}

	bool Search(std::string& str) {
		return std::regex_search(str.begin(), str.end(), m_result, m_reg);
	}

	bool Match(std::string& str) {
		return std::regex_match(str.begin(), str.end(), m_result, m_reg);
	}

	static std::vector<std::string> m_replace_char;
	static std::map<std::string, std::any> m_conv_func;
};

inline Parser::Parser() {
	m_impl = new ParserImpl;
}

inline Parser::Parser(const std::string& fmt) {
	m_impl = new ParserImpl(fmt);
}

inline Parser::Parser(const Parser & parser)
{
	*this = parser;
}

inline Parser & Parser::operator=(const Parser & parser)
{
	m_impl = new ParserImpl(parser.m_impl->m_reg);
	return *this;
}

inline Parser::~Parser()
{
	delete m_impl;
}

void Parser::SetPattern(const std::string & fmt)
{
	m_impl->SetRegex(fmt);
}

template<typename ...Args>
bool Parser::Parse(const std::string& str, Args && ...args)
{
	if (!m_impl->Search(const_cast<std::string&>(str)))return false;
	return ParseOnce(1, std::forward<Args>(args)...);
}

template<typename T>
static void Parser::Regist(std::function<void(const std::string&, T&)> func)
{
	ParserImpl::m_conv_func.emplace(typeid(T).name(), func);
}

template<typename T, typename ...Args>
bool Parser::ParseOnce(int index, T && t, Args && ...args)
{
	try
	{
		auto key_name = typeid(T).name();
		auto any_func = m_impl->m_conv_func[key_name];
		auto conv_func = std::any_cast<std::function<void(const std::string&, T&)>>(any_func);
		conv_func(m_impl->m_result[index], t);
	}
	catch (std::exception& e)
	{
		return false;
	}
	return ParseOnce(index + 1, std::forward<Args>(args)...);
}

inline bool Parser::ParseOnce(int index)
{
	return true;
}

std::vector<std::string> ParserImpl::m_replace_char{
	"\\\\",
	"\\^",
	"\\$",
	"\\.",
	"\\[",
	"\\]",
	"\\(",
	"\\)",
	"\\*",
	"\\?",
	"\\+",
	"\\|"
};

std::map<std::string, std::any> ParserImpl::m_conv_func{
	{std::make_pair(typeid(void).name(), std::function<void(const std::string&)>([](const std::string& str) {}))},
	{std::make_pair(typeid(char).name(), std::function<void(const std::string&, char&)>([](const std::string& str, char& t) {t = str[0]; }))},
	{std::make_pair(typeid(unsigned char).name(), std::function<void(const std::string&, unsigned char&)>([](const std::string& str, unsigned char& t) {t = str[0]; }))},
	{std::make_pair(typeid(int).name(), std::function<void(const std::string&, int&)>([](const std::string& str, int& t) {t = std::atoi(str.c_str()); }))},
	{std::make_pair(typeid(unsigned int).name(), std::function<void(const std::string&, unsigned int&)>([](const std::string& str, unsigned int& t) {t = std::atoi(str.c_str()); }))},
	{std::make_pair(typeid(float).name(), std::function<void(const std::string&, float&)>([](const std::string& str, float& t) {t = static_cast<float>(std::atof(str.c_str())); }))},
	{std::make_pair(typeid(double).name(), std::function<void(const std::string&, double&)>([](const std::string& str, double& t) {t = std::atof(str.c_str()); }))},
	{std::make_pair(typeid(long).name(), std::function<void(const std::string&, long&)>([](const std::string& str, long& t) {t = std::atol(str.c_str()); }))},
	{std::make_pair(typeid(unsigned long).name(), std::function<void(const std::string&, unsigned long&)>([](const std::string& str, unsigned long& t) {t = std::atol(str.c_str()); }))},
	{std::make_pair(typeid(long long).name(), std::function<void(const std::string&, long long&)>([](const std::string& str, long long& t) {t = std::atoll(str.c_str()); }))},
	{std::make_pair(typeid(unsigned long long).name(), std::function<void(const std::string&, unsigned long long&)>([](const std::string& str, unsigned long long& t) {t = std::atoll(str.c_str()); }))},
	{std::make_pair(typeid(std::string).name(), std::function<void(const std::string&, std::string&)>([](const std::string& str, std::string& t) {t = str; }))},
	{std::make_pair(typeid(ParserPlaceHolder).name(), std::function<void(const std::string&, ParserPlaceHolder&)>([](const std::string& str, ParserPlaceHolder& t) {}))},
};