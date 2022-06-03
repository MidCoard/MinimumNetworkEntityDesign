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

	extern bool debugMode;

	std::vector<std::string> readFile(const std::string &filePath);

	std::string trim(const std::string &str);

	bool startWith(const std::string &str, const std::string &prefix);

	bool contains(const std::string &str, const std::string &substr);

	std::vector<std::string> split(const std::string &str, const std::string &delimiter);

	bool isNumber(const std::string &str);

	bool equalsIgnoreCase(const std::string &str1, const std::string &str2);

	bool endWith(const std::string &str, const std::string &suffix);

	std::vector<std::string>
	subVector(const std::vector<std::string> &vec, unsigned long long start, unsigned long long end);

	void writeFile(const std::string &filename, const std::vector<std::string> &lines);

	std::string toHex(unsigned char c);

	std::string completeWith(std::string str, int length, char c);

	// must delete after use!!!
	std::pair<unsigned char *, long long> readBinaryFile(const std::string &filename);

	void writeBinaryFile(const std::string &filename, const unsigned char *data, long long size);

	bool inDebugMode();

	void setDebugMode(bool debug);

	unsigned int CRC(const unsigned char *data, int length);

	void put(std::vector<bool> *vector, unsigned char byte);

	unsigned char get(std::vector<bool> *vector, int index);
}

#endif //NETWORKDESIGN_UTIL_H
