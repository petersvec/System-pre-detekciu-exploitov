#include "AgentSearch.hpp"
#include "TCPServer.hpp"
#include "CmdLine.hpp"

// tcp port
int tcpPort = 9999;

int main(int argc, char **argv)
{
	// vyhladanie agentov
	AgentSearch agentSearcher;
	agentSearcher.run(tcpPort);

	// cakanie na ich spojenia
	TCPServer tcpServer(tcpPort);
	tcpServer.run();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	// prikazovy riadok na ovladanie agentov
	CmdLine cmd;
	cmd.run(tcpServer);

	cmd.join();
	tcpServer.join();

	return 0;
}