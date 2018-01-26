#include "TCPConnection.hpp"

std::unique_ptr<TCPConnection> tcpConnection;

void TCPConnection::establishConnection(const asio::ip::address &address, int port, Configuration &config)
{
	std::cout << "[TCPConnection] Establishing connection!" << std::endl;
	io_service = std::make_shared<asio::io_service>();
	asio::ip::tcp::endpoint endpoint(address, port);

	mSocket = std::make_shared<asio::ip::tcp::socket>(*io_service);

	try {
		mSocket->connect(endpoint);
	}
	catch (std::exception &e)
	{
		std::cout << "[TCPConnection] Exception: " << e.what() << std::endl;
	}

	// odoslanie prvej spravy, ktorou sa identifikuje agent
	asio::error_code error;
	const std::string msg = config.getAgentName() + "\n";
	std::size_t bytesSent = asio::write(*mSocket, asio::buffer(msg), error);
	std::cout << "[TCPConnection] Sent identification message(" << bytesSent << " b): " << msg;

	// ak sa podarilo spojit tak spustime vlakno na prijmanie sprav
	if (mSocket->is_open())
	{
		mThread = std::thread([this]() {
			while (true)
			{
				this->receiveMessage();
			}
		});
	}
}

void TCPConnection::receiveMessage()
{
	asio::error_code error;
	char buffer[1000];
	std::size_t bytesReceived{ 0 };
	
	while (bytesReceived == 0)
	{
		bytesReceived = mSocket->read_some(asio::buffer(buffer), error);

		if (bytesReceived != 0)
		{
			messageControlMutex.lock();
			std::string message(buffer, bytesReceived);
			std::cout << "[TCPConnection] Message received: " << message << std::endl;
			clientMessage = message;
			messageControlMutex.unlock();
		}
	}
}