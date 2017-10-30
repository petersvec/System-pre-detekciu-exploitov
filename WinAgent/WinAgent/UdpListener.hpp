#pragma once

#include <asio.hpp>
#include <thread>
#include <iostream>
#include <exception>

#include "Configuration.hpp"

class UdpListener {
private:
	// vlakno ktore caka na spravu od klienta
	std::thread mThread;

public:
	void run(Configuration &config);
	void join();
};