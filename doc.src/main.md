A library for TTAK.KO-06.0288 Part 1, 2		{#mainpage}
=======================================

# libgnode

libgnode 는 온실관제시스템 표준인 TTAK.KO-06.0288/R1 Part 1, 2 표준을 구현한 라이브러리로, 온실 운영시스템과 온실 통합제어기 사이의 통신 메세지를 작성, 해석할 수 있다. 해당 표준에 대한 자세한 사항은 [TTAK.KO-06.0288 Part 1](http://www.tta.or.kr/data/ttas_view.jsp?rn=1&pk_num=TTAK.KO-06.0288-Part1), [TTAK.KO-06.0288 Part 2](http://www.tta.or.kr/data/ttas_view.jsp?rn=1&pk_num=TTAK.KO-06.0288-Part2)를 참조하길 바란다.

## Maintainers

김준용 (joonyong.jinong@gmail.com)

# libgnode의 구성
libgnode 는 2가지 모듈로 구성된다.

# Implementation 
## Data structures

## Request Message Creation

## Response Message

# Device Type
 TTAK.KO-06.0288/R1 Part 1, 2 표준에서는 센서의 타입이나 제어기의 타입을 정의하고 있지 않다. libgnode 에서는 특정 센서 혹은 특정 제어기에 대한 타입을 사용하지 않고, 사용자에 의한 처리를 기본으로 한다. 센서와 제어기의 타입은 0x00 으로 고정되며, 데이터값에 할당된 2byte는 사용자가 정의하여 사용하는 것으로 한다. 

# How to build
~~~~~~~~~~~~~~~
cmake .
make clean
make
make test
make doc
make install
~~~~~~~~~~~~~~~

# Unit tests & Sample

# How to use

