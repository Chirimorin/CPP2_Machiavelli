#pragma once
#include <vector>

inline std::vector<std::string> Split(const std::string& s, char delim)
{
	std::vector<std::string> result;

	size_t i = 0;
	size_t pos = s.find(delim);
	while (pos != std::string::npos)
	{
		result.push_back(s.substr(i, pos - i));
		i = ++pos;
		pos = s.find(delim, pos);
	}

	result.push_back(s.substr(i, s.length()));

	return result;
}