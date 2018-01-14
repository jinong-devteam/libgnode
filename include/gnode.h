/**
 * Copyright © 2016-2017 JiNong Inc. 
 * All Rights Reserved.
 *
 * \file gnode.h
 * \brief TTAK.KO-06.0288 Part 1, 2 의 기본이 되는 상수와 클래스 정의
 */

#ifndef _GNODE_H_
#define _GNODE_H_

#include <string.h>

#define GNODE_MAX_DEVICE     15
#define GNODE_HEADER_LEN     9
#define GNODE_PLBUF_LEN      (GNODE_MAX_DEVICE * 7 + 15)  // << STATE1
#define GNODE_MSGBUF_LEN     (GNODE_HEADER_LEN + GNODE_PLBUF_LEN)
#define GNODE_MAX_SEQUENCE   65535

#define GNODE_FRAMEHEADERSIZE    9

typedef unsigned char byte;
#ifndef USE_ARDUINO
typedef unsigned short word;
#endif
typedef unsigned int uint;

/** 에러 타입 정의 */
typedef enum {
    GNODE_SUCCESS = 0,           ///< 성공
    GNODE_ERR_PARSE = -1,        ///< 파싱 에러
    GNODE_OUTOFINDEX = -2,       ///< 인덱스 범위 에러
    GNODE_NOTREADY = -3          ///< 메세지가 준비되어 있지 않다는 에러
} gnodeerr_t;

/** 장비 타입 정의 */
typedef enum {
    DT_SENSOR = 0,              ///< 센서
    DT_ACTUATOR = 1,            ///< 구동기
} devtype_t;

#define GENERAL_DEVICE  0x00    ///< TTAK.KO-06.0288 에서 센서나 구동기의 종류를 정의하지 않고 있어서 여기서는 0x00으로 고정하여 사용함


/** 프레임 타입 정의 */
typedef enum {
    FT_REQUEST = 0,     ///< 요청 프레임
    FT_RESPONSE = 1,    ///< 응답 프레임
    FT_CONFIRM = 2,     ///< 확인 프레임
    FT_NOTIFY = 3,      ///< 알림 프레임
    FT_DATA = 4,        ///< 데이터 프레임
    FT_ACK = 5          ///< 인지 프레임
} frametype_t;

/** 페이로드 타입 정의 */
typedef enum {
    PLT_DEV_INIT = 0,       ///< 초기화 페이로드
    PLT_DEV_INFO = 1,       ///< 정보 페이로드
    PLT_DEV_VALUE = 2,      ///< 값 페이로드
    PLT_DEV_RESERVED = 3    ///< 사용자 정의 페이로드
} payloadtype_t;

/** 장비 초기화 상태 타입 정의 */
typedef enum {
    INIT_STAT_FAIL = 0,         ///< 초기화 상태 : 실패
    INIT_STAT_SUCCESS = 1,      ///< 초기화 상태 : 성공
    INIT_STAT_UNAVAILABLE = 2   ///< 초기화 상태 : 사용불능
} initstat_t;

/** 모니터링 모드 타입 정의 */
typedef enum {
    MON_MODE_PASSIVE = 1,       ///< 패시브 모드 : 요청시 응답
    MON_MODE_ACTIVE = 2,        ///< 액티브 모드 : 주기적 전송
    MON_MODE_EVENT = 3,         ///< 이벤트 모드 : 이벤트 발생시 전송
    MON_MODE_UNAVAILABLE = 4    ///< 사용 불능
} monmode_t;

/** 장비 상태 타입 정의 */
typedef enum {
    DEV_STAT_NORMAL = 1,        ///< 장비상태 : 정상
    DEV_STAT_ABNORMAL = 2,      ///< 장비상태 : 비정상
    DEV_STAT_UNAVAILABLE = 3    ///< 장비상태 : 사용불능
} devstat_t;

/** 페이로드 가상 클래스 */
class GNODEPayload {
protected:
    byte payloadlen;        ///< 페이로드 길이

public:
    //! 생성자
    GNODEPayload () {
        this->payloadlen = 0;
    }

    //! 페이로드 길이 지정 생성자
    GNODEPayload (byte payloadlen) {
        this->payloadlen = payloadlen;
    }

    virtual ~GNODEPayload () {
    }

    //! 페이로드 길이를 리턴하는 메소드
    byte getpayloadlen () {
        return this->payloadlen;
    }

    //! 페이로드를 전송가능한 (프로토콜을 지원하는) 형태로 만들어 주는 메소드
    virtual void getmessage (byte *buf, uint *len, byte *payloadlen) = 0;
    //! 두 개의 페이로드를 비교해주는 메소드
    virtual int compare (GNODEPayload *payload) = 0;
};

/** 장비 정보 구조체 */
typedef struct {
    byte devid;         ///< 장비 아이디
    byte type;          ///< 장비 타입
    word value;         ///< 데이터
} devinfo_t;

