//
// Created by 周蜀杰 on 2022/5/29.
//

#include "DHCPHelper.h"

std::thread *dhcp::dhcpThread = nullptr;

code_machina::BlockingQueue<NetworkLayer *> dhcp::layers{};

bool dhcp::shouldStop = false;

void dhcp::request(NetworkLayer *layer) {
	layers.emplace(layer);
}

void dhcp::start() {
	dhcpThread = new std::thread([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		while (true) {
			if (shouldStop)
				break;
			if (layers.is_empty())
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			else {
				NetworkLayer *layer = nullptr;
				code_machina::BlockingCollectionStatus status = layers.take(layer);
				if (status == code_machina::BlockingCollectionStatus::Ok && !layer->isIPValid ) {
					layer->sendDHCP();
					std::this_thread::sleep_for(std::chrono::milliseconds (2000));
					if (!layer->isIPValid)
						layers.emplace(layer);
				}
			}
		}
	});
}

void dhcp::stop() {
	if (dhcpThread != nullptr) {
		shouldStop = true;
		dhcpThread->join();
		delete dhcpThread;
		dhcpThread = nullptr;
	}
}