/**
 * Copyright © 2015-2016 JiNong Inc. 
 * All Rights Reserved.
 *
 * \file libgnode.h
 * \brief Arduino 에서 사용하기 위한 헤더파일
 * This file is only used for arduino
 */

#ifndef _LIB_GNODE_H_
#define _LIB_GNODE_H_

#define USE_ARDUINO
#define USE_NODE_HELPER

#ifdef USE_ARDUINO
    #include <Arduino.h>
    #include "include/gnodeutil.h"
    #include "include/gnode.h"
    #include "include/gnodehelper.h"
#endif

#endif
