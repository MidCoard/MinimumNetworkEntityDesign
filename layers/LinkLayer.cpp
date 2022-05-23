#include "LinkLayer.h"


//返回帧内容 字符填充法
char *getFrame(const char *a, int len) {
	bool pesc = 0;
	bool ok = 0;
	bool err = 0;

	int start = -1;
	int cnt = 0;

	char *b;


	for (int i = 0; i < len; i++) {
		if (a[i] == flag)// 遇到标志字符
		{
			if (pesc == 0) // 之前没有转义字符
			{
				if (start == -1)start = i;
				else break;
				ok = 0;
			} else ok = 1;
			pesc = 0;
		} else if (a[i] == esc)// 遇到转义字符
		{
			if (pesc == 1)// 之前有转义字符
			{
				ok = 1;
				pesc = 0;
			} else {
				ok = 0;
				pesc = 1;
			}
		} else if (pesc == 1 && start != -1)
			//遇到单独的转义字符，说明数据出现错误
		{
			err = 1;
			break;
		} else if (start != -1)
			// 记录有效数据
		{
			ok = 1;
			pesc = 0;
		}
		if (start != -1 && ok == 1) b[cnt++] = a[i];
	}
	if (err == 1) {
		char *c;
		return c;
	} else return b;
}

char* generateFrame(const char* data, int len) {
	char *b;
	b[0] = flag;
	int cnt = 1;
	// 增加标志字符 
	for (int i = 0; i < len; i++) {
		if (data[i] == flag || data[i] == esc)b[cnt++] = esc;
		// 增加转义字符 
		b[cnt++] = data[i];
	}
	b += flag;
	return b;
}


LinkLayer::LinkLayer(int device,int entityId, INetAddress address) : address(std::move(address)), socket(address.createSocket()), Layer(device,entityId) {}

void LinkLayer::receive(const char *data, int len) const {
	char *Frame = getFrame(data, len);

	if () {
		//data is error
	}

	//todo decode the data
}

char * toCharArray(std::vector<char> bytes) {
	char *b = new char[bytes.size()];
	for (int i = 0; i < bytes.size(); i++)
		b[i] = bytes[i];
	return b;
}

// raw data
void LinkLayer::send(char *data, int len) const {
	std::vector<char> bytes = std::vector<char>();
	for (int i = 0; i < len; i++) {
		if (data[i] == kFrameHeader || data[i] == kFrameFooter || data[i] == kFrameEscape)
			bytes.push_back(kFrameEscape);
		bytes.push_back(data[i]);
	}
	char * b = toCharArray(bytes);
	std::vector<Frame> frames = createFrames(1,b,bytes.size(),this->address);
	delete[] b;
	this->socket.send(data, len);
}

std::string mac;

//send Address Resolution Protocol packet
void LinkLayer::sendARP() const {
	std::string head = "ARP";
	std::string broadcast = "ff:ff:ff:ff:ff:ff";
	std::vector<char> bytes = std::vector<char>();
	char* string = const_cast<char *>(head.c_str());
	for (int i = 0;i<head.size();i++)
		bytes.push_back(string[i]);
	string = const_cast<char *>(broadcast.c_str());
	for (int i = 0;i<broadcast.size();i++)
		bytes.push_back(string[i]);
	string = const_cast<char *>(mac.c_str());
	for (int i = 0;i<mac.size();i++)
		bytes.push_back(string[i]);
	char * b = toCharArray(bytes);
	this->socket.send(b, strlen(b));
}


