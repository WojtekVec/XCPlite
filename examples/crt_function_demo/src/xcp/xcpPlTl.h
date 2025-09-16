#pragma once

#include <cstddef>

/* XCP 1.0 Transport Layer Software Interface */
/* Transport Layer Version 1.0 and 2.0 */


/* --------------------------------------------------------------------------- */
/* Version 1.0 */


#ifdef XCP_PL_LOCAL
  #define XCP_PL_API
#else
  #ifdef XCP_PL_EXPORTS
    #define XCP_PL_API __declspec(dllexport)
  #else
    #define XCP_PL_API __declspec(dllimport)
  #endif
#endif

/* Error codes */
#define VERR_QUEUE_IS_EMPTY          10      /* plReceive */
#define VERR_QUEUE_MSG_OVERRUN       1001    /* plReceive */
#define VERR_QUEUE_MSG_LOST          1002    /* plReceive */
#define VERR_QUEUE_MSG_CORRUPT       1003    /* plReceive */
#define VERR_QUEUE_MSG_IGNORTED      1004    /* plReceive */

#define VERR_TX_NOT_POSSIBLE         12      /* plTransmit */
#define VERR_HW_NOT_READY            120     /* plXXX */
#define VERR_HW_NOT_PRESENT          129     /* plXXX */
#define VERR_CANNOT_OPEN_DRIVER      201     /* plXXX */


/* Function prototypes */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef XCP_PL_DYNAMIC

  typedef unsigned int (*PLINIT)( void );
  typedef unsigned int (*PLOPEN)( void );
  typedef unsigned int (*PLCLOSE)( void );
  typedef unsigned int (*PLEXIT)( void );
  typedef unsigned int (*PLSETPARAMS)( void *blob );
  typedef unsigned int (*PLFLUSHTRANSMITQUEUE)( void );
  typedef unsigned int (*PLFLUSHRECEIVEQUEUE)( void );
  typedef unsigned int (*PLTRANSMIT)( unsigned int len, unsigned char *msg );
  typedef unsigned int (*PLSETNOTIFICATION)(ptrdiff_t *pHandle, int queueLevel );
  typedef unsigned int (*PLRECEIVE)( unsigned int *lenp, unsigned char **msgp );

#else

  XCP_PL_API unsigned int plInit( void );
  XCP_PL_API unsigned int plOpen( void );
  XCP_PL_API unsigned int plClose( void );
  XCP_PL_API unsigned int plExit( void );
  XCP_PL_API unsigned int plSetParams( void *blob );
  XCP_PL_API unsigned int plFlushTransmitQueue( void );
  XCP_PL_API unsigned int plFlushReceiveQueue( void );
  XCP_PL_API unsigned int plTransmit( unsigned int len, unsigned char *msg );
  XCP_PL_API unsigned int plSetNotification(ptrdiff_t *pHandle, int queueLevel );
  XCP_PL_API unsigned int plReceive( unsigned int *lenp, unsigned char **msgp );

#endif




/* --------------------------------------------------------------------------- */
/* Version 2.0 */


/*
   ToDo:

   - Fehlercodes
       Transport Layer spezifische Codes
       Fehlertexte
   - Transport Layer spezifische Attribute der Daten (z.B CAN-Identifier)
   - Timestamps und aktueller Timerwert
   - Transport Layer Counter
   - Logischer Kanal
   - Versionsmanagement
   - Speicherfragen
   - Threadsicherheit
   - Editor für IF_DATA ?
*/

#ifdef XCP_TL_LOCAL
  #define XCP_TL_API
#else
  #ifdef XCP_TL_EXPORTS
    #define XCP_TL_API __declspec(dllexport)
  #else
    #define XCP_TL_API __declspec(dllimport)
  #endif
#endif


/* Data types */

typedef unsigned int XCP_TL_STATUS;
typedef unsigned int XCP_TL_HANDLE;
typedef void*        HANDLE;

/* declaration of incomplete types before using in function declaration */
/* gcc warning: "declared inside parameter list will not be visible outside of this definition or declaration" */
/* cl  warning: "named type definition in parentheses (C4115)" */

struct paramValue;
struct paramDesc;

/* Status codes */

#define XCP_TL_UNKNOWN_PARAMETER     1
#define XCP_TL_ALREADY_OPEN          2
#define XCP_TL_TX_NOT_POSSIBLE       3
#define XCP_TL_HW_NOT_READY          4
#define XCP_TL_HW_NOT_PRESENT        5
#define XCP_TL_CANNOT_OPEN_DRIVER    6

/* Communication */

XCP_PL_API XCP_TL_STATUS xcpTlInit( void );
XCP_PL_API XCP_TL_STATUS xcpTlExit( void );

XCP_TL_API XCP_TL_STATUS xcpTlOpenChannel( XCP_TL_HANDLE *h, unsigned int paramCount, struct paramValue *pParam, void *pIfData, HANDLE notificationEvent );
XCP_TL_API XCP_TL_STATUS xcpTlCloseChannel( XCP_TL_HANDLE h );

XCP_TL_API XCP_TL_STATUS xcpTlReceive( XCP_TL_HANDLE h, unsigned int *lenp, unsigned char *msg );
XCP_TL_API XCP_TL_STATUS xcpTlTransmit( XCP_TL_HANDLE h, unsigned int len, unsigned char *msg );

XCP_TL_API XCP_TL_STATUS xcpTlFlushReceiveQueue( XCP_TL_HANDLE h );
XCP_TL_API XCP_TL_STATUS xcpTlFlushTransmitQueue( XCP_TL_HANDLE h );

/* Generic parameter handling */

typedef XCP_TL_STATUS (*XCP_TL_CALLBACK)( char *parameterName, char *parameterValue);

#define XCP_TL_TYP_INT     0
#define XCP_TL_TYP_STRING  1
#define XCP_TL_TYP_ENUM    2

#include "pshpack1.h"

typedef struct paramDescription {
  char name[128];
  unsigned long type;
  char defaultValue[128];
  char **options; /* Array of strings in case of XCP_TL_TYP_ENUM */
} XCP_TL_PAR_DESC;

typedef struct parameterValue {
  char name[128];
  char value[128];
} XCP_TL_PAR_VAL;

#include "poppack.h"


XCP_PL_API XCP_TL_STATUS xcpTlGetParameterDefaults( XCP_TL_CALLBACK p );
XCP_TL_API XCP_TL_STATUS xcpTlGetChannelParameters( unsigned int *count, struct paramDesc **s );


#ifdef __cplusplus
}
#endif
