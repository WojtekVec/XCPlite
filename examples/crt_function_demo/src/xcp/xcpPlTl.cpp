// export the PL interface
#define XCP_PL_EXPORTS

#include "xcpConfig.h"

#ifdef XCP_TRANSPORT_LAYER_PL

#include "xcplib.h"
#include "xcpPlTl.h"

#ifdef __cplusplus
extern "C" 
{
#endif

#include "xcpQueue.h"

  // forward the required, external XCP functions
  uint8_t XcpCommand(const uint32_t *pCommand, uint8_t len);
  void XcpStart(tQueueHandle queueHandle, bool resumeMode);

#ifdef __cplusplus
} // extern "C"
#endif




/***************************************************************************/
/* Transport Layer Interface */

static struct 
{    
    bool isOnline = false;

    // Queue
    tQueueHandle TransmitQueue;

} gXcpServer;



HANDLE plNotifyEvent = 0;

extern "C" 
{
/* Initialize the transport layer interface */
/* This function will be called before any other */
/* This function will be called when CANape starts or when the CANape device is created */
/* Returns */
unsigned int plInit(void) 
{
  // XCP: Initialize the XCP singleton, activate XCP, must be called before starting the server
  // If XCP is not activated, the server will not start and all XCP instrumentation will be passive with minimal overhead
  XcpInit(true);

  // Create queue
  gXcpServer.TransmitQueue = QueueInit(Xcp::OptionQueueSize);
  if (gXcpServer.TransmitQueue == NULL)
  {
    return false;
  }

  // Start XCP protocol layer
  XcpStart(gXcpServer.TransmitQueue, false);

  // set the server to online state
  gXcpServer.isOnline = true;

  return 0;
}

/* Set the parameters of the transport layer */
unsigned int plSetParams(void *blob)
{
  /* Set the comunication parameters. */
  return 0;
}

/* Open the the comunication */
/* This function will be called after the comunication parameter have been set or changed */
unsigned int plOpen(void) 
{
  /* Establish the comunication */
  gXcpServer.isOnline = 1;

  return 0;
}

/* Close the comunication */
unsigned int plClose(void) 
{
  /* Finish the comunication */
  gXcpServer.isOnline = 0;

  return 0;
}

/* Deinitialize the transport layer interface */
unsigned int plExit(void) 
{
  /* Delete temporary files */
  /* Free memory */
  gXcpServer.isOnline = 0;

  return 0;
}

/* Clear the transmit queue */
unsigned int plFlushTransmitQueue() 
{ 
  return 0; 
}

/* Clear the receive queue */
unsigned int plFlushReceiveQueue() 
{
  return 0;
}

/* Transmit a XCP message */
/* Returns */
unsigned int plTransmit(unsigned int len, unsigned char *pData)
{
  if (gXcpServer.isOnline) 
  {
    const uint32_t *cmdBuf = (const uint32_t *)pData;
    XcpCommand(cmdBuf, len);
  }

  return 0;
}

/* Set the windows event handle to notify CANape when messages have been received */
unsigned int plSetNotification(ptrdiff_t *pHandle, int queueLevel) 
{
  plNotifyEvent = (HANDLE)*pHandle;

  /* Use SetEvent(plNotifyEvent); */
  /* to notify CANape that plReceive has pending messages */
  return 0;
}

/* Receive a XCP message */
/* Return VERR_QUEUE_IS_EMPTY, if no message has been received */
/* Will be called after Notification or at least every 10 ms */
/* plReceive will be immediately called again if the the queue was not empty */
unsigned int plReceive(unsigned int *lenp, unsigned char **msgp) 
{
  /* Check the send queue of the XCP driver */
  if (gXcpServer.isOnline) 
  {
    // TODO: the call to plReceive expcets XCP data to be written to the message data pointer with the corresponding length
    return 0;
  }
  return VERR_QUEUE_IS_EMPTY; /* Queue is empty */
}

} // extern "C"

#endif // XCP_TRANSPORT_LAYER_ETH