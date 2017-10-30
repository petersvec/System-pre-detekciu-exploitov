#include "UdpListener.hpp"
#include "Configuration.hpp"
#include "Windivert.hpp"

int __cdecl main(int argc, char **argv)
{
	Configuration config;
	Windivert wd;
	
	UdpListener listener;
	listener.run(config);

	wd.init(config);
	wd.run(config);

	return 0;
}