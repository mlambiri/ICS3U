/*
 * @author   dwlambiri
 * @date     Nov 17, 2019
 *
 * car-breaker-debug.h
 *
 */


#ifndef CARBREAKER_DEBUG_H
#define CARBREAKER_DEBUG_H

/*
 * @author   dwlambiri
 * @date     Nov 17, 2019
 *
 */


enum DEBUGENUM { debug_c =0, info_c = 1, warn_c = 2, error_c = 3};
extern bool debugon;
extern bool traceon;
extern int  debuglevel;

char* printDebugTime();
void  functionEnter();
void  functionExit();
char* printCallDepth();

//#define CARDEBUG

#ifdef CARDEBUG
#define DEBUG(s) if(debugon && debuglevel == debug_c) printf("D[%s %s:%4d] %s\n", printDebugTime(),__FILE__, __LINE__,s)
#define DEBUG2(s,d) if(debugon && debuglevel == debug_c) printf("D[%s %s:%4d] %s %d\n", printDebugTime(), __FILE__, __LINE__,s, d)
#define INFO(s) if(debugon && debuglevel <= info_c) printf("I[%s %s:%4d] %s\n", printDebugTime(), __FILE__, __LINE__,s)
#define WARN(s) if(debugon && debuglevel <= warn_c) printf("W[%s %s:%4d] %s\n", printDebugTime(), __FILE__, __LINE__,s)
#define ERROR(s) if(debugon && debuglevel <= error_c) printf("E[%s %s:%4d] %s\n", printDebugTime(), __FILE__, __LINE__,s)
#define FENTRY() functionEnter()
#define FEXIT()   functionExit()

#define TRACE() if(traceon) printf("T<%s: %s:%4d> %s%s;\n", printDebugTime(), __FILE__, __LINE__, printCallDepth(), __PRETTY_FUNCTION__)
#else
#define DEBUG(s)
#define DEBUG2(s,d)
#define INFO(s)
#define WARN(s)
#define ERROR(s)
#define TRACE()
#define FENTRY()
#define FEXIT()
#endif

#endif /* CARBREAKER_DEBUG_H */
