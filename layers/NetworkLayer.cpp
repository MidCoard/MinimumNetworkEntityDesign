//
// Created by 周蜀杰 on 2022/4/18.
//

#include <pcap/socket.h>
#include <unistd.h>
#include <net/if_var.h>
#include <net/if.h>
#include <cstring>
#include <utility>
#include <ifaddrs.h>
#include "NetworkLayer.h"

// backward address table learning
void NetworkLayer::learn(const uint8_t *mac, const uint32_t ip) {
	auto it = mac_table.find(mac);
	if (it == mac_table.end()) {
		mac_table.insert(std::make_pair(mac, ip));
	} else {
		it->second = ip;
	}
}

// receive packet from network and learn the mac address
void NetworkLayer::receive(const uint8_t *packet, const size_t len) {
	// check the packet length
	if (len < sizeof(struct ether_header)) {
		return;
	}
	// get the ethernet header
	struct ether_header *eth = (struct ether_header *)packet;
	// check the ethernet type
	if (ntohs(eth->ether_type) != ETHERTYPE_IP) {
		return;
	}
	// get the ip header
	struct ip *ip = (struct ip *)(packet + sizeof(struct ether_header));
	// check the ip version
	if (ip->ip_v != 4) {
		return;
	}
	// check the ip header length
	if (ip->ip_hl < 5) {
		return;
	}
	// check the ip protocol
	if (ip->ip_p != IPPROTO_TCP) {
		return;
	}
	// get the tcp header
	struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct ether_header) + ip->ip_hl * 4);
	// check the tcp header length
	if (tcp->th_off < 5) {
		return;
	}
	// get the source ip
	uint32_t sip = ip->ip_src.s_addr;
	// get the destination ip
	uint32_t dip = ip->ip_dst.s_addr;
	// get the source port
	uint16_t sport = tcp->th_sport;
	// get the destination port
	uint16_t dport = tcp->th_dport;
	// get the source mac address
	uint8_t smac[6];
	memcpy(smac, eth->ether_shost, 6);
	// get the destination mac address
	uint8_t dmac[6];
	memcpy(dmac, eth->ether_dhost, 6);
	// learn the mac address
	learn(smac, sip);
	learn(dmac, dip);
	// print the information
	printf("%s:%d -> %s:%d\n", inet_ntoa(ip->ip_src), sport, inet_ntoa(ip->ip_dst), dport);
}

// broadcast the packet to network
void NetworkLayer::broadcast(const uint8_t *packet, const size_t len) {
	// get the interface list
	struct ifaddrs *ifaddr, *ifa;
	if (getifaddrs(&ifaddr) == -1) {
		return;
	}
	// broadcast the packet to each interface
	for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
		// check the interface
		if (ifa->ifa_addr == nullptr) {
			continue;
		}
		// check the interface type
		if (ifa->ifa_addr->sa_family != AF_LINK) {
			continue;
		}
		// get the interface name
		const char *ifname = ifa->ifa_name;
		// get the interface address
		struct sockaddr_dl *sdl = (struct sockaddr_dl *)ifa->ifa_addr;
		// get the interface mac address
		uint8_t mac[6];
		memcpy(mac, LLADDR(sdl), 6);
		// get the interface index
		int ifindex = if_nametoindex(ifname);
		// get the interface flags
		uint32_t flags = if_flags(ifindex);
		// check the interface flags
		if ((flags & IFF_UP) == 0) {
			continue;
		}
		// get the interface mac address
		uint8_t smac[6];
		memcpy(smac, LLADDR(sdl), 6);
		// get the interface ip address
		uint32_t ip = if_ip(ifindex);
		// get the interface netmask
		uint32_t mask = if_mask(ifindex);
		// check the interface ip address
		if (ip == 0) {
			continue;
		}
		// check the interface netmask
		if (mask == 0) {
			continue;
		}
		// check the interface mac address
		if (memcmp(smac, mac, 6) == 0) {
			continue;
		}
		// get the interface network address
		uint32_t network = ip & mask;
		// get the interface broadcast address
		uint32_t broadcast = network | ~mask;
		// broadcast
		send(network, broadcast, smac, dmac, packet, len);
		// send
		send(broadcast, network, smac, dmac, packet, len);
	}
	// free the interface list
	freeifaddrs(ifaddr);
}
// port address list
std::map<uint16_t, uint32_t> NetworkLayer::port_address_list;
// port address list lock

void mian() {

}
