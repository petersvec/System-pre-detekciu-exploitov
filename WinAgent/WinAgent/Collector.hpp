#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <asio.hpp>

// globalny zoznam kolektorov
class Collector;
extern std::vector<Collector> collectors;

class Configuration;

class Collector {
private:
	// ip adresa a port kolektora
	std::string mIPAddress;
	std::string mPort;

	// timeout(v ms)
	DWORD mTimeout{ 5000 };

	// info o spustanom procese
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;

	void closeHandles();

public:
	Collector(const std::string &ip, const std::string &port);

	// verzia so spustanim procesu
	bool send(const Configuration &configuration);
};