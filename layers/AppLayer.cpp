//
// Created by 周蜀杰 on 2022/4/18.
//

#include "AppLayer.h"
#include "UDPPrePacket.h"
#include "UDPACKPacket.h"
#include "NetworkLayer.h"

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
			int index = block->readInt();
			int size = block->readInt();
			int count = block->readInt();
			int wholeLength = block->readInt();
			log("Receive UDP Packet index: " + std::to_string(index) + " size: " + std::to_string(size) + " count: " + std::to_string(count) + " wholeLength: " + std::to_string(wholeLength));
			this->udpTable.add(block, index, size,count, wholeLength);
			break;
		}
		case 0x65: {
			IP ip = block->readIP();
			int count = block->readInt();
			int target = this->udpTable.tryAllocate();
			this->log("Receive UDP Pre Packet ip " + ip.str() + " pre_id " + std::to_string(count) + " packet_id " + std::to_string(target));
			auto* networkLayer = (NetworkLayer*)this->lowerLayers[0];
			auto* packet = new UDPACKPacket(ip, networkLayer->getIP(0),  count, target);
			auto* newBlock = packet->createBlock();
			delete packet;
			this->send(newBlock);
			break;
		}
		case 0x66: {
			IP ip = block->readIP();
			int count = block->readInt();
			int target = block->readInt();
			this->log("Receive UDP ACK Packet ip " + ip.str() + " pre_id " + std::to_string(count) + " packet_id " + std::to_string(target));
			this->log("Send UDP Packet to ip " + ip.str() + " pre_id " + std::to_string(count) + " packet_id " + std::to_string(target));
			this->table.send(count,target, ip);
			break;
		}
	}
}

void AppLayer::sendUDPData0(const IP& ip, unsigned char* data, int len) {
	auto* networkLayer = (NetworkLayer*)this->lowerLayers[0];
	IP source = networkLayer->getIP(0);
	int count = this->table.tryAllocate(data,len);
	this->log("Ready to send UDP data to " + ip.str() + " from " + source.str() + " pre_id " + std::to_string(count) + " bytes: " + std::to_string(len));
	auto* packet = new UDPPrePacket(ip,source,count);
	auto* block = packet->createBlock();
	delete packet;
	this->send(block);
}

void AppLayer::handleUDPData(unsigned char *data, long long length) {
	this->log("Receive UDP data size: " + std::to_string(length));
	util::writeBinaryFile("a.jpeg", data, length);
}

void AppLayer::start() {
	Layer::start();
	this->thread = new std::thread([this]() {
		while (true) {
			if (this->shouldThreadStop)
				break;
			std::pair<unsigned char*, std::pair<IP,long long>>* data = nullptr;
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
	auto* d = new unsigned char[length];
	memcpy(d, data, length);
	this->queue.emplace(new std::pair{d, std::pair{ip,length}});
}

void AppLayer::stop() {
	if (this->thread != nullptr) {
		this->shouldThreadStop = true;
		this->thread->join();
		delete this->thread;
		this->thread = nullptr;
	}
	Layer::stop();
}


