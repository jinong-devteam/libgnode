# libgnode : A library for TTAK.KO-06.0288 Part 1, 2

## Introduction 

libgnode 는 온실관제시스템 표준인 TTAK.KO-06.0288/R1 Part 1, 2 표준을 구현한 라이브러리로, 온실 운영시스템과 온실 통합제어기 사이의 통신 메세지를 작성, 해석할 수 있다. 해당 표준에 대한 자세한 사항은 [TTAK.KO-06.0288 Part 1](http://www.tta.or.kr/data/ttas_view.jsp?rn=1&pk_num=TTAK.KO-06.0288-Part1), [TTAK.KO-06.0288 Part 2](http://www.tta.or.kr/data/ttas_view.jsp?rn=1&pk_num=TTAK.KO-06.0288-Part2)를 참조하길 바란다.

## Maintainers
* 김준용 (joonyong.jinong@gmail.com)

## libgnode의 구성
libgnode는 gnodebase와 gnodehelper의 두가지 모듈로 구성된다. gnodebase는 메세지를 구성하고 다루기위한 모듈이며, gnodehelper는 노드(센서노드 혹은 제어노드)와 온실통합제어기 각각에서 메세지를 쉽게 처리할 수 있도록 돕는 모듈이다.
libgnode는 메세지 생성까지만을 담당하며, 메세지 전송은 개별 어플리케이션에서 구현해야 한다.

## Getting Started

## How to use
### 노드 구동시 전송할 notify 메세지 생성
```
byte buf[1024];
int len;
GNODENodeHelper helper = new GNODENodeHelper (gatewayid, nodeid);

helper.initdevice (sensorid, INIT_STAT_SUCCESS, MON_MODE_ACTIVE, working_period);
len = helper.notify (ST3_INIT, buf, 1024); 
```
### 작동 주기마다 데이터 메세지 생성
```
// every 3 seconds
helper.setvalue (sensorid, new_observation); 
len = helper.data (buf, 1024);
```

### 온실통합제어기에서 active 모니터링 요청 메세지 생성
```
byte buf[1024];
int len;
GNODEGCGHelper helper = new GNODEGCGHelper (gatewayid);
helper.clear ();
helper.setnodeid (nodeid);
helper.setnextactive (sensorid, working_period);
len = helper.request_activemonitor (buf, 1024);
```

### 읽은 메세지 파싱 및 처리
핼퍼에 적당한 값들이 이미 들어있다면, 핼퍼는 응답을 자동으로 생성할 수 있다.

```
byte recvbuf[1024];
byte sendbuf[1024];
int len;

GNODENodeHelper helper = new GNODENodeHelper (gatewayid);

......
// in processing loop 
while (true) {
    len = read_from_gcg (recvbuf, 1024);
    if (helper.receive (recvbuf, len) > 0) {    // success to read a message

        len = helper.response (sendbuf, 1024);        
        send_to_gcg (sendbuf, len);
    }
}
```

## Modification
 TTAK.KO-06.0288/R1 Part 1, 2 표준은 해당 표준을 그대로 사용하기에 문제가 되는 부분들이 몇가지 있다. libgnode 에서는 해당 문제들을 대응하기 위해 임의로 결정한 부분들이 몇가지 있다. 

### Part 1, Part 2 의 통합
 Part 1과 Part 2는 센서노드, 제어노드를 대상으로 한다는 점을 제외하면 기본적인 프로토콜 구성이 동일하다. 특정 필드(ex. request command)에서의 중복등을 제외하면 두 표준을 하나처럼 사용하는 것이 가능하다. libgnode는 노드를 구분하지 않고 사용하도록 구성되어 있다.

### Device Type
 TTAK.KO-06.0288/R1 Part 1, 2 표준에서는 센서의 타입이나 제어기의 타입을 정의하고 있지 않다. libgnode 에서는 특정 센서 혹은 특정 제어기에 대한 타입을 사용하지 않고, 사용자에 의한 처리를 기본으로 한다. 센서와 제어기의 타입은 0x00 으로 고정되며, 데이터값에 할당된 2byte는 사용자가 정의하여 사용하는 것으로 한다. 

### active 모드의 작동 주기
 표준상에서 active 모드의 작동 주기의 단위를 설정할 수 있도록 되어 있다. 초, 분, 시간, 일, 월에 대한 설정이 가능하다. 하지만 상태정보(STATE1)를 전달할때는 해당 단위를 전달할 수 있는 필드가 존재하지 않는다. 그래서 libgnode는 작동주기의 단위를 최소단위인 초만을 고정으로 사용한다. 여기서 작동주기를 위한 필드가 2byte 이기 때문에, 가장 긴 주기는 약 18시간이 된다. 

### event 모드의 지원
 표준상에서 event 모드의 연산자와 피연산자를 설정할 수 있도록 되어 있다. 하지만 상태정보(STATE1)를 전달할때는 연산자와 피연산자를 전달할 수 있는 필드가 존재하지 않는다. libgnode에서는 event 모드에 대한 '처리'를 지원하지는 않는다. event 모드 메세지 전송에는 아무 문제가 없으며 다만 해당 이벤트에 대한 처리만을 지워하지 않는다는 의미이다. 따라서 필요하다면 사용자가 해당 모드에 대한 처리 코드를 추가해야 한다.

### Data 메세지의 페이로드 타입
 표준상에서 Data메세지의 페이로드 타입으로 Sensor Info (01)을 사용하도록 되어 있으나, Sensor Value (10)이 전체 내용상으로 볼때 더 적합하기 때문에 Data 메세지의 페이로드 타입은 Sensor Value로 처리한다.

### Request Command 의 중복
 표준에서는 센서노드와 제어노드를 위한 Request Command 가 별도로 제공되어 동일한 값이 다른 의미를 지니고 있다. libgnode에서는 두 표준을 통합하여 운영하기 때문에 중복을 피할 필요가 있다. 이에  제어노드의 Request Command 값중 ACTUATOR_SET은 표준상의 값 4가 아닌 7을 사용한다.  

### State2 Confirm 의 문제
 표준상에서 State2는 46bit로 이루어져있다. 바이트 단위로 떨어지지 않는 데이터를 전송하는 것이 적절한 것인지 의문이 든다. 또한 State2의 컨텐츠가 노드의 응답에 대한 확인으로 적절한 데이터는 아닌 것으로 보인다. libgnode에서는 Ack와 동일하게 Payload가 없는 Confirm을 사용한다.

## How to use
### Configuration
 * 사용할 용도에 따라서 libgnode.h 파일을 수정할 필요가 있다.  Arduino 에서 사용할 경우에는 다음의 define 문을 사용해야 한다.
```
    #define USE_ARDUINO
```
 * Node용으로 사용할 경우에는 다음의 define 문을 사용해야 한다.
```
    #define USE_NODE_HELPER
```
 * GCG용으로 사용할 경우에는 다음의 define 문을 사용해야 한다.
```
    #define USE_GCG_HELPER
```
 * Test 를 구동하려는 경우에는 USE_NODE_HELPER와 USE_GCG_HELPER를 모두 사용하는 것이 좋다.

 * libgnode 의 로그를 보고 싶다면 VERBOSE 를 정의한다.
```
    #define VERBOSE
```
 * 노드 하나에 할당되는 최대 장비의 수는 디폴트로 15개 이다. 이를 늘리거나 줄이고자 하는 경우 include/gnode.h 의 GNODE_MAX_DEVICE 의 값을 변경하면 된다.
 * 핼퍼를 사용해서 메세지를 생성할때 사용하는 버퍼의 크기는 최대 256으로 설정되어 있다.변경하고자 한다면 include/gnodehelper.h의 BUFSIZE 를 변경하면 된다.

### For test
```
cd debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make clean
make
make test
```

### For release
```
./build.sh
```

## License

This project is licensed under the BSD License - see the [LICENSE.md](LICENSE.md) file for details