/** 장비 정보 추가를 위한 함수형 매크로 */
#define SETDEVINFO(ptr,pdevinfo)            \
do {                                        \
    ptr[0] = (pdevinfo)->devid;             \
    ptr[1] = (pdevinfo)->type;              \
    ptr[2] = (pdevinfo)->value >> 8;        \
    ptr[3] = (pdevinfo)->value & 0xFF;      \
    ptr += 4;                               \
} while (0)

/** 장비 정보를 읽기 위한 함수형 매크로 */
#define GETDEVINFO(pdevinfo,ptr)                \
do {                                            \
    (pdevinfo)->devid = ptr[0];                 \
    (pdevinfo)->type = ptr[1];                  \
    (pdevinfo)->value = ptr[2] << 8 | ptr[3];   \
    ptr += 4;                                   \
} while (0)


/** 요청 타입 정의 */
typedef enum {
    REQ_INIT = 0,           ///< 초기화 요청
    REQ_INITNODE = 1,       ///< 노드 초기화 요청
    REQ_INITDEV = 2,        ///< 장비 초기화 요청
    REQ_NODEINFO = 3,       ///< 노드 정보 요청
    REQ_PASSIVE = 4,        ///< 패시브모드 센서 데이터 요청
    REQ_ACTIVE = 5,         ///< 액티브모드 센서 데이터 요청
    REQ_EVENT = 6,          ///< 이벤트모드 센서 데이터 요청
    REQ_DEVSET = 7          ///< 장비 세팅 요청
} reqcmd_t;

/** 간격 타입 정의 */
typedef enum {
    INTERVAL_SEC = 1,       ///< 초단위
    INTERVAL_MIN = 2,       ///< 분단위
    INTERVAL_HOUR = 3,      ///< 시간단위
    INTERVAL_DAY = 4,       ///< 일단위
    INTERVAL_MONTH = 5      ///< 월단위
} interval_t;

/** 이벤트 모드 연산자 정의 */
typedef enum {
    OP_GTEQ = 1,            ///< 크거나 같다
    OP_SMEQ = 2,            ///< 작거나 같다
    OP_GT = 3,              ///< 크다
    OP_SM = 4,              ///< 작다
    OP_EQ = 5               ///< 같다
} eventop_t;

/** 요청의 페이로드 클래스 */
class GNODERequestPayload : public GNODEPayload {
private:
    reqcmd_t requestcmd;                    ///< 요청의 종류
    devinfo_t devinfo[GNODE_MAX_DEVICE];     ///< 장비정보 배열. GNODE_MAX_DEVICE가 사이즈 임.

public:
    //! 응답에 대한 파싱시 사용하는 생성자
    /*!
     \param payloadlen 페이로드 길이
     \param buflen 버퍼의 길이
     \param buf 버퍼의 포인터
    */
    GNODERequestPayload (byte payloadlen, uint buflen, byte *buf)
        : GNODEPayload (payloadlen) {
            int i;
            byte *ptr;

            this->requestcmd = (reqcmd_t) buf[0];

            ptr = buf + 1;
            for (i = 0; i < payloadlen; i++) {
                GETDEVINFO((this->devinfo + i), ptr);
            }
        }

    //! 요청 생성시 사용하는 생성자
    /*!
     \param reqcmd 요청의 종류
    */
    GNODERequestPayload (reqcmd_t reqcmd)
        : GNODEPayload () {
            this->requestcmd = reqcmd;
        }

    ~GNODERequestPayload () {
    }


    //! 장비정보를 세팅하는 메소드
    /*!
     \param devinfo 장비정보에 대한 포인터
     \return 0 이면 성공, -1 이면 실패. 실패는 최대 장비수를 초과하는 경우.
    */
    int setnextdeviceinfo (devinfo_t *devinfo) {
        if (this->payloadlen >= GNODE_MAX_DEVICE)
            return -1;

        memcpy ((void *)(this->devinfo + this->payloadlen), (void *)devinfo, sizeof(devinfo_t));
        (this->payloadlen)++;
        return 0;
    }

    //! 장비정보를 꺼내는 메소드
    /*!
     \param idx 꺼내고자 하는 장비정보에 대한 인덱스
     \return 장비정보에 대한 포인터. NULL 이면 인덱스 초과
    */
    devinfo_t *getdeviceinfo (int idx) {
        if (idx < this->payloadlen) {
            return this->devinfo + idx;
        }

        return NULL;
    }

    //! 요청의 종류를 리턴하는 메소드
    /*!
     \return 페이로드의 요청 타입
    */
    reqcmd_t getrequestcmd () {
        return this->requestcmd;
    }

    //! 페이로드를 전송가능한 (프로토콜을 지원하는) 형태로 만들어 주는 메소드
    /*!
     \param buf 페이로드를 담을 버퍼에 대한 포인터
     \param len 버퍼의 길이
     \param payloadlen 페이로드의 길이
    */
    void getmessage (byte *buf, uint *len, byte *payloadlen) {
        int i;
        byte *ptr;

        buf[0] = (byte) this->requestcmd;
        ptr = buf + 1;

        for (i = 0; i < this->payloadlen; i++) {
            SETDEVINFO (ptr, (this->devinfo + i));
        }

        *len = ptr - buf;
        *payloadlen = this->payloadlen;
    }

