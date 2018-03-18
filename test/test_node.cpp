/**
 * \file test_node.cpp
 * \brief libgnode 테스트
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#define USE_NODE_HELPER

#include "gnodeutil.h"
#include "gnode.h"
#include "gnodehelper.h"
#include "test.h"

#define TEST_HOST   "220.90.136.106" 
#define TEST_PORT   5000

static uint gatewayid = 1;
static uint nodeid = 3;
static uint sensorid = 10;
static uint sensorid2 = 20;
//static uint actuatorid = 15;
static word working_period = 10;

int
test_node_data () {
    byte buf[128];
    int len;
    GNODENodeHelper *node = new GNODENodeHelper (gatewayid, nodeid, DT_SENSOR);

    node->initdevice (sensorid, INIT_STAT_SUCCESS, MON_MODE_ACTIVE, working_period);
    node->initdevice (sensorid2, INIT_STAT_SUCCESS, MON_MODE_ACTIVE, working_period);

    node->setvalue (sensorid, 100);
    node->setvalue (sensorid2, 200);
    len = node->data(buf, 128);

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(TEST_HOST), TEST_PORT);

    tcp::socket socket(io_service);
    socket.connect(endpoint);

    boost::asio::write(socket, boost::asio::buffer(buf, len));//, boost::asio::transfer_all(), ignored_error);
    
    /*
    boost::array<char, 128> readbuf;
    boost::system::error_code error;
    len = socket.read_some(boost::asio::buffer(readbuf), error);
    std::cout << "read : " << len;
    */

    /*
    for (;;) {
        boost::array<char, 128> readbuf;
        boost::system::error_code error;

        size_t len = socket.read_some(boost::asio::buffer(readbuf), error);

        if (error == boost::asio::error::eof)
            break; // Connection closed cleanly by peer.
        else if (error)
            throw boost::system::system_error(error); // Some other error.

        
        len = node->receive((byte *)(&readbuf[0]), readbuf.size());
        if (len > 0) {
            std::cout << "It's ok.";
        }
    }
    */

    return 0;
}

int
main () {
	GNODE_TEST (0, test_node_data ());

	return 0;
}
