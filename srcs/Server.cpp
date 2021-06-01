#include "webserv.h"
#include "Location.hpp"
#include "Server.hpp"

Server::Server() {}

Server::Server(Server const &serv) : option(serv.option), location(serv.location)
{}

Server &Server::operator=(Server const &serv)
{
    if (this != &serv)
    {
        option = serv.option;
        location = serv.location;
    }
    return *this;
}

Server::~Server() {}


Server::Server(const std::string &str)
{}