    //! 두 개의 페이로드를 비교해주는 메소드
    /*!
     \param payload 비교하고자 하는 페이로드의 포인터
     \return 0 이면 같음. 그 외의 값은 다름.
    */
    int compare (GNODEPayload *payload) {
        GNODERequestPayload *tmp = (GNODERequestPayload *)payload;
        if ((this->requestcmd == tmp->getrequestcmd ()) 
                && (this->payloadlen == tmp->getpayloadlen ())) {

            // 아래 코드는 RequestPayload 의 devinfo가 배열이 아니면 변경되어야 함
            return memcmp ((void *)(this->devinfo), (void *)(tmp->getdeviceinfo(0)), 
                    sizeof(devinfo_t) * this->payloadlen);

        }
        return -1;
    }
};

/** 부가 상태 정보1의 헤더 부분에 대한 구조체 */
typedef struct {
    byte swver;             ///< 소프퉤어 버전
    byte pfver;             ///< 프로파일 버전
    uint gatewayid;         ///< 게이트웨이 아이디
    uint nodeid;            ///< 노드 아이디
    initstat_t initstat;    ///< 노드 초기화 상태 
    monmode_t monmode;      ///< 모니터링 모드
    word montime;           ///< 모니터링 주기
    byte devnum;            ///< 노드에 연결된 장비 개수
} state1_head_t;

/** 부가 상태정보1 헤더 구조체를 이용해 프로토콜에서 지정한 형태로 함수형 매크로 */
#define SETSTATE1HEAD(ptr,phead)                        \
do {                                                    \
    ptr[0] = (phead)->swver;                            \
    ptr[1] = (phead)->pfver;                            \
    ptr[2] = (((phead)->gatewayid) >> 12) & 0xFF;       \
    ptr[3] = (((phead)->gatewayid) >> 4) & 0xFF;        \
    ptr[4] = ((((phead)->gatewayid) << 4) & 0xF0)       \
            | ((((phead)->nodeid) >> 16) & 0xF);        \
    ptr[5] = (((phead)->nodeid) >> 8) & 0xFF;           \
    ptr[6] = (((phead)->nodeid)) & 0xFF;                \
    ptr[7] = ((((phead)->initstat) << 4) & 0xF0)        \
            | (((phead)->monmode) & 0xF);               \
    ptr[8] = (((phead)->montime) >> 8) & 0xFF;          \
    ptr[9] = ((phead)->montime) & 0xFF;                 \
    ptr[10] = (phead)->devnum;                          \
    ptr += 11;                                          \
} while (0);

/** 프로토콜에서 지정한 형태로부터 부가 상태정보1의 헤더구조체를 세팅해주는 함수형 매크로 */
#define GETSTATE1HEAD(phead,ptr)                        \
do {                                                    \
    (phead)->swver = ptr[0];                            \
    (phead)->pfver = ptr[1];                            \
    (phead)->gatewayid = (ptr[2] << 12) | (ptr[3] << 4) \
                        | ((ptr[4] >> 4) & 0xF);        \
    (phead)->nodeid = ((ptr[4] & 0xF) << 16)            \
                        | (ptr[5] << 8) | (ptr[6]);     \
    (phead)->initstat = (initstat_t)((ptr[7] >> 4) & 0xF);  \
    (phead)->monmode = (monmode_t)(ptr[7] & 0xF);       \
    (phead)->montime = (ptr[8] << 8) | ptr[9];          \
    (phead)->devnum = ptr[10];                          \
    ptr += 11;                               \
} while (0);

/** 부가 상태정보1의 반복되는 부분에 대한 구조체 */
typedef struct {
    byte devid;             ///< 장비 아이디
    byte type;              ///< 장비 타입
    word value;             ///< 장비 값
    devstat_t devstat;      ///< 장비 상태
    initstat_t initstat;    ///< 장비 초기화 상태
    monmode_t monmode;      ///< 장비 모니터링 모드
    word montime;           ///< 장비 모니터링 주기
} state1_body_t;

/** 부가 상태정보1의 바디로 부터 프로토콜 형태로 세팅해주는 함수형 매크로 */
#define SETSTATE1BODY(ptr,pbody)                    \
do {                                                \
    ptr[0] = (pbody)->devid;                        \
    ptr[1] = (pbody)->type;                         \
    ptr[2] = (pbody)->value >> 8;                   \
    ptr[3] = (pbody)->value & 0xFF;                 \
    ptr[4] = (pbody)->devstat;                      \
    ptr[5] = ((((pbody)->initstat) << 4) & 0xF0)    \
            | (((pbody)->monmode) & 0xF);           \
    ptr[6] = (pbody)->montime >> 8;                 \
    ptr[7] = (pbody)->montime & 0xFF;               \
    ptr += 8;                                       \
} while (0)

