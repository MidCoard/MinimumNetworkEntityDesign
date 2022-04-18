//
// Created by 周蜀杰 on 2022/4/18.
//

#ifndef NETWORKDESIGN_LINKLAYER_H
#define NETWORKDESIGN_LINKLAYER_H

#include "INetAddress.h"

class LinkLayer {
public:
	explicit LinkLayer(INetAddress address);

	void send(char *data, int len) const;
	void receive(const char *data, int len) const;

private:
	const INetAddress address;
	const Socket socket;
};


#endif //NETWORKDESIGN_LINKLAYER_H
