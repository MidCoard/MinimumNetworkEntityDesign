#include "Util.h"

std::vector<std::string> util::readFile(const std::string &filePath) {
	std::ifstream file(filePath);
	std::vector<std::string> lines;
	std::string line;
	while (std::getline(file, line))
		lines.push_back(line);
	return lines;
}

std::string util::trim(const std::string &str) {
	std::stringstream ss(str);
	std::string line;
	std::getline(ss, line);
	return line;
}

bool util::startWith(const std::string &str, const std::string &prefix) {
	if (str.size() < prefix.size())
		return false;
	return str.substr(0, prefix.size()) == prefix;
}

bool util::contains(const std::string &str, const std::string &substr) {
	return str.find(substr) != std::string::npos;
}

std::vector<std::string> util::split(const std::string &str, const std::string &delimiter) {
	if (delimiter.empty())
		return std::vector<std::string>{str};
	std::vector<std::string> tokens;
	unsigned long start = 0;
	while (str.find(delimiter, start) != std::string::npos) {
		unsigned long end = str.find(delimiter, start);
		tokens.push_back(str.substr(start, end - start));
		start = end + delimiter.size();
	}
	if (start < str.size())
		tokens.push_back(str.substr(start));
	return tokens;
}

bool isNumber(const std::string &str) {
	return std::all_of(str.begin(), str.end(), ::isdigit);
}