/** 프로토콜 메세지에서 부가 상태정보1의 구조체를 세팅해주는 함수형 매크로 */
#define GETSTATE1BODY(pbody,ptr)                \
do {                                            \
    (pbody)->devid = ptr[0];                    \
    (pbody)->type = ptr[1];                     \
    (pbody)->value = ptr[2] << 8 | ptr[3];      \
    (pbody)->devstat = (devstat_t)(ptr[4]);                 \
    (pbody)->initstat = (initstat_t)((ptr[5] >> 4) & 0xF);  \
    (pbody)->monmode = (monmode_t)(ptr[5] & 0xF);           \
    (pbody)->montime = (ptr[6] << 8) | ptr[7];              \
    ptr += 8;                                               \
} while (0)

/** 부가 상태정보 1의 푸터 */
typedef struct {
    word commerr;       ///< 통신 장애 회수
    word svcerr;        ///< 서비스 장애 회수
} state1_foot_t;

/** 부가 상태정보 1의 푸터로 부터 프로토콜 형태로 세팅해주는 함수형 매크로 */
#define SETSTATE1FOOT(ptr,pfoot)                \
do {                                            \
    ptr[0] = (pfoot)->commerr >> 8;             \
    ptr[1] = (pfoot)->commerr & 0xFF;           \
    ptr[2] = (pfoot)->svcerr >> 8;              \
    ptr[3] = (pfoot)->svcerr & 0xFF;            \
    ptr += 4;                                   \
} while (0)

/** 프로토콜 형태의 바이트배열에서 부가 상태정보 1의 푸터를 세팅해주는 함수형 매크로 */
#define GETSTATE1FOOT(pfoot,ptr)                \
do {                                            \
    pfoot->commerr = (ptr[0] << 8) | ptr[1];    \
    pfoot->svcerr = (ptr[2] << 8) | ptr[3];     \
    ptr += 4;                                   \
} while (0)

/** 응답 페이로드 클래스. 응답 페이로드의 경우 내용이 많고, 중간 부분에서 센서마다 다른 값을 설정해야하기 때문에 head, body, foot 으로 나누고, body를 센서별로 반복하는 형태를 갖는다.  */
class GNODEResponsePayload : public GNODEPayload {
private:
    state1_head_t head;                     ///< 부가 상태정보 1의 헤더
    state1_body_t body[GNODE_MAX_DEVICE];    ///< 부가 상태정보 1의 바디 (반복부분)
    state1_foot_t foot;                     ///< 부가 상태정보 1의 푸터

public:
    //! 생성자
    GNODEResponsePayload () 
        : GNODEPayload () {
    }

    //! 생성자
    /*!
     \param buflen 버퍼의 길이
     \param buf 버퍼의 포인터
     */
    GNODEResponsePayload (uint buflen, byte *buf)
        : GNODEPayload (buflen) {

        int i;
        byte *ptr = buf;

        GETSTATE1HEAD((&(this->head)), ptr);
        for (i = 0; i < (this->head).devnum; i++) {
            GETSTATE1BODY((this->body + i), ptr);
        }
        GETSTATE1FOOT((&(this->foot)), ptr);
    }

    ~GNODEResponsePayload () {
    }

    //! head를 세팅하는 메소드
    /*!
     \param head state1_head_t 타입에 대한 포인터 
     */
    void sethead (state1_head_t *head) {
        memcpy ((void *)(&(this->head)), (void *)head, sizeof (state1_head_t)); 
    }

    //! body를 세팅하는 메소드. 반복해서 사용함.
    /*!
     \param body state1_body_t 타입에 대한 포인터 
     \return 0 이면 성공, -1이면 실패. 실패인 경우 GNODE_MAX_DEVICE 초과임.
     */
    int setnextbody (state1_body_t *body) {
        if (this->payloadlen >= GNODE_MAX_DEVICE)
            return -1;

        memcpy ((void *)(this->body + this->payloadlen), (void *)body, sizeof (state1_body_t)); 
        ((this->head).devnum)++;

        return 0;
    }

    //! foot을 세팅하는 메소드. 
    /*!
     \param foot state1_foot_t 타입에 대한 포인터 
     */
    void setfoot (state1_foot_t *foot) {
        memcpy ((void *)(&(this->foot)), (void *)foot, sizeof (state1_foot_t)); 
    }

    //! head를 가져오는 메소드
    /*!
     \return 설정된 head 정보
     */
    state1_head_t *gethead () {
        return &(this->head);
    }

    //! body를 가져오는 메소드 
    /*!
     \param idx 인덱스값. 0부터 시작
     \return 설정된 body 정보
     */
    state1_body_t *getbody (int idx) {
        return this->body + idx;
    }

    //! foot을 가져오는 메소드
    /*!
     \return 설정된 foot 정보
     */
    state1_foot_t *getfoot () {
        return &(this->foot);
    }

    //! message를 버퍼에 설정해주는 메소드 
    /*!
     \param buf 버퍼에 대한 포인터
     \param len 길이에 대한 포인터
     \param payloadlen 페이로드 길이에 대한 포인터
     */
    void getmessage (byte *buf, uint *len, byte *payloadlen) {
        int i;
        byte *ptr = buf;

        SETSTATE1HEAD(ptr, (&(this->head)));
        for (i = 0; i < (this->head).devnum; i++) {
            SETSTATE1BODY(ptr, (this->body + i));
        }
        SETSTATE1FOOT (ptr, (&(this->foot)));
        *payloadlen = *len = ptr - buf;
    }

