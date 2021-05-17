#include <thread>
#include <signal.h>
#include "Chat.h"
void printExitMessage(int)
{
	std::cout << "Write !q to to quit\n";
}

int main(int argc, char **argv)
{
    ChatClient ec(argv[1], argv[2], argv[3]);

    std::thread net_thread([&ec](){ ec.net_thread(); });

    ec.login();
		signal(SIGINT, printExitMessage);

    ec.input_thread();
		net_thread.detach();
		return EXIT_SUCCESS;
}

