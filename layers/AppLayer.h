//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_APPLAYER_H
#define NETWORKDESIGN_APPLAYER_H

#include "Layer.h"
#include "UDPPreTable.h"
#include "UDPTable.h"

class AppLayer : public Layer {

public:
	explicit AppLayer(NetworkEntity *networkEntity);

	std::string getRawName() override;

	void handleReceive(int id, Block *block) override;

	void handleSend(Block *block) override;

	void handleUDPData(unsigned char *data, long long length);

	void start() override;

	void stop() override;

	void sendUDPData(const IP& ip, unsigned char *data, long long length);

private:
	void sendUDPData0(const IP& ip, unsigned char *data, int len);
	std::thread * thread = nullptr;
	code_machina::BlockingQueue<std::pair<unsigned char*, std::pair<IP,long long>>*> queue;
	UDPPreTable table = UDPPreTable(this);
	UDPTable udpTable = UDPTable(this);
	bool shouldThreadStop = false;
};


#endif //NETWORKDESIGN_APPLAYER_H
