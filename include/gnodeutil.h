/**
 * Copyright © 2015-2016 JiNong Inc. 
 * All Rights Reserved.
 *
 * \file gnodeutil.h
 * \brief libgnode 에서 사용하는 매크로 
 */

#ifndef _GNODE_UTIL_H_
#define _GNODE_UTIL_H_

#ifdef USE_ARDUINO
    #define SECOND()    (uint)(millis()/1000)
#ifdef VERBOSE
    #define _TL(msg)    Serial.print(msg)
    #define _TLn(msg)   Serial.print(msg); Serial.print (" ("); Serial.print (__FILE__); \
                        Serial.print (":"); Serial.print (__LINE__); Serial.println (")"); \
                        Serial.flush ();
    #define _TLnn(msg)  Serial.println(msg); Serial.flush ();
#endif
#else
    #include <iostream>
    #define SECOND()    (uint)(time(NULL))
#ifdef VERBOSE
    #define _TL(msg)    std::cerr<<(msg) 
    #define _TLn(msg)   std::cerr<<(msg)<<" ("<<__FILE__<<" : "<<__LINE__<<")\n"
    #define _TLnn(msg)  std::cerr<<(msg)<<"\n"
#endif
#endif

#ifndef VERBOSE
    #define _TL(msg)    
    #define _TLn(msg)   
    #define _TLnn(msg)  
#endif


#ifdef USE_SERIAL
    #define STARTMARK   "###"
    #define STARTLEN    3
#else
    #define STARTMARK   ""  
    #define STARTLEN    0
#endif

#define GNODE_ASSERT(expr,msg)           \
do {                                    \
    if (!(expr)) {                      \
        _TL ("Assertion failed [");     \
        _TL (#expr);                    \
        _TL ("] : " );                     \
        _TLn (msg);                     \
        abort();                        \
    }                                   \
} while (0)


#endif
