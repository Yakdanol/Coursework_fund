#include <algorithm>
#include "handler.h"
#include "handler_concrete.h"

int main()
{
    handler *interpreter = new handler_concrete;
    std::string command;
    std::cout << "Print the commands:" << std::endl;

    while (command != "FINISH")
    {
        std::getline(std::cin, command);

        try
        {
            interpreter->accept_request(command);
        }
        catch (const std::exception &ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }

    delete interpreter;
    return 0;
}
