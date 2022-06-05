//
// Created by 周蜀杰 on 2022/4/18.
//

#include "AppLayer.h"
#include "UDPPrePacket.h"
#include "UDPPreACKPacket.h"
#include "NetworkLayer.h"
#include "NetworkEntity.h"
#include "UDPRequestPacket.h"

const long long kPacketTime = 2LL * 1000 * 1000;

std::string AppLayer::getRawName() {
	return "APP";
}

AppLayer::AppLayer(NetworkEntity *networkEntity) : Layer(0, networkEntity) {
}

void AppLayer::handleSend(Block *block) {
	if (this->lowerLayers.size() == 1)
		this->lowerLayers[0]->send(new Block(block));
	else
		throw std::invalid_argument(" app layer must have one lower layer");
}

void AppLayer::handleReceive(int id, Block *block) {
	unsigned char header;
	block->read(&header, 1);
	switch (header) {
		case 0x60: {
			IP ip = block->readIP();
			int index = block->readInt();
			int count = block->readInt();
			int length = this->udpTable.add(block, ip, count, index);
			log("Receive UDP Packet index: " + std::to_string(index) + " count: " + std::to_string(count) +
			    " length: " + std::to_string(length));
			auto time = util::getNowTime();
			this->queue2.emplace(new std::pair{std::pair{ip, count}, time + kPacketTime * 2});
			break;
		}
		case 0x65: {
			IP ip = block->readIP();
			int count = block->readInt();
			int size = block->readInt();
			int wholeLength = block->readInt();
			int target = this->udpTable.tryAllocate(ip, count, size, wholeLength);
			this->log("Receive UDP Pre Packet ip " + ip.str() + " pre_id " + std::to_string(count) + " packet_id " +
			          std::to_string(target) + " packets: " + std::to_string(size) + " bytes: " +
			          std::to_string(wholeLength));
			auto *networkLayer = (NetworkLayer *) this->lowerLayers[0];
			auto *packet = new UDPPreACKPacket(ip, networkLayer->getIP(0), count, target);
			auto *newBlock = packet->createBlock();
			delete packet;
			this->send(newBlock);
			auto time = util::getNowTime();
			this->queue2.emplace(new std::pair{std::pair{ip, count}, time + kPacketTime * 3});
			break;
		}
		case 0x66: {
			IP ip = block->readIP();
			int count = block->readInt();
			int target = block->readInt();
			this->log("Receive UDP Pre ACK Packet ip " + ip.str() + " pre_id " + std::to_string(count) + " packet_id " +
			          std::to_string(target));
			this->log("Send UDP Packet to ip " + ip.str() + " pre_id " + std::to_string(count) + " packet_id " +
			          std::to_string(target));
			auto *networkLayer = (NetworkLayer *) this->lowerLayers[0];
			this->table.send(ip, networkLayer->getIP(0), count, target);
			break;
		}
		case 0x90: {
			// udp ack
			IP ip = block->readIP();
			int count = block->readInt();
			this->log("Receive UDP ACK Packet ip " + ip.str() + " pre_id " + std::to_string(count));
			auto time = util::getNowTime();
			this->queue2.emplace(new std::pair{std::pair{ip, count}, time + kPacketTime});
			this->udpTable.ack(ip, count);
			break;
		}
		case 0x91: {
			IP ip = block->readIP();
			int count = block->readInt();
			std::vector<int> ids;
			while (block->getRemaining())
				ids.push_back(block->readInt());
			this->log("Receive Request Resend Packet ip " + ip.str() + " pre_id " + std::to_string(count) + " ids " +
			          std::to_string(ids.size()));
			auto *networkLayer = (NetworkLayer *) this->lowerLayers[0];
			this->table.resend(ip, networkLayer->getIP(0), count, ids);
			break;
		}
		default: {
			error("Unknown protocol: " + std::to_string(header));
		}
	}
}

