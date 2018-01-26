#pragma once

#include <asio.hpp>
#include <thread>
#include <memory>
#include <map>
#include <string>
#include <iostream>
#include <vector>

class TCPServer {
private:
	// hlavne vlakno ktore pocuva pre prichadzajuce tcp spojenia
	std::thread mMainThread;

	// port na ktorom pocuvame
	int mPort;
	
	std::unique_ptr<asio::ip::tcp::acceptor> mAcceptor;
	asio::io_service mService;

	// otvorene spojenia
	std::map<std::string, std::unique_ptr<asio::ip::tcp::socket>> mOpenedConnections;
public:
	TCPServer(int tcpPort);

	void run();
	void join();

	int getPort() const { return mPort; }

	asio::io_service &getService() { return mService; }
	std::unique_ptr<asio::ip::tcp::acceptor> getAcceptor() { return std::move(mAcceptor); }

	void addConnection(const std::string &name, std::unique_ptr<asio::ip::tcp::socket> socket);
	void sendMessage(const std::string &agent, const std::string &msg);

	std::vector<std::string> getAgents() const;
};