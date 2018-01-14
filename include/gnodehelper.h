/**
 * Copyright © 2016-2017 JiNong Inc. 
 * All Rights Reserved.
 *
 * \file gnodehelper.h
 * \brief libgnode는 노드와 온실통합제어기에서 사용되는데, 양단에서 필요한 메소드들만 모아서 핼퍼의 형태로 구현해 놓음.
 */

#ifndef _GNODE_HELPER_H_
#define _GNODE_HELPER_H_

#include <string.h>

#include "gnodeutil.h"
#include "gnode.h"

#define GNODE_VERSION    0
#define GNODE_DEFAULT_INITSTAT   INIT_STAT_SUCCESS
#define GNODE_DEFAULT_MONMODE    MON_MODE_ACTIVE
#define GNODE_DEFAULT_MONTIME    60

#define BUFSIZE     256

/** 메세지 상태 플래그 */
typedef enum {
    MSG_NONE = 0,           ///< 메세지 없음
    MSG_READY = 1           ///< 준비됨
} msgflag_t;

/** TTA Part 1, 2를 위한 베이스 핼퍼 클래스 */
class GNODEHelper {
protected:
    frameheader_t commonheader;         ///< 프레임 헤더
    GNODEMessage recvmsg;                ///< 받은 메세지
    GNODEMessage sendmsg;                ///< 전송할 메세지
    msgflag_t recvflag;                 ///< 받은 메세지의 상태 플래그
    msgflag_t sendflag;                 ///< 전송할 메세지의 상태 플래그

    char detailbuf[BUFSIZE];

    int getmessage (GNODEPayload *payload, byte *buf, uint bufsize) {
        int ret;

        _TLn ("getmessage ");
        ((this->commonheader).sequence)++;
        if ((this->commonheader).sequence >= GNODE_MAX_SEQUENCE)
            (this->commonheader).sequence = 0;

        //PRINTFRAMEHEADER ("getmessage", &(this->commonheader));

        (this->sendmsg).setframeheader (&(this->commonheader));
        (this->sendmsg).setpayload (payload);
        //PRINTFRAMEHEADER ("in message", (this->sendmsg).getframeheader());

        ret = (this->sendmsg).getmessage (buf, bufsize);
        _TL (ret); _TLn ("bytes generated.");
        return ret;
    }
public:
    //! 생성자
    GNODEHelper () {
        memset ((void *)(&(this->commonheader)), 0x00, sizeof(frameheader_t));
        (this->commonheader).sequence = 0;
        this->recvflag = MSG_NONE;
        this->sendflag = MSG_NONE;
    }

    //! 생성자
    GNODEHelper (uint gatewayid) {
        memset ((void *)(&(this->commonheader)), 0x00, sizeof(frameheader_t));
        (this->commonheader).sequence = 0;
        this->recvflag = MSG_NONE;
        this->sendflag = MSG_NONE;
        (this->commonheader).gatewayid = gatewayid;
    }

    //! 소멸자
    ~GNODEHelper () {
    }

    //! 메세지를 수신하는 메소드
    /*!
     \param buf 버퍼에 대한 포인터
     \param buflen 버퍼의 길이
     \return 사용된 버퍼의 길이
    */
    int receive (byte *buf, uint buflen) {
        int usedlen;

        _TL (buflen); _TLn ("bytes received");

        usedlen = (this->recvmsg).parse (buf, buflen);
        if (usedlen > 0) {
            _TL (usedlen); _TLn ("bytes used to parse");
            this->recvflag = MSG_READY;
        } else {
            _TL ("fail to parse :"); _TLn (usedlen);
            this->recvflag = MSG_NONE;
        }
        return usedlen;
    }

    //! 받은 메세지 리턴
    GNODEMessage *getrecvmsg () {
        return &(this->recvmsg);
    }

    //! 전송할 메세지 리턴
    GNODEMessage *getsendmsg () {
        return &(this->sendmsg);
    }

    //! 받은 메세지 클리어
    void clearrecvmsg () {
        (this->recvmsg).clear ();
    }

    //! 전송할 메세지 클리어
    void clearsendmsg () {
        (this->sendmsg).clear ();
    }

