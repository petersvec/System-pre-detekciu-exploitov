#include "Collector.hpp"
#include "Configuration.hpp"

std::vector<Collector> collectors;

Collector::Collector(const std::string &ip, const std::string &port)
{
	mIPAddress = ip;
	mPort = port;
}

void Collector::closeHandles()
{
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
}

bool Collector::send(const Configuration &configuration)
{
	std::string path = configuration.getDirectory() + "packets.txt";
	std::string arguments = "HBaseSender.exe " + path + " " + mIPAddress + " " + mPort;
	char *args = (char*)arguments.c_str();

	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInformation;
	DWORD ret = 0;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
	ZeroMemory(&processInformation, sizeof(processInformation));

	if (!CreateProcess("HBaseSender.exe", args, NULL, FALSE, 0, NULL, NULL, NULL, &startupInfo, &processInformation))
	{
		std::cout << "[Collector] Failed to create process HBaseSender.exe" << std::endl;
		return false;
	}

	WaitForSingleObject(processInformation.hProcess, INFINITE);
	GetExitCodeProcess(processInformation.hProcess, &ret);

	CloseHandle(processInformation.hProcess);
	CloseHandle(processInformation.hThread);

	return ret;
}