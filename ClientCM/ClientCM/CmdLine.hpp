#pragma once

#include <iostream>
#include <thread>

#include "TCPServer.hpp"

class CmdLine {
private:
	// hlavne vlakno 
	std::thread mMainThread;
public:
	// spustenie prikazoveho riadku
	void run(TCPServer &server);

	void join();
};
