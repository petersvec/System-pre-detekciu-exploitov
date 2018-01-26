#include "CmdLine.hpp"

void CmdLine::run(TCPServer &server)
{
	std::cout << "[CmdLine] Starting command line" << std::endl;

	mMainThread = std::thread([this, &server]() {
		while (true)
		{
			std::string cmd;
			std::cout << "> ";
			std::getline(std::cin, cmd);

			// rozsekanie vstupu
			std::size_t pos = 0;
			std::vector<std::string> tokens;
			std::string delimiter = " ";

			while ((pos = cmd.find(delimiter)) != std::string::npos)
			{
				std::string  token = cmd.substr(0, pos);
				tokens.push_back(token);
				cmd.erase(0, pos + delimiter.length());
			}

			tokens.push_back(cmd);

			if (tokens.at(0) == "help")
			{
				std::cout << "Help:" << std::endl;
				std::cout << "list -> list of all connected agents" << std::endl;
				std::cout << "stop [agent] -> stops agent activity" << std::endl;
				std::cout << "start [agent] -> starts agent activity" << std::endl;
				std::cout << "filter [agent] [filter] -> resets windivert filter on agent" << std::endl;

				continue;
			}
			else if (tokens.at(0) == "list")
			{
				std::vector<std::string> agents = server.getAgents();

				int c = 1;
				for (auto &agent : agents)
				{
					std::cout << c << ". " << agent << std::endl;
					c++;
				}

				continue;
			}
			else if (tokens.at(0) == "stop")
			{
				std::string agent = tokens.at(1);

				auto agents = server.getAgents();
				for (auto &a : agents)
					if (a == agent)
						server.sendMessage(agent, "stop");

				continue;
			}
			else if (tokens.at(0) == "start")
			{
				std::string agent = tokens.at(1);

				auto agents = server.getAgents();
				for (auto &a : agents)
					if (a == agent)
						server.sendMessage(agent, "start");

				continue;
			}
			else if (tokens.at(0) == "filter")
			{
				std::string agent = tokens.at(1);
				std::string filter;

				for (std::size_t i = 2; i < tokens.size(); i++)
				{
					filter += tokens.at(i);
					filter += " ";
				}

				auto agents = server.getAgents();
				for (auto &a : agents)
					if (a == agent)
						server.sendMessage(agent, "filter//" + filter);

				continue;
			}
			else {
				std::cout << "> Wrong command" << std::endl;
				continue;
			}
		}
	});
}

void CmdLine::join()
{
	mMainThread.join();
}