void AppLayer::resendPre(const IP &ip, std::pair<int, int> pair, int len) {
	kExecutor.submit(
			[this, ip, pair, len]() {
				if (this->table.requestResendPre(ip, pair.first)) {
					auto *networkLayer = (NetworkLayer *) this->lowerLayers[0];
					IP source = networkLayer->getIP(0);
					auto *packet = new UDPPrePacket(ip, source, pair.first, pair.second, len);
					auto *block = packet->createBlock();
					delete packet;
					this->send(block);
					this->resendPre(ip, pair, len);
				}
			}, std::chrono::milliseconds(3000));
}

void AppLayer::sendUDPData0(const IP &ip, unsigned char *data, int len) {
	auto *networkLayer = (NetworkLayer *) this->lowerLayers[0];
	IP source = networkLayer->getIP(0);
	auto pair = this->table.tryAllocate(ip, source, data, len);
	this->log("Ready to send UDP data to " + ip.str() + " from " + source.str() + " pre_id " +
	          std::to_string(pair.first) + " packets: " + std::to_string(pair.second) + " bytes: " +
	          std::to_string(len));
	auto *packet = new UDPPrePacket(ip, source, pair.first, pair.second, len);
	auto *block = packet->createBlock();
	delete packet;
	this->send(block);
	this->resendPre(ip, pair, len);
}

void AppLayer::handleUDPData(unsigned char *data, long long length) {
	this->log("Receive UDP data size: " + std::to_string(length));
	util::writeBinaryFile("a.jpeg", data, length);
}

void AppLayer::start() {
	Layer::start();
	this->queueThread = new std::thread(
			[this]() {
				while (true) {
					if (this->shouldThreadStop)
						break;
					std::pair<std::pair<IP, int>, long long> *pair = nullptr;
					code_machina::BlockingCollectionStatus status = this->queue2.try_take(pair);
					while (status == code_machina::BlockingCollectionStatus::Ok) {
						this->timeMap.insert_or_assign(pair->first, pair->second);
						delete pair;
						pair = nullptr;
						status = this->queue2.try_take(pair);
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					auto now = util::getNowTime();
					for (auto it = this->timeMap.begin(); it != this->timeMap.end();) {
						if (it->second < now) {
							log("Receive timer request ack");
							if (this->udpTable.ack(it->first.first, it->first.second))
								it->second += kPacketTime;
							else
								it = this->timeMap.erase(it);
						} else {
							it++;
						}
					}
				}
			}
	);
	this->thread = new std::thread([this]() {
		while (true) {
			if (this->shouldThreadStop)
				break;
			std::pair<unsigned char *, std::pair<IP, long long>> *data = nullptr;
			code_machina::BlockingCollectionStatus status = this->queue.try_take(data, std::chrono::seconds(1));
			if (status == code_machina::BlockingCollectionStatus::Ok) {
				this->sendUDPData0(data->second.first, data->first, data->second.second);
				delete[] data->first;
				delete data;
			}
		}
	});
}

void AppLayer::sendUDPData(const IP &ip, unsigned char *data, long long length) {
	auto *d = new unsigned char[length];
	memcpy(d, data, length);
	this->queue.emplace(new std::pair{d, std::pair{ip, length}});
}

void AppLayer::stop() {
	this->shouldThreadStop = true;
	if (this->thread != nullptr) {
		this->thread->join();
		delete this->thread;
		this->thread = nullptr;
	}
	if (this->queueThread != nullptr) {
		this->queueThread->join();
		delete this->queueThread;
		this->queueThread = nullptr;
	}
	Layer::stop();
}

void AppLayer::receive(int id, Block *block) {
	Layer::receive(id, block);
}

void AppLayer::resend(const IP &ip, int count, std::vector<int> ids) {
	auto layer = (NetworkLayer *) this->lowerLayers[0];
	auto *packet = new UDPRequestPacket(ip, layer->getIP(0), count, ids);
	auto *block = packet->createBlock();
	delete packet;
	this->send(block);
}