    //! 페이로드를 비교하는 메소드
    /*!
     \param payload 비교할 payload에 대한 포인터
     \return 0이면 동일.
     */
    int compare (GNODEPayload *payload) {
        int ret;
        GNODEResponsePayload *tmp = (GNODEResponsePayload *)payload;

        ret = memcmp ((void *)(&(this->head)), (void *)(tmp->gethead ()), sizeof (state1_head_t));
        if (ret == 0) {
            ret = memcmp ((void *)(this->body), (void *)(tmp->getbody(0)), 
                    sizeof (state1_body_t) * ((this->head).devnum));
            if (ret == 0) {
                ret = memcmp ((void *)(&(this->foot)), (void *)(tmp->getfoot ()), 
                        sizeof (state1_foot_t));
            }
        }
        return ret;
    }
};

/** 부가 상태정보2에 대한 구조체 */
typedef struct {
    byte swver;                 ///< GCG 소프트웨어 버전
    byte pfver;                 ///< GCG 프로파일 버전
    uint gatewayid;             ///< GCG 게이트웨이 아이디
    initstat_t nodeinitstat;    ///< 노드 초기화 상태
    initstat_t devinitstat;     ///< 장비 초기화 상태
    initstat_t iserror;         ///< 에러 상태
    initstat_t isbusy;          ///< GCG 용량초과여부
    byte requestnum;            ///< 메세지 재요청
} state2_t;

/** 컨펌 페이로드 클래스 */
class GNODEConfirmPayload : public GNODEPayload {
private:
    state2_t state;             ///< 부가 상태정보 1

public:
    //! 생성자
    GNODEConfirmPayload () : GNODEPayload () {
    }

    //! 생성자
    /*!
     \param buflen 버퍼의 길이
     \param buf 버퍼의 포인터
     */
    GNODEConfirmPayload (uint buflen, byte *buf)
        : GNODEPayload (buflen) {
        }

    ~GNODEConfirmPayload () {
    }

    //! 상태정보2를 세팅하는 메소드
    /*!
     \param state state2_t 타입에 대한 포인터 
     */
    void setstate2 (state2_t *state) { 
        memcpy ((void *)(&(this->state)), (void *)state, sizeof (state2_t));
    }

    //! 상태정보2를 가져오는 메소드
    /*!
     \return 설정된 상태정보2 
     */
    state2_t *getstate2 () { 
        return &(this->state);
    }

    //! message를 버퍼에 설정해주는 메소드 
    /*!
     \param buf 버퍼에 대한 포인터
     \param len 길이에 대한 포인터
     \param payloadlen 페이로드 길이에 대한 포인터
     */
    void getmessage (byte *buf, uint *len, byte* payloadlen) {
        *payloadlen = *len = 0;
    }

    //! 페이로드를 비교하는 메소드
    /*!
     \param payload 비교할 payload에 대한 포인터
     \return 0이면 동일.
     */
    int compare (GNODEPayload *payload) {
        return 0;
    }
};

/** 상태정보3에 대한 타입. 상태정보3은 상태정보 1바이트로 구성된 형태라서 enum타입하나로 처리함 */
typedef enum {
    ST3_INIT = 1,           ///< 사용자에의한 초기화
    ST3_RESET = 2,          ///< 노드 이상으로 인한 초기화
    ST3_COMM_ERR = 3,       ///< 통신 모듈 이상
    ST3_DEV_ERR = 4,        ///< 장비 이상
    ST3_DEVINT_ERR = 5,     ///< 장비 인터페이스 이상
    ST3_READ_ERR = 6,       ///< 장비 정보 수신 이상 // 7 is not used
    ST3_KEEPALIVE = 8,      ///< 정상 상태
    ST3_RECV_ERR = 9,       ///< 수신 불가
    ST3_SWTIMER_ERR = 10,   ///< 소프트웨어 타이머 이상
    ST3_HWTIMER_ERR = 11,   ///< 하드웨어 타이머 이상
    ST3_POWER_ERR = 12,     ///< 전원부 이상
    ST3_POWER_OFF = 13,     ///< 전원 꺼짐
    ST3_BAT_ERR = 14,       ///< 배터리 이상
    ST3_BAT_LOW = 15,       ///< 배터리 저전력
    ST3_BAT_OFF = 16,       ///< 배터리 꺼짐
    ST3_UNKNOWN_ERR = 255   ///< 알 수 없는 에러
} state3_t;

/** 노티파이 페이로드 클래스 */
class GNODENotifyPayload : public GNODEPayload {
private:
    state3_t state;         ///< 상태정보 3

public:
    //! 생성자
    /*!
     \param state 상태정보 3
     */
    GNODENotifyPayload (state3_t state) : GNODEPayload () {
        this->state = state;
    }

