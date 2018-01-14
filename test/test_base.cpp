/**
 * \file test.cpp
 * \brief libgnode 테스트
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USE_NODE_HELPER
#define USE_GCG_HELPER

#include "gnodeutil.h"
#include "gnode.h"
#include "gnodehelper.h"
#include "test.h"

static uint gatewayid = 1;
static uint nodeid = 3;
static uint sensorid = 10;
static uint sensorid2 = 20;
//static uint actuatorid = 15;
static word working_period = 10;

int
test_frameheader () {
    byte *ptr;
    byte buf[1024];
    frameheader_t src;
    frameheader_t dest;

    src.devtype = DT_ACTUATOR;       // 1
    src.frametype = FT_CONFIRM;      // 1010
    src.security = 1;                // 1010 1
    src.ackconfirm = 1;              // 1010 11
    src.reserved = 0;                // 1010 1100  => 0xAC
    src.sequence = 173;              // 0000 0000 1010 1101 => 0x00AD  
    src.gatewayid = 70000;           // 0001 0001 0001 0111 0000 => 0x11170
    src.nodeid = 5000;               // 0000 0001 0011 1000 1000 => 0x01388
    src.payloadtype = PLT_DEV_VALUE; // 10 
    src.payloadlen = 10;             // 1000 1100  => 0x8A

    ptr = buf;
    SETFRAMEHEADER(ptr, (&src));
    printf ("%x\n", buf[8]);
    ptr = buf;
    GETFRAMEHEADER((&dest), ptr);

    GNODE_EQUAL (src.devtype, dest.devtype);
    GNODE_EQUAL (src.frametype, dest.frametype);
    GNODE_EQUAL (src.security, dest.security);
    GNODE_EQUAL (src.ackconfirm, dest.ackconfirm);
    GNODE_EQUAL (src.reserved, dest.reserved);
    GNODE_EQUAL (src.gatewayid, dest.gatewayid);
    GNODE_EQUAL (src.nodeid, dest.nodeid);
    GNODE_EQUAL (src.payloadtype, dest.payloadtype);
    GNODE_EQUAL (src.payloadlen, dest.payloadlen);

    return 0;
}

int
test_node_notify () {
#if defined (USE_NODE_HELPER) && defined (USE_GCG_HELPER)
    byte buf[1024];
    int len;
    GNODENodeHelper *node = new GNODENodeHelper (gatewayid, nodeid, DT_SENSOR);
    GNODEGCGHelper *gcg = new GNODEGCGHelper (gatewayid);
    GNODEMessage *pm1, *pm2;

    node->initdevice (sensorid, INIT_STAT_SUCCESS, MON_MODE_ACTIVE, working_period);
    len = node->notify (ST3_INIT, buf, 1024);
    pm1 = node->getsendmsg ();

    GNODE_ASSERT (0 < gcg->receive (buf, len), "notify message from node is not well formed");
    pm2 = gcg->getrecvmsg ();
    GNODE_ASSERT (0 == pm1->compare(pm2), "message is not same :(");

    len = gcg->response (buf, 1024);
    pm1 = gcg->getsendmsg ();

    GNODE_ASSERT (0 < node->receive (buf, len), "notify message from gcg is not well formed");
    pm2 = node->getrecvmsg ();
    GNODE_ASSERT (0 == pm1->compare(pm2), "message is not same :(");
#endif

    return 0;
}

int
test_node_data () {
#if defined (USE_NODE_HELPER) && defined (USE_GCG_HELPER)
    byte buf[1024];
    int len;
    GNODENodeHelper *node = new GNODENodeHelper (gatewayid, nodeid, DT_SENSOR);
    GNODEGCGHelper *gcg = new GNODEGCGHelper (gatewayid);
    GNODEMessage *pm1, *pm2;
    frameheader_t *phead;
    GNODEDataPayload *payload;
    devinfo_t *pinfo;

    node->initdevice (sensorid, INIT_STAT_SUCCESS, MON_MODE_ACTIVE, working_period);
    node->initdevice (sensorid2, INIT_STAT_SUCCESS, MON_MODE_ACTIVE, working_period);

    node->setvalue (sensorid, 100);
    node->setvalue (sensorid2, 200);
    len = node->data(buf, 1024);

    pm1 = node->getsendmsg ();
    GNODE_ASSERT (0 < gcg->receive (buf, len), "data message from node is not well formed");
    pm2 = gcg->getrecvmsg ();
    GNODE_ASSERT (0 == pm1->compare(pm2), "message is not same :(");

    phead = pm1->getframeheader ();
    GNODE_ASSERT (2 == phead->payloadlen, "payload length is not 2");
    payload = (GNODEDataPayload *)(pm1->getpayload ());
    pinfo = payload->getdeviceinfo (0);
    GNODE_ASSERT (100 == pinfo->value, "sensor value is not 100");
    pinfo = payload->getdeviceinfo (1);
    GNODE_ASSERT (200 == pinfo->value, "sensor value is not 100");

    len = gcg->response (buf, 1024);
    GNODE_ASSERT (0 < node->receive (buf, len), "data message from gcg is not well formed");
#endif
    return 0;
}

int
main () {
    GNODE_TEST (0, test_frameheader ());
	GNODE_TEST (0, test_node_notify ());
	GNODE_TEST (0, test_node_data ());

	return 0;
}
