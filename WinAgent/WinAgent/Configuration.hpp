#pragma once

#include <string>
#include <iostream>
#include <mutex>

// nazov konfiguracneho suboru
const std::string configurationFileName{ "config_agent.xml" };

// mutex na riadenie sprav od klienta
extern std::mutex messageControlMutex;

// sprava od klienta
extern std::string clientMessage;

class Configuration {
private:
	std::string mAgentName{ "" };		// nazov agenta
	std::string mIPProtocol{ "" };		// filtrovanie podla IP protokolu(IPv4, IPv6)
	std::string mSrcAddr{ "" };			// filtrovanie podla zdrojovej adresy
	std::string mDstAddr{ "" };			// filtrovanie podla cielovej adresy
	std::string mCoreProtocol{ "" };	// filtrovanie podla vnoreneho protokolu(TCP,UDP,...)
	std::string mSrcPort{ "" };			// filtrovanie podla zdrojoveho portu
	std::string mDstPort{ "" };			// filtrovanie podla cieloveho portu
	std::string mBound{ "" };			// filtrovanie prichadzajucich/odchadzajucich paketov

	unsigned mQueueLength{ 8192 };		// velkost fronty na pakety
	unsigned mQueueTime{ 2048 };		// maximalny cas paketu vo fronte
	unsigned mSendingTime{ 60 };			// cas po ktorom sa odosielaju data na kolektor

	std::string mDirectory{ "Data\\" };		// adresar kam sa ukladaju pakety

	// aktualny pracovny adresar
	std::string mWorkingDirectory{ "" };

	// filter pre windivert
	std::string mWindivertFilter{ "" };

	// parsovanie xml dokumentu
	void parse();

	// ziskanie aktualneho pracovneho adresara
	void getWorkingDirectory();

	// vytvorenie filtra
	void createFilter();

	// vytvorenie adresara na ukladanie paketov
	void createDirectory();

	// ziskanie lokalnej ipcky
	std::string getLocalIP() const;

public:
	Configuration();

	std::string getWindivertFilter() const { return mWindivertFilter; }
	std::string getWorkingDirectory() const { return mWorkingDirectory; }

	unsigned getQueueLenght() const { return mQueueLength; }
	unsigned getQueueTime() const { return mQueueTime; }
	unsigned getSendingTime() const { return mSendingTime; }

	std::string getDirectory() const { return mDirectory; }
	std::string getAgentName() const { return mAgentName; }
};
