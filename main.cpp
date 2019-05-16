
#define BOOST_COROUTINES_NO_DEPRECATION_WARNING
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <thread>
#include <iostream>
#include <boost/scoped_array.hpp>
#include "app/udpserver.h"
#include "app/controller.h"

int main(int argc, char *argv[]) {

	boost::asio::io_context ioc(1);

	logger::info("Start STUN Server");
	udpserver::startServer("0.0.0.0", 3478, ioc, controller::handleStunRequest);

	ioc.run();
}