    //! 핼퍼 클래스의 내용을 확인하기 위한 간단한 요약 문자열을 리턴하는 메소드
    /*!
     \return 설정된 메세지
     */
    char *getdetail () {
        int idx = 0;

        if (this->sendflag == MSG_READY) {
            idx += sprintf (this->detailbuf + idx, "Request : \n");
            idx += (this->sendmsg).getdetail (this->detailbuf + idx, BUFSIZE);
        } else {
            idx += sprintf (this->detailbuf + idx, "No Request \n");
        }

        if (this->recvflag == MSG_READY) {
            idx += sprintf (this->detailbuf + idx, "Response : \n");
            idx += (this->recvmsg).getdetail (this->detailbuf + idx, BUFSIZE);
        } else {
            idx += sprintf (this->detailbuf + idx, "No Response \n");
        }

        return this->detailbuf;
    }
};

#ifdef USE_NODE_HELPER
/** 노드를 위한 핼퍼 클래스. USE_NODE_HELPER가 정의되어야 활성화 됨 */
class GNODENodeHelper : public GNODEHelper {
private:
    uint devicecount;                           ///< 장비 개수
    state1_head_t head;                         ///< state1 해더
    state1_foot_t foot;                         ///< state1 푸터
    state1_body_t devices[GNODE_MAX_DEVICE];     ///< state1 바디 배열 (장비 개수 만큼)

    //! 장비 정보 찾기
    /*!
     \param deviceid 장비 아이디
     \return state1 바디
     */
    state1_body_t *finddevice (byte deviceid) {
        uint i;

        for (i = 0; i < this->devicecount; i++) {
            if (((this->devices)[i]).devid == deviceid)
                return this->devices + i;
        }
        return NULL;
    }

public:
    //! 생성자
    /*!
     \param gatewayid 게이트웨이 아이디
     \param nodeid 노드 아이디
     \param devtype 장비 타입
     */
    GNODENodeHelper (uint gatewayid, uint nodeid, devtype_t devtype)
        : GNODEHelper (gatewayid) {
        (this->commonheader).nodeid = nodeid;
        (this->commonheader).devtype = devtype;

        (this->head).swver = GNODE_VERSION;
        (this->head).pfver = GNODE_VERSION;
        (this->head).gatewayid = gatewayid;
        (this->head).nodeid = nodeid;
        (this->head).initstat = GNODE_DEFAULT_INITSTAT;
        (this->head).monmode = GNODE_DEFAULT_MONMODE;
        (this->head).montime = GNODE_DEFAULT_MONTIME;
        (this->head).devnum = 0;
        (this->foot).commerr = 0;
        (this->foot).svcerr = 0;
        this->devicecount = 0;
    }

    //! 장비를 추가하면서 초기화함. (구동중 장비가 변경될 일이 없다고 가정).
    /*!
     \param deviceid 장비 아이디
     \param stat 장비 상태
     \param monmode 모니터링 모드
     \param montime 모니터링 주기
     \return GNODE_SUCCESS. GNODE_OUTOFINDEX 라면 장비 최대 개수 초과.
     */
    int initdevice (byte deviceid, initstat_t stat, monmode_t monmode, word montime) {
        state1_body_t *ptmp;

        _TL ("try add device : ");
        _TLn ((uint)deviceid);

        if (this->devicecount >= GNODE_MAX_DEVICE) {
            _TLn ("fail to add device. not enough space.");
            return GNODE_OUTOFINDEX;
        }

        ptmp = this->devices + this->devicecount;
        ptmp->devid = deviceid;
        ptmp->type = GENERAL_DEVICE;
        ptmp->value = 0;
        ptmp->devstat = DEV_STAT_NORMAL;
        ptmp->initstat = stat;
        ptmp->monmode = monmode;
        ptmp->montime = montime;
        (this->devicecount)++;
        (this->head).devnum = this->devicecount;

        return GNODE_SUCCESS;
    }

    //! 장비 값 업데이트
    /*!
     \param deviceid 장비 아이디
     \param value 장비 값
     \return GNODE_SUCCESS. GNODE_OUTOFINDEX 라면 없는 장비.
     */
    int setvalue (byte deviceid, word value) {
        state1_body_t *ptmp = this->finddevice (deviceid);

        if (ptmp == NULL) {
            _TLn ("device not found.");
            return GNODE_OUTOFINDEX;
        }
        _TLn ("device found.");

        ptmp->value = value;
        return GNODE_SUCCESS;
    }

