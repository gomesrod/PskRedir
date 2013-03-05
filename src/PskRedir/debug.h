
// undef this to disable debugging
// # define PSKDEBUG

#ifdef PSKDEBUG

#include <cstdio>

/** Sends a literal message to debug output */
#define DEBUGMSG(msg) printf("[DEBUG][%s:%d] %s\n", __FILE__, __LINE__, msg);

/** Sends a char* var to debug output */
#define DEBUGSTR(msg) printf("[DEBUG][%s:%d] %s : %s\n", __FILE__, __LINE__, #msg, msg);

/** Sends a int var to debug output */
#define DEBUGNUM(val) printf("[DEBUG][%s:%d] %s : %d\n", __FILE__, __LINE__, #val, val);

#else

#define DEBUGMSG(msg) (void)0
#define DEBUGSTR(msg) (void)0
#define DEBUGNUM(msg) (void)0

#endif