    //! 생성자
    /*!
     \param buflen 버퍼의 길이
     \param buf 버퍼의 포인터
     */
    GNODENotifyPayload (uint buflen, byte *buf)
        : GNODEPayload (buflen) {
            this->state = (state3_t)buf[0];
        }

    ~GNODENotifyPayload () {
    }


    //! 상태정보3을 가져오는 메소드
    /*!
     \return 설정된 상태정보3
     */
    state3_t *getstate3 () { 
        return &(this->state);
    }

    //! message를 버퍼에 설정해주는 메소드 
    /*!
     \param buf 버퍼에 대한 포인터
     \param len 길이에 대한 포인터
     \param payloadlen 페이로드 길이에 대한 포인터
     */
    void getmessage (byte *buf, uint *len, byte* payloadlen) {
        buf[0] = (byte)(this->state);
        *payloadlen = *len = 1;
    }

    //! 페이로드를 비교하는 메소드
    /*!
     \param payload 비교할 payload에 대한 포인터
     \return 0이면 동일.
     */
    int compare (GNODEPayload *payload) {
        GNODENotifyPayload *tmp = (GNODENotifyPayload *)payload;
        if (this->state != *(tmp->getstate3 ()))
            return -1;
        return 0;
    }
};

/** 애크 페이로드 클래스 */
class GNODEAckPayload : public GNODEPayload {
public:
    //! 생성자
    GNODEAckPayload () {
    }

    //! 생성자
    /*!
     \param buflen 버퍼의 길이
     \param buf 버퍼의 포인터
     */
    GNODEAckPayload (uint buflen, byte *buf) {
    }

    //! message를 버퍼에 설정해주는 메소드 
    /*!
     \param buf 버퍼에 대한 포인터
     \param len 길이에 대한 포인터
     \param payloadlen 페이로드 길이에 대한 포인터
     */
    void getmessage (byte *buf, uint *len, byte *payloadlen) {
        *payloadlen = *len = 0;
    }

    //! 페이로드를 비교하는 메소드
    /*!
     \param payload 비교할 payload에 대한 포인터
     \return 0이면 동일.
     */
    int compare(GNODEPayload *payload) {
        return 0;
    }
};

/** 데이터 페이로드 클래스 */
class GNODEDataPayload : public GNODEPayload {
private:
    devinfo_t devinfo[GNODE_MAX_DEVICE];     ///< 장비 정보. GNODE_MAX_DEVICE 만큼의 배열

public:
    //! 생성자
    /*!
     \param payloadlen 페이로드의 길이. 실제로는 장비의 개수.
     \param buflen 버퍼의 길이
     \param buf 버퍼의 포인터
     */
    GNODEDataPayload (byte payloadlen, uint buflen, byte *buf)
        : GNODEPayload (payloadlen) {
        int i;
        byte *ptr = buf;

        for (i = 0; i < this->payloadlen; i++) {
            GETDEVINFO ((this->devinfo + i), ptr);
        }
    }

    //! 생성자
    GNODEDataPayload () : GNODEPayload () {
    }

    ~GNODEDataPayload () {
    }

    //! 다음 장비정보를 설정하는 메소드
    /*!
     \param devinfo 설정하고자 하는 장비 정보의 포인터
     \return 0 이면 성공, -1이면 실패. 실패인 경우 장비 개수가 GNODE_MAX_DEVICE를 초과하는 경우임.
     */
    int setnextdeviceinfo (devinfo_t *devinfo) {
        if (this->payloadlen >= GNODE_MAX_DEVICE)
            return -1;

        memcpy ((void *)(this->devinfo + this->payloadlen), (void *)devinfo, sizeof(devinfo_t));
        (this->payloadlen)++;
        return 0;
    }

    //! 설정된 장비정보를 가져오는 메소드
    /*!
     \param idx 확인하고자하는 장비의 인덱스
     \return 설정된 장비정보
     */
    devinfo_t *getdeviceinfo (int idx) {
        if (idx < this->payloadlen) {
            return this->devinfo + idx;
        }

        return NULL;
    }

    //! message를 버퍼에 설정해주는 메소드 
    /*!
     \param buf 버퍼에 대한 포인터
     \param len 길이에 대한 포인터
     \param payloadlen 페이로드 길이에 대한 포인터
     */
    void getmessage (byte *buf, uint *len, byte* payloadlen) {
        int i;
        byte *ptr = buf;

        for (i = 0; i < this->payloadlen; i++) {
            SETDEVINFO(ptr, (this->devinfo + i));
        }

        *len = ptr - buf;
        *payloadlen = this->payloadlen;
    }

    //! 페이로드를 비교하는 메소드
    /*!
     \param payload 비교할 payload에 대한 포인터
     \return 0이면 동일.
     */
    int compare (GNODEPayload *payload) {
        GNODEDataPayload *tmp = (GNODEDataPayload *)payload;

        return memcmp ((void *)(this->devinfo), (void *)(tmp->getdeviceinfo(0)), 
                sizeof(devinfo_t) * this->payloadlen);
    }
};

