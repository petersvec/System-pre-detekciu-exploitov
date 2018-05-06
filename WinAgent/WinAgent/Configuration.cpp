#include "Configuration.hpp"
#include "pugixml.hpp"
#include "Collector.hpp"

#include <direct.h>
#include <Windows.h>

std::mutex messageControlMutex;
std::string clientMessage{ "" };

Configuration::Configuration()
{
	std::cout << "[Configuration] Creating configuration" << std::endl;
	getWorkingDirectory();
	parse();
	createFilter();
	createDirectory();
}

void Configuration::parse()
{
	std::string path = mWorkingDirectory + "\\" + configurationFileName;
	pugi::xml_document configurationFile;
	pugi::xml_parse_result result = configurationFile.load_file(path.c_str());

	if (result.status == pugi::xml_parse_status::status_ok)
	{
		pugi::xml_node configuration = configurationFile.child("Configuration");
		pugi::xml_node agent;
		
		if(configuration)
			agent = configuration.child("Agent");

		// nacitanie nastaveni agenta
		if (agent)
		{
			if (agent.child("Name"))
				mAgentName = agent.child("Name").text().as_string();
			if (agent.child("IP_Protocol"))
				mIPProtocol = agent.child("IP_Protocol").text().as_string();
			if (agent.child("SrcAddr"))
				mSrcAddr = agent.child("SrcAddr").text().as_string();
			if (agent.child("DstAddr"))
				mDstAddr = agent.child("DstAddr").text().as_string();
			if (agent.child("CoreProtocol"))
				mCoreProtocol = agent.child("CoreProtocol").text().as_string();
			if (agent.child("SrcPort"))
				mSrcPort = agent.child("SrcPort").text().as_string();
			if (agent.child("DstPort"))
				mDstPort = agent.child("DstPort").text().as_string();
			if (agent.child("Bound"))
				mBound = agent.child("Bound").text().as_string();
			if (agent.child("QueueLength"))
				mQueueLength = agent.child("QueueLength").text().as_uint();
			if (agent.child("QueueTime"))
				mQueueTime = agent.child("QueueTime").text().as_uint();
			if (agent.child("Directory"))
				mDirectory = agent.child("Directory").text().as_string();
		}

		// nacitanie kolektorov
		pugi::xml_node sender = configuration.child("Sender");

		if (sender)
		{
			for (pugi::xml_node collector = sender.child("Collector"); collector; collector = collector.next_sibling("Collector"))
			{
				std::string collectorAddr = collector.text().as_string();
				std::string addr = collectorAddr.substr(0, collectorAddr.find(":", 0));
				std::string port = collectorAddr.substr(collectorAddr.find(":", 0) + 1, collectorAddr.length() - collectorAddr.find(":", 0));
			
				Collector newCollector(addr, port);
				collectors.push_back(newCollector);
			}

			if (sender.child("SendingTime"))
				mSendingTime = atoi(sender.child("SendingTime").text().as_string());
		}
	}
	else {
		std::cout << "[Configuration] Could not parse configuration file: " << result.description() << std::endl;
	}
}

void Configuration::getWorkingDirectory()
{
	char path[200];

	if (_getcwd(path, sizeof(path)) == NULL)
	{
		std::cout << "[Configuration] Failed to retrieve working directory!" << std::endl;
	} else {
		mWorkingDirectory = std::string(path);
	}
}

void Configuration::createFilter()
{
	if (mBound != "")
		mWindivertFilter += mBound;

	if (mIPProtocol != "")
	{
		if (mIPProtocol == "IPv4")
			mWindivertFilter += " and ip";

		if (mIPProtocol == "IPv6")
			mWindivertFilter += " and ipv6";
	}

	if (mSrcAddr != "")
		mWindivertFilter += " and ip.SrcAddr == " + mSrcAddr;

	if (mDstAddr != "")
		mWindivertFilter += " and ip.DstAddr == " + mDstAddr;

	if (mCoreProtocol != "")
	{
		if (mCoreProtocol == "TCP")
		{
			if (mSrcPort != "")
				mWindivertFilter += "and tcp.SrcPort == " + mSrcPort;
			if (mDstPort != "")
				mWindivertFilter += "and tcp.DstPort == " + mDstPort;

		} 
		else if (mCoreProtocol == "UDP")
		{
			if (mSrcPort != "")
				mWindivertFilter += "and udp.SrcPort == " + mSrcPort;
			if (mDstPort != "")
				mWindivertFilter += "and udp.DstPort == " + mDstPort;
		} 
		else if (mCoreProtocol == "ICMP")
		{
			mWindivertFilter += " and icmp";
		}
	}

	// v pripade ak sme nemali nastaveny konfiguracny subor filtrujeme celu prevadzku
	if (mWindivertFilter == "")
	{
		std::string ip = getLocalIP();
		mWindivertFilter = "ip.SrcAddr == " + ip + " or ip.DstAddr == " + ip;
	}

	std::cout << "[Configuration] Filter created from configuration file: " << mWindivertFilter << std::endl;
}

std::string Configuration::getLocalIP() const
{
	WSADATA wsaData;
	WSAStartup(0x101, &wsaData);

	char hostName[255];
	gethostname(hostName, 255);
	struct hostent *host_entry;
	host_entry = gethostbyname(hostName);
	char *ip = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);
	WSACleanup();

	return std::string(ip);
}

void Configuration::createDirectory()
{
	std::string dataPath = mWorkingDirectory + "\\" + mDirectory;

	int ret = _mkdir(dataPath.c_str());
	
	if (ret == EEXIST)
		std::cout << "[Configuration] Directory " << dataPath << " already exists!" << std::endl;
	else if (ret == ENOENT)
	{
		std::cout << "[Configuration] Path " << dataPath << " was not found!" << std::endl;
		mDirectory = "";
	}
}