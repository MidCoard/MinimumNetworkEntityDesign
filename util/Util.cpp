#include "Util.h"
#include "iostream"

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

bool util::endWith(const std::string &str, const std::string &suffix) {
	if (str.size() < suffix.size())
		return false;
	return str.substr(str.size() - suffix.size(), suffix.size()) == suffix;
}

bool util::contains(const std::string &str, const std::string &substr) {
	return str.find(substr) != std::string::npos;
}

std::vector<std::string> util::split(const std::string &str, const std::string &delimiter) {
	if (delimiter.empty())
		return std::vector<std::string>{str};
	std::vector<std::string> tokens;
	unsigned long long start = 0;
	while (str.find(delimiter, start) != std::string::npos) {
		unsigned long long end = str.find(delimiter, start);
		tokens.push_back(str.substr(start, end - start));
		start = end + delimiter.size();
	}
	if (start < str.size())
		tokens.push_back(str.substr(start));
	return tokens;
}

bool util::isNumber(const std::string &str) {
	return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool util::equalsIgnoreCase(const std::string &str1, const std::string &str2) {
	return str1.size() == str2.size() && std::equal(str1.begin(), str1.end(), str2.begin(), [](char c1, char c2) {
		return std::tolower(c1) == std::tolower(c2);
	});
}

std::vector<std::string>
util::subVector(const std::vector<std::string> &vec, unsigned long long start, unsigned long long end) {
	std::vector<std::string> subVec;
	for (unsigned long long i = start; i < end; i++)
		subVec.push_back(vec[i]);
	return subVec;
}

void util::writeFile(const std::string &filename, const std::vector<std::string> &lines) {
	std::ofstream file(filename);
	for (const auto &line: lines)
		file << line << std::endl;
}

std::string util::toHex(unsigned char c) {
	std::stringstream ss;
	ss << std::hex << (int) c;
	return ss.str();
}

std::string util::completeWith(std::string str, int length, char c) {
	// append c to the head of str until str is of length length
	while (str.size() < length)
		str = c + str;
	return str;
}

std::pair<
		unsigned char *, long long> util::readBinaryFile(const std::string &filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
		return {nullptr, 0};
	file.seekg(0, std::ios::end);
	unsigned long long size = file.tellg();
	auto *buffer = new unsigned char[size];
	file.seekg(0, std::ios::beg);
	file.read((char *) buffer, size);
	file.close();
	return {buffer, size};
}


void util::writeBinaryFile(const std::string &filename, const unsigned char *data, long long size) {
	std::ofstream file(filename, std::ios::binary);
	file.write((char *) data, size);
	file.close();
}

bool util::debugMode = false;

void util::setDebugMode(bool debug) {
	debugMode = debug;
}

bool util::inDebugMode() {
	return debugMode;
}

unsigned int util::CRC(const unsigned char *data, int length) {
	unsigned int crc = 0xffffffff;
	for (int i = 0; i < length; i++) {
		crc = crc ^ data[i];
		for (int j = 0; j < 8; j++) {
			if ((crc & 0x1) == 1) {
				crc = (crc >> 1) ^ 0xedb88320;
			} else {
				crc = crc >> 1;
			}
		}
	}
	return crc;
}

void util::put(std::vector<bool> *vector, unsigned char byte) {
	for (int i = 0; i < 8; ++i) {
		vector->push_back(byte & 0x80);
		byte <<= 1;
	}
}

unsigned char util::get(std::vector<bool> *vector, int index) {
	unsigned char byte = 0;
	for (int i = 0; i < 8; ++i) {
		byte <<= 1;
		byte |= vector->at(index + i);
	}
	return byte;
}