/** 프레임 헤더에 대한 구조체 */
typedef struct {
	devtype_t devtype;          ///< 장비 타입
	frametype_t frametype;      ///< 프레임 타입
	byte security;              ///< 암호화 처리 필드. 암호화에 대한 지원은 없음.
	byte ackconfirm;            ///< 후속명령확인 필드
	byte reserved;              ///< 사용하지 않는 2비트 필드. 
    word sequence;              ///< 일련번호 필드
    uint gatewayid;             ///< GCG 아이디
    uint nodeid;                ///< 노드 아이디
    payloadtype_t payloadtype;  ///< 페이로드 타입
    byte payloadlen;            ///< 페이로드 길이
} frameheader_t;

/** 프레임헤더를 출력하는 매크로 */
#define PRINTFRAMEHEADER(msg, phead)                    \
do {                                                    \
    _TLnn (msg);                                        \
    _TL ("devtype "); _TLnn ((phead)->devtype);         \
    _TL ("frametype "); _TLnn ((phead)->frametype);     \
    _TL ("security "); _TLnn ((phead)->security);       \
    _TL ("ackconfirm "); _TLnn ((phead)->ackconfirm);   \
    _TL ("reserved "); _TLnn ((phead)->reserved);       \
    _TL ("sequence "); _TLnn ((phead)->sequence);       \
    _TL ("gatewayid "); _TLnn ((phead)->gatewayid);     \
    _TL ("nodeid "); _TLnn ((phead)->nodeid);           \
    _TL ("payloadtype "); _TLnn ((phead)->payloadtype); \
    _TL ("payloadlen "); _TLnn ((phead)->payloadlen);   \
} while (0)

/** 프레임헤더 구조체를 이용해서 실제 프레임헤더를 세팅하는 매크로 */
#define SETFRAMEHEADER(ptr,phead)   \
do {                                \
    ptr[0] = ((((phead)->devtype) << 7) & 0x80)     \
            | ((((phead)->frametype) << 4) & 0x70)  \
            | ((((phead)->security) << 3) & 0x08)   \
            | ((((phead)->ackconfirm) << 2) & 0x04) \
            | (((phead)->reserved) & 0x03);         \
    ptr[1] = (((phead)->sequence) >> 8) & 0xFF;           \
    ptr[2] = (((phead)->sequence)) & 0xFF;                \
    ptr[3] = (((phead)->gatewayid) >> 12) & 0xFF;       \
    ptr[4] = (((phead)->gatewayid) >> 4) & 0xFF;        \
    ptr[5] = ((((phead)->gatewayid) << 4) & 0xF0)       \
            | ((((phead)->nodeid) >> 16) & 0xF);        \
    ptr[6] = (((phead)->nodeid) >> 8) & 0xFF;           \
    ptr[7] = (((phead)->nodeid)) & 0xFF;                \
    ptr[8] = (((phead)->payloadtype) << 6) | ((phead)->payloadlen);     \
    ptr += 9;                                                           \
} while (0)

/** 실제 프레임헤더의 정보를 이용하여 프레임헤더 구조체를 세팅하는 매크로 */
#define GETFRAMEHEADER(phead,ptr)   \
do {                                \
    (phead)->devtype = (devtype_t)((ptr[0] >> 7) & 0x1);        \
    (phead)->frametype = (frametype_t)((ptr[0] >> 4) & 0x7);    \
    (phead)->security = (ptr[0] >> 3) & 0x1;    \
    (phead)->ackconfirm = (ptr[0] >> 2) & 0x1;  \
    (phead)->reserved = ptr[0] & 0x3;           \
    (phead)->sequence = (ptr[1] << 8) | ptr[2]; \
    (phead)->gatewayid = (ptr[3] << 12) | (ptr[4] << 4) \
                        | ((ptr[5] >> 4) & 0xF);        \
    (phead)->nodeid = ((ptr[5] & 0xF) << 16)            \
                        | (ptr[6] << 8) | (ptr[7]);     \
    (phead)->payloadtype = (payloadtype_t)((ptr[8] >> 6) & 0x3);    \
    (phead)->payloadlen = ptr[8] & 0x3F;                \
    ptr += 9;                                           \
} while (0);

/** 메세지 클래스 */
class GNODEMessage {
private:
    frameheader_t header;       ///< 프레임 헤더
    GNODEPayload *payload;       ///< 페이로드에 대한 포인터

public:
    //! 생성자
    GNODEMessage () { 
        this->payload = NULL;
    }

    //! 소멸자
    ~GNODEMessage () { 
        this->clear ();
    }

