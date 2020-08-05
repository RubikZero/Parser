#pragma once
#include <regex>
#include <vector>
#include "parser.h"

void StringToType(const std::string& str) {

}

void StringToType(const std::string& str, int& t) {
	t = std::atoi(str.c_str());
}

void StringToType(const std::string& str, double& t) {
	t = std::atof(str.c_str());
}

void StringToType(const std::string& str, long& t) {
	t = std::atol(str.c_str());
}

void StringToType(const std::string& str, long long& t) {
	t = std::atoll(str.c_str());
}

void StringToType(const std::string& str, std::string& t) {
	t = str;
}

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
};

Parser::Parser() {
	m_impl = new ParserImpl;
}

Parser::Parser(const std::string& fmt) {
	m_impl = new ParserImpl(fmt);
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
void Parser::Regist(std::function<T(const std::string&)> func)
{
}

template<typename T, typename ...Args>
bool Parser::ParseOnce(int index, T && t, Args && ...args)
{
	try
	{
		StringToType(m_impl->m_result[index], t);
	}
	catch (...)
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