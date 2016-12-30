#include "HttpServer.h"
#include <ios>

int main(int argc, char** argv)
{
	std::ios_base::sync_with_stdio(false);

	HttpServer app;
	return app.run(argc, argv);
}