    //! 메세지를 파싱하는 메소드
    /*!
     \param buf 버퍼에 대한 포인터
     \param buflen 버퍼의 길이 
     \return 0 이면 실패. 0 < 이면 사용된 길이.
     */
    int parse (byte *buf, uint buflen) { 
        byte *ptr;
        uint tmplen;

        if (GNODE_FRAMEHEADERSIZE > buflen) {
            _TLn ("too small buffer");
            return 0;
        }

        this->clear ();

        ptr = buf;
        GETFRAMEHEADER ((&(this->header)), ptr);

        tmplen = (this->header).payloadlen;
        if (tmplen + GNODE_FRAMEHEADERSIZE > buflen) {
            _TL ("payload is not enough. needed : "); _TL (tmplen);
            _TL ("/ inputed : "); _TLn (buflen -  GNODE_FRAMEHEADERSIZE);
            return 0;
        }

        switch ((this->header).frametype) {
            case FT_REQUEST:
                tmplen = (this->header).payloadlen * sizeof(devinfo_t) + 1;
                if (tmplen + GNODE_FRAMEHEADERSIZE > buflen)
                    return 0;
                this->payload = new GNODERequestPayload ((this->header).payloadlen, tmplen, ptr);
                break;

            case FT_RESPONSE:
                this->payload = new GNODEResponsePayload (tmplen, ptr);
                break;

            case FT_CONFIRM:
                this->payload = new GNODEConfirmPayload (tmplen, ptr);
                break;

            case FT_NOTIFY:
                this->payload = new GNODENotifyPayload (tmplen, ptr);
                break;

            case FT_ACK:
                this->payload = new GNODEAckPayload (tmplen, ptr);
                break;

            case FT_DATA:
                tmplen = (this->header).payloadlen * sizeof(devinfo_t);
                if (tmplen + GNODE_FRAMEHEADERSIZE > buflen)
                    return 0;
                this->payload = new GNODEDataPayload ((this->header).payloadlen, tmplen, ptr);
                break;
        }

        return tmplen + GNODE_FRAMEHEADERSIZE;
    }

    //! 프레임헤더를 설정하는 메소드
    /*!
     \param frameheader 프레임헤더 구조체의 포인터
     */
    void setframeheader (frameheader_t *frameheader) { 
        memcpy ((void *)(&(this->header)), (void *)frameheader, sizeof(frameheader_t));
    }

    //! 설정된 프레임헤더를 가져오는 메소드
    /*!
     \return 설정된 프레임헤더 구조체의 포인터
     */
    frameheader_t *getframeheader () { 
        return &(this->header);
    }

    //! 페이로드를 설정하는 메소드
    /*!
     \param payload 페이로드 클래스의 포인터
     */
    void setpayload (GNODEPayload *payload) { 
        if (this->payload != NULL)
            this->clear ();
        this->payload = payload;
    }

    //! 설정된 페이로드를 가져오는 메소드
    /*!
     \return 설정된 페이로드 클래스의 포인터
     */
    GNODEPayload *getpayload () { 
        return this->payload;
    }

    //! 전송가능한 형태의 메세지를 버퍼에 설정해주는 메소드 
    /*!
     \param buf 버퍼에 대한 포인터
     \param bufsize 버퍼의 사이즈
     \return 설정된 메세지의 길이. 0이라면 버퍼의 길이가 짧아서 메세지 설정이 불가능하여 실패한 것임.
     */
    uint getmessage (byte *buf, uint bufsize) { 
        byte *ptr;
        byte plbuf[GNODE_PLBUF_LEN];
        uint plbuflen;
        byte payloadlen;

        if (this->payload == NULL) {
            (this->header).payloadlen = 0;
            plbuflen = 0;
        } else {
            (this->payload)->getmessage (plbuf, &plbuflen, &payloadlen);
            (this->header).payloadlen = payloadlen;
        }

        if (bufsize > plbuflen + GNODE_FRAMEHEADERSIZE) {
            ptr = buf;
            SETFRAMEHEADER(ptr, (&(this->header)));
            memcpy ((void *)ptr, (void *)plbuf, plbuflen);
            return plbuflen + GNODE_FRAMEHEADERSIZE;
        }

        return 0;
    }

    //! 메세지 내용을 확인하기 위한 간단한 요약 문자열을 리턴하는 메소드
    /*!
     \param buf 버퍼에 대한 포인터
     \param bufsize 버퍼의 사이즈
     \return 설정된 메세지의 길이
     */
    uint getdetail (char *buf, uint bufsize) {
        int n;
        n = sprintf (buf, 
                "DT:%d,FT:%d,SC:%d,CA:%d,SQ:%d,GW:%d,ND:%d,PT:%d,PL:%d\n", 
                (this->header).devtype, (this->header).frametype,
                (this->header).security, (this->header).ackconfirm,
                (this->header).sequence, (this->header).gatewayid,
                (this->header).nodeid, (this->header).payloadtype,
                (this->header).payloadlen); 
        return n;
    }

    //! 클래스에서 할당한 메모리 해제. 정확히는 페이로드 클래스의 삭제.
    void clear () {
        if (this->payload != NULL) {
            delete (this->payload);
            this->payload = NULL;
        }
    }

    //! 메세지를 비교하는 메소드
    /*!
     \param msg 비교할 메세지에 대한 포인터
     \return 0이면 동일.
     */
    int compare (GNODEMessage *msg) {
        int ret;
        ret = memcmp ((void *)(&(this->header)), (void *)(&(msg->header)), sizeof(frameheader_t));
        if (ret == 0) {
            ret = (this->payload)->compare (msg->getpayload ());
        }

        return ret;
    }

};

#endif