    //! 장비 상태 업데이트
    /*!
     \param deviceid 장비 아이디
     \param value 장비 상태
     \return GNODE_SUCCESS. GNODE_OUTOFINDEX 라면 없는 장비.
     */
    int setstate (byte deviceid, devstat_t stat) {
        state1_body_t *ptmp = this->finddevice (deviceid);

        if (ptmp == NULL) {
            return GNODE_OUTOFINDEX;
        }

        ptmp->devstat = stat;
        return GNODE_SUCCESS;
    }

    //! 장비의 모니터링 모드 확인
    /*!
     \param deviceid 장비 아이디
     \return 모니터링 모드. MON_MODE_UNAVAILABLE라면 없는 장비.
     */
    monmode_t getmonitormode (byte deviceid) {
        state1_body_t *ptmp = this->finddevice (deviceid);

        if (ptmp == NULL) {
            return MON_MODE_UNAVAILABLE;
        }

        return ptmp->monmode;
    }

    //! 장비의 모니터링 주기 확인
    /*!
     \param deviceid 장비 아이디
     \return 모니터링 주기. 없는 장비라면 주기로 0을 리턴함.
     */
    word getmonitortime (byte deviceid) {
        state1_body_t *ptmp = this->finddevice (deviceid);

        if (ptmp == NULL) {
            return 0;
        }

        return ptmp->montime;
    }

    //! notify 메세지 생성
    /*!
     \param state state3 의 상태값
     \param buf 버퍼에 대한 포인터
     \param bufsize 버퍼의 길이
     \return 생성된 메세지의 길이
     */
    int notify (state3_t state, byte *buf, uint bufsize) {
        GNODENotifyPayload *payload = new GNODENotifyPayload (state);

        (this->commonheader).frametype = FT_NOTIFY;
        if (state == ST3_KEEPALIVE) {
            (this->commonheader).ackconfirm = 0;
        } else {
            (this->commonheader).ackconfirm = 1;
        }

        if (state == ST3_INIT) {
            (this->commonheader).payloadtype = PLT_DEV_INIT;
        } else {
            (this->commonheader).payloadtype = PLT_DEV_INFO;
        }
        (this->commonheader).payloadlen = 1;

        //PRINTFRAMEHEADER("notify", &(this->commonheader));

        return this->getmessage(payload, buf, bufsize);
    }

    //! 주기적으로 전송되는 데이터 메세지 생성
    /*!
     \param buf 버퍼에 대한 포인터
     \param bufsize 버퍼의 길이
     \return 생성된 메세지의 길이
     */
    int dataontime (byte *buf, uint bufsize) {
        uint i, num = 0;
        devinfo_t tmp;
        state1_body_t *body;
        GNODEDataPayload *payload = new GNODEDataPayload ();

        for (i = 0; i < this->devicecount; i++) {
            body = this->devices + i;
            switch (body->monmode) {
                case MON_MODE_ACTIVE:
                    if ((SECOND() % body->montime) == 0) {
                        tmp.devid = body->devid;
                        tmp.type = GENERAL_DEVICE;
                        tmp.value = body->value;
                        payload->setnextdeviceinfo (&tmp);
                        num++;
                    }
                    break;

                default:
                    break;
            }
        }

        (this->commonheader).frametype = FT_DATA;
        (this->commonheader).ackconfirm = 1;
        (this->commonheader).payloadtype = PLT_DEV_VALUE;
        (this->commonheader).payloadlen = num;

        return this->getmessage(payload, buf, bufsize);
    }

    //! 데이터 메세지 생성
    /*!
     \param buf 버퍼에 대한 포인터
     \param bufsize 버퍼의 길이
     \return 생성된 메세지의 길이
     */
    int data (byte *buf, uint bufsize) {
        uint i;
        devinfo_t tmp;
        state1_body_t *body;
        GNODEDataPayload *payload = new GNODEDataPayload ();

        for (i = 0; i < this->devicecount; i++) {
            body = this->devices + i;
            tmp.devid = body->devid;
            tmp.type = GENERAL_DEVICE;
            tmp.value = body->value;
            payload->setnextdeviceinfo (&tmp);
        }

        (this->commonheader).frametype = FT_DATA;
        (this->commonheader).ackconfirm = 1;
        (this->commonheader).payloadtype = PLT_DEV_VALUE;
        (this->commonheader).payloadlen = this->devicecount;

        return this->getmessage(payload, buf, bufsize);
    }

