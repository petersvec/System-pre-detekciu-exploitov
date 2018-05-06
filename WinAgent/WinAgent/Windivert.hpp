#pragma once

#include "Collector.hpp"
#include "Configuration.hpp"

#include <windivert.h>
#include <ctime>
#include <fstream>
#include <iomanip>

class Windivert {
private:
	// handle na windivert
	HANDLE mWindivert;

	// pointer na data packetu
	unsigned char *mPacket;

	// casovac pre odosielnanie packetov
	std::clock_t start;
	std::clock_t end;

	// zapis paketu do suboru
	void writePacket(unsigned char *data, UINT size, const std::string &protocol, const std::string &src_addr, const std::string &dst_addr, unsigned short src_port, unsigned short dst_port);

	// stream na zapis
	std::ofstream mOutput;

	// subor s paketmi
	std::string mPacketFile{ "" };

	// konvertovanie casoveho formatu
	time_t convert(SYSTEMTIME st);

	// zapnutie/vypnutie analyzy
	bool mEnabled{ true };

	// spracovanie sprav od klienta
	void messageProcess(const Configuration &config);

	// prevod formatu ip adresy
	std::string getIPAddress(UINT32 ip) const;

	// parsovane data
	WINDIVERT_ADDRESS address;
	PWINDIVERT_IPHDR ip_header;
	PWINDIVERT_IPV6HDR ipv6_header;
	PWINDIVERT_ICMPHDR icmp_header;
	PWINDIVERT_ICMPV6HDR icmpv6_header;
	PWINDIVERT_TCPHDR tcp_header;
	PWINDIVERT_UDPHDR udp_header;
	UINT packetLenght = 0;

public:
	Windivert();
	~Windivert();

	// inicializacia
	bool init(const Configuration &config);
	bool init(const std::string &filter, const Configuration &config);

	// spustenie zachytavania
	void run(const Configuration &config);
};
