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
		while (true) {
			if (shouldStop)
				break;
			if (layers.is_empty())
				std::this_thread::sleep_for(std::chrono::seconds(1));
			else {
				NetworkLayer *layer = nullptr;
				code_machina::BlockingCollectionStatus status = layers.take(layer);
				if (status == code_machina::BlockingCollectionStatus::Ok) {
					layer->sendDHCP();
					std::this_thread::sleep_for(std::chrono::seconds(1));
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