//
// Created by 周蜀杰 on 2022/5/9.
//

#ifndef NETWORKDESIGN_UTIL_H
#define NETWORKDESIGN_UTIL_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>


namespace util {
	std::vector<std::string> readFile(const std::string &filePath);
	std::string trim(const std::string &str);
	bool startWith(const std::string &str, const std::string &prefix);
	bool contains(const std::string &str, const std::string &substr);
	std::vector<std::string> split(const std::string &str, const std::string &delimiter);
	bool isNumber(const std::string &str);
	bool equalsIgnoreCase(const std::string &str1, const std::string &str2);
	bool endWith(const std::string &str, const std::string &suffix);

	std::vector<std::string> subVector(const std::vector<std::string> &vec, unsigned long start, unsigned long end);
}

#endif //NETWORKDESIGN_UTIL_H