#pragma once

#include <asio.hpp>
#include <iostream>
#include <string>
#include <exception>

class AgentSearch {
private:
	// sprava ktora sa broadcastuje
	std::string mMessage{ "agentSearch" };

public:
	// spustenie vyhladavanie agentov na sieti
	void run(int tcpPort);
};
