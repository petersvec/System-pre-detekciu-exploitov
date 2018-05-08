#include "Windivert.hpp"

Windivert::Windivert()
{
	mPacket = new unsigned char[0xFFFF];
}

Windivert::~Windivert()
{
	if (mWindivert)
		WinDivertClose(mWindivert);

	if (mPacket)
		delete[]mPacket;

	if (mOutput.is_open())
		mOutput.close();
}

bool Windivert::init(const Configuration &config)
{
	bool ret{ true };

	std::cout << "[Windivert] Initializing windivert from configuration file" << std::endl;

	// vytvorenie windivert filtra
	mWindivert = WinDivertOpen(config.getWindivertFilter().c_str(), WINDIVERT_LAYER_NETWORK, 0, WINDIVERT_FLAG_SNIFF);

	if (mWindivert == INVALID_HANDLE_VALUE)
	{
		ret = false;
		std::cout << "[Windivert] WinDivertOpen invalid handle value!" << std::endl;
		return ret;
	}

	// nastavenie velkosti fronty
	if (!WinDivertSetParam(mWindivert, WINDIVERT_PARAM_QUEUE_LEN, config.getQueueLenght()))
	{
		ret = false;
		std::cout << "[Windivert] Failed to set queue length!" << std::endl;
		return ret;
	}

	// nastavenie casu paketu
	if (!WinDivertSetParam(mWindivert, WINDIVERT_PARAM_QUEUE_TIME, config.getQueueTime()))
	{
		ret = false;
		std::cout << "[Windivert] Failed to set queue time!" << std::endl;
		return ret;
	}

	mPacketFile += config.getDirectory() + "packets.txt";

	return ret;
}

bool Windivert::init(const std::string &filter, const Configuration &config)
{
	bool ret{ true };

	std::cout << "[Windivert] Initializing windivert from client filter" << std::endl;

	// vytvorenie windivert filtra
	mWindivert = WinDivertOpen(filter.c_str(), WINDIVERT_LAYER_NETWORK, 0, WINDIVERT_FLAG_SNIFF);

	if (mWindivert == INVALID_HANDLE_VALUE)
	{
		ret = false;
		std::cout << "[Windivert] WinDivertOpen invalid handle value!" << std::endl;
		return ret;
	}

	// nastavenie velkosti fronty
	if (!WinDivertSetParam(mWindivert, WINDIVERT_PARAM_QUEUE_LEN, config.getQueueLenght()))
	{
		ret = false;
		std::cout << "[Windivert] Failed to set queue length!" << std::endl;
		return ret;
	}

	// nastavenie casu paketu
	if (!WinDivertSetParam(mWindivert, WINDIVERT_PARAM_QUEUE_TIME, config.getQueueTime()))
	{
		ret = false;
		std::cout << "[Windivert] Failed to set queue time!" << std::endl;
		return ret;
	}

	return ret;
}

void Windivert::run(const Configuration &config)
{
	start = clock();

	std::cout << "[Windivert] Starting windivert" << std::endl;

	while (TRUE)
	{
		messageProcess(config);

		if (mEnabled)
		{
			if (!WinDivertRecv(mWindivert, mPacket, 0xFFFF, &address, &packetLenght))
			{
				std::cout << "[Windivert] Failed to open packet!" << std::endl;
				continue;
			}

			WinDivertHelperParsePacket(mPacket, packetLenght, &ip_header,
				&ipv6_header, &icmp_header, &icmpv6_header, &tcp_header,
				&udp_header, NULL, NULL);

			if (ip_header == NULL && ipv6_header == NULL)
			{
				std::cout << "[Windivert] Junk packet!" << std::endl;
			}
			else
			{
				std::string src_addr = "";
				std::string dst_addr = "";

				if (ip_header)
				{
					src_addr = getIPAddress(ip_header->SrcAddr);
					dst_addr = getIPAddress(ip_header->DstAddr);

					unsigned short src_port = 0, dst_port = 0;
					std::string protocol = "";

					if (tcp_header)
					{
						protocol = "TCP";
						src_port = tcp_header->SrcPort;
						dst_port = tcp_header->DstPort;
					}
					else if (udp_header)
					{
						protocol = "UDP";
						src_port = udp_header->SrcPort;
						dst_port = udp_header->DstPort;
					}
					else if (icmp_header)
					{
						protocol = "ICMP";
					}

					writePacket(mPacket, packetLenght, protocol, src_addr, dst_addr, src_port, dst_port);
				}
			}

			// kontrola casu na odoslanie suborov
			end = clock();
			unsigned time = static_cast<unsigned>((end - start) / CLOCKS_PER_SEC);

			if (time >= config.getSendingTime() || mOutput.tellp() >= 60000)
			{
				std::cout << "[Windivert] Sending data to collector!" <<std::endl;
				std::cout << "[Windivert] Sending " << mOutput.tellp() << " bytes!" << std::endl;

				start = clock();

				mOutput << "End of Packets File\n";
				mOutput.close();

				// skusame postupne vsetky kolektory az kym sa nepodari odoslat data
				for (auto &col : collectors)
					if (col.send(config))
						break;
			}
		}
	}
}

void Windivert::messageProcess(const Configuration &config)
{
	messageControlMutex.lock();

	if (clientMessage != "")
	{
		if (clientMessage == "start")
		{
			mEnabled = true;
			clientMessage = "";
		}
		else if (clientMessage == "stop")
		{
			mEnabled = false;
			clientMessage = "";
		}
		else if (clientMessage.find("filter//", 0) != std::string::npos)
		{
			std::string filter = clientMessage.substr(clientMessage.find("//", 0) + 2, clientMessage.size() - clientMessage.find("filter//", 0));
			this->init(filter, config);
			clientMessage = "";
		}
	}

	messageControlMutex.unlock();
}

void Windivert::writePacket(unsigned char *data, UINT size, const std::string &protocol, const std::string &src_addr, const std::string &dst_addr, unsigned short src_port, unsigned short dst_port)
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	time_t packetTime = convert(st);

	// otvorenie streamu na zapis
	if (!mOutput.is_open())
		mOutput.open(mPacketFile, std::ofstream::out | std::ofstream::trunc);
	
	mOutput << std::dec << packetTime << "\n";
	mOutput << protocol << "\n";
	mOutput << src_addr << "\n";
	mOutput << dst_addr << "\n";
	mOutput << src_port << "\n";
	mOutput << dst_port << "\n";

	// zapis dat
	for (int i = 0; i < size; i++)
	{
		if (i && !(i % 16))
		{
			mOutput << "\n";
		}
		
		mOutput << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (unsigned)(data[i]) << " ";
	}
	
	mOutput << "\n";
	mOutput << "End of packet\n\n";
}

time_t Windivert::convert(SYSTEMTIME st)
{
	tm tf;

	tf.tm_sec = st.wSecond;
	tf.tm_min = st.wMinute;
	tf.tm_hour = st.wHour;
	tf.tm_mday = st.wDay;
	tf.tm_mon = st.wMonth - 1;
	tf.tm_year = st.wYear - 1900;
	tf.tm_isdst = -1;

	time_t time;
	time = mktime(&tf);
	return time;
}

std::string Windivert::getIPAddress(UINT32 ip) const
{
	struct in_addr addr;
	addr.S_un.S_addr = ip;
	auto *addr_ip = inet_ntoa(addr);
	return std::string(addr_ip);
}