    //! 요청에 대한 응답 메세지 생성
    /*!
     \param buf 버퍼에 대한 포인터
     \param bufsize 버퍼의 길이
     \return 생성된 메세지의 길이. 음수일 경우 gnodeerr_t
     */
    int response (byte *buf, uint bufsize) {
        uint i;
        GNODERequestPayload *reqpayload;
        frameheader_t *reqheader;
        GNODEResponsePayload *payload;

        if (this->recvflag != MSG_READY)
            return GNODE_NOTREADY;

        reqpayload = (GNODERequestPayload *)((this->recvmsg).getpayload ());
        payload = new GNODEResponsePayload ();
        reqheader = (this->recvmsg).getframeheader ();

        payload->sethead (&(this->head));
        payload->setfoot (&(this->foot));

        for (i = 0; i < this->devicecount; i++) {
            payload->setnextbody (this->devices + i);
        }

        (this->commonheader).frametype = FT_RESPONSE;
        if (reqpayload->getrequestcmd () == REQ_PASSIVE)
            (this->commonheader).ackconfirm = 0;
        else
            (this->commonheader).ackconfirm = 1;
        (this->commonheader).payloadtype = reqheader->payloadtype;

        return this->getmessage(payload, buf, bufsize);
    }

    //! 요청 처리를 위해서 요청 페이로드를 제공
    /*!
      \return 요청 페이로드의 포인터
     */
    GNODERequestPayload *getrequestpayload () {
        if (this->recvflag != MSG_READY)
            return NULL;
        return (GNODERequestPayload *)((this->recvmsg).getpayload ());
   }
};
#endif

#ifdef USE_GCG_HELPER
/** GCG를 위한 핼퍼 클래스.  USE_GCG_HELPER가 정의 되어야 함 */
class GNODEGCGHelper : public GNODEHelper {
private:
    uint devicecount;                       ///< 장비 개수
    devinfo_t devices[GNODE_MAX_DEVICE];     ///< 장비 정보 배열(장비 개수 만큼)

public:
    //! 생성자
    GNODEGCGHelper () : GNODEHelper () {
    }

    //! 생성자
    GNODEGCGHelper (uint gatewayid) : GNODEHelper (gatewayid) {
    }

    //! 내부 정보 삭제
    void clear () {
        this->devicecount = 0;
    }

    //! 게이트웨이 아이디 설정
    void setgatewayid (uint gatewayid) {
        (this->commonheader).gatewayid = gatewayid;
    }

    //! 노드 아이디 설정
    void setnodeid (uint nodeid) {
        (this->commonheader).nodeid = nodeid;
    }

    //! 장비 정보 추가 (범용 메소드)
    /*!
     \param deviceid 장비 아이디
     \param type 장비 타입(TTA 표준에서 사용되는 장비 타입)
     \param value 장비 값
     \return gnodeerr_t
     */
    int setnext (byte deviceid, byte type, word value) {
        devinfo_t *pinfo;

        if (this->devicecount >= GNODE_MAX_DEVICE)
            return GNODE_OUTOFINDEX;

        pinfo = this->devices + this->devicecount;

        pinfo->devid = deviceid;
        pinfo->type = type;
        pinfo->value = value;

        (this->devicecount)++;

        return GNODE_SUCCESS;
    }

    //! 패시브형 센서 추가
    int setnextpassive (byte deviceid) {
        return this->setnext (deviceid, 0, 0);
    }

    //! 액티브형 센서 추가
    int setnextactive (byte deviceid, word time) {
        return this->setnext (deviceid, INTERVAL_SEC, time);
    }

    //! 이벤트형 센서 추가
    int setnextevent (byte deviceid, eventop_t op, word value) {
        return this->setnext (deviceid, (byte)op, value);
    }

    //! 구동기 추가
    int setnextcontrol (byte deviceid, word argument) {
        return this->setnext (deviceid, GENERAL_DEVICE, argument);
    }

    //! 요청 메세지 생성 (범용 메소드)
    /*!
     \param payload 요청 페이로드
     \param buf 버퍼에 대한 포인터
     \param bufsize 버퍼의 길이
     \return 생성된 메세지의 길이. 음수일 경우 gnodeerr_t
     */
    int request (GNODERequestPayload *payload, byte *buf, uint bufsize) {
        uint i;

        for (i = 0; i < this->devicecount; i++) {
            payload->setnextdeviceinfo (this->devices + i);
        }

        (this->commonheader).frametype = FT_REQUEST;
        (this->commonheader).ackconfirm = 1;

        return this->getmessage(payload, buf, bufsize);
    }

    //! 초기화 요청 메세지 생성
    int request_init (byte *buf, uint bufsize) {
        GNODERequestPayload *payload = new GNODERequestPayload (REQ_INIT);
        (this->commonheader).payloadtype = PLT_DEV_INIT;
        return this->request(payload, buf, bufsize);
    }

    //! 노드 초기화 요청 메세지 생성
    int request_initnode (byte *buf, uint bufsize) {
        GNODERequestPayload *payload = new GNODERequestPayload (REQ_INITNODE);
        (this->commonheader).payloadtype = PLT_DEV_INIT;
        return this->request(payload, buf, bufsize);
    }

    //! 장비 초기화 요청 메세지 생성
    int request_initdev (byte *buf, uint bufsize) {
        GNODERequestPayload *payload = new GNODERequestPayload (REQ_INITDEV);
        (this->commonheader).payloadtype = PLT_DEV_INIT;
        return this->request(payload, buf, bufsize);
    }

    //! 정보 요청 메세지 생성
    int request_info (byte *buf, uint bufsize) {
        GNODERequestPayload *payload = new GNODERequestPayload (REQ_NODEINFO);
        (this->commonheader).payloadtype = PLT_DEV_INIT;
        return this->request(payload, buf, bufsize);
    }

    //! 패시브 모니터링 메세지 생성
    int request_passivemonitor (byte *buf, uint bufsize) {
        GNODERequestPayload *payload = new GNODERequestPayload (REQ_PASSIVE);
        (this->commonheader).payloadtype = PLT_DEV_VALUE;
        return this->request(payload, buf, bufsize);
    }

    //! 액티브 모니터링 메세지 생성
    int request_activemonitor (byte *buf, uint bufsize) {
        GNODERequestPayload *payload = new GNODERequestPayload (REQ_ACTIVE);
        (this->commonheader).payloadtype = PLT_DEV_VALUE;
        return this->request(payload, buf, bufsize);
    }

    //! 이벤트 모니터링 메세지 생성
    int request_eventmonitor (byte *buf, uint bufsize) {
        GNODERequestPayload *payload = new GNODERequestPayload (REQ_EVENT);
        (this->commonheader).payloadtype = PLT_DEV_VALUE;
        return this->request(payload, buf, bufsize);
    }

    //! 제어 요청 메세지 생성
    int request_control (byte *buf, uint bufsize) {
        GNODERequestPayload *payload = new GNODERequestPayload (REQ_DEVSET);
        (this->commonheader).payloadtype = PLT_DEV_VALUE;
        return this->request(payload, buf, bufsize);
    }

    //! 응답 메세지 생성
    /*!
     \param buf 버퍼에 대한 포인터
     \param bufsize 버퍼의 길이
     \return 생성된 메세지의 길이. 음수일 경우 gnodeerr_t
     */
    int response (byte *buf, uint bufsize) {
        frameheader_t *reqheader;
        GNODEPayload *payload;

        if (this->recvflag != MSG_READY)
            return GNODE_NOTREADY;

        reqheader = (this->recvmsg).getframeheader ();

        switch (reqheader->frametype) {
            case FT_RESPONSE:
            case FT_NOTIFY:
                if (reqheader->ackconfirm == 1) {
                    //confirm
                    (this->commonheader).frametype = FT_CONFIRM;
                    (this->commonheader).nodeid = reqheader->nodeid;
                    payload = new GNODEConfirmPayload ();
                } else {
                    return GNODE_SUCCESS;
                }
                break;

            case FT_DATA:
                //Ack
                (this->commonheader).frametype = FT_ACK;
                (this->commonheader).nodeid = reqheader->nodeid;
                payload = new GNODEAckPayload ();
                break;

            default:
                return GNODE_SUCCESS;
        }

        (this->commonheader).ackconfirm = 0;

        return this->getmessage(payload, buf, bufsize);
    }

};
#endif

#endif
