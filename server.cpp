#include <iostream>
#include <chrono>
#include "http_win_server.h"

int main(int argc, char** argv)
{
    using namespace std::this_thread;     
    using namespace std::chrono_literals; 
    using std::chrono::system_clock;
    try {

    if (argc != 4)
    {
        for (int i = 0; i < argc; ++i)
        {
            std::cerr << argv[i] << '\n';
        }
        throw("\n!E!Invalid argument count.\nExample: '<ip> <port> <rootdir>, 127.0.0.1 8080 path\\'");
    }
    
        const auto ip = boost::asio::ip::make_address(argv[1]);
        const auto port = std::stoi(argv[2]);

        const std::string dir_root(argv[3]);
        
        rhttp::server server1;
        server1.start(ip, port, dir_root);
    }
    catch (const char* ex)
    {
        std::cout << "Error What: " << ex << '\n';
        return -1;
    }
    
    return 0;
}