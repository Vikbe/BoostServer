#include <iostream>
#include "Server.h"



int main()
{
	try
	{
		srv::Server server;

		server.listen(80);

		server.run();
		
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}