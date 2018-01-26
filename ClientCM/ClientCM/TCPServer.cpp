#include "TCPServer.hpp"

TCPServer::TCPServer(int tcpPort)
{
	mPort = tcpPort;
	mAcceptor = std::make_unique<asio::ip::tcp::acceptor>(mService, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), mPort));
}

void TCPServer::run()
{
	mMainThread = std::thread([this]() {
		std::cout << "[TCPServer] Listening on port " << this->getPort() << std::endl;
		auto &service = this->getService();
		auto acceptor = this->getAcceptor();

		while (true)
		{
			// cakanie na spojenie
			std::unique_ptr<asio::ip::tcp::socket> newSocket = std::make_unique<asio::ip::tcp::socket>(service);
			acceptor->accept(*newSocket);

			// prijatie identifikacnej spravy
			std::size_t bytesReceived{ 0 };
			asio::error_code error;
			char buffer[1000];

			try {
				bytesReceived = newSocket->read_some(asio::buffer(buffer), error);
			}
			catch (std::exception &e)
			{
				std::cout << "Exceptiom: "<< e.what() << std::endl;
			}
			
			std::string agent(buffer, bytesReceived - 1);
			std::cout << "[TCPServer] Received identification from agent: " << agent << std::endl;
			std::cout << "[TCPServer] Establishing tcp connection with agent \'" << agent << "\'" << std::endl;

			this->addConnection(agent, std::move(newSocket));
		}
	});
}

void TCPServer::sendMessage(const std::string &agent, const std::string &msg)
{
	auto find = mOpenedConnections.find(agent);

	if (find != mOpenedConnections.end())
	{
		std::size_t bytesSent{ 0 };
		asio::error_code error;
		std::string message = msg + "\n";
		bytesSent = asio::write(*mOpenedConnections[agent], asio::buffer(msg), error);
		std::cout << "[TCPServer] Sending message to agent: " << agent << std::endl;
		std::cout << "[TCPServer] Message content: " << message;
	}
	else {
		std::cout << "[TCPServer] Agent: " << agent << " not found!" << std::endl;
	}
}

void TCPServer::join()
{
	mMainThread.join();
}

void TCPServer::addConnection(const std::string &name, std::unique_ptr<asio::ip::tcp::socket> socket)
{
	mOpenedConnections[name] = std::move(socket);
}

std::vector<std::string> TCPServer::getAgents() const
{
	std::vector<std::string> agents;

	for (auto &conn : mOpenedConnections)
		agents.push_back(conn.first);

	return agents;
}