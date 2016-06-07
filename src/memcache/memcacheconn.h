

#ifndef MEMCACHECONN_H
#define MEMCACHECONN_H

#include <edio/eventreactor.h>

#include <socket/gsockaddr.h>

#include <util/loopbuf.h>
#include <util/autostr.h>
#include <sys/stat.h>

#include <string>
#include <zlib.h>

#include "repl/replpacket.h"
//#include "replicate/replpeer.h"

/* _ConnFlags */
#define CS_INTERNAL     0x01    // internal connection
#define CS_REMBUSY      0x02    // remote is busy
#define CS_RESPWAIT     0x04    // waiting for remote response
#define CS_CMDWAIT      0x08    // waiting to process new command

class ReplPacketHeader;

struct sockaddr;
class Multiplexer;
class LsMcSasl;
struct ReplPacketHeader;

class MemcacheConn : public EventReactor
{

public:
    static time_t   s_curTime;
    enum
    {
        CS_DISCONNECTED,
        CS_CONNECTING,
        CS_CONNECTED,
        CS_CLOSING,
        CS_SHUTDOWN,
        CS_CLOSED
    };

    enum
    {
        MC_UNKNOWN,
        MC_ASCII,
        MC_BINARY,
        MC_INTERNAL
    };

    MemcacheConn();
    ~MemcacheConn();
    
    int InitConn( int fd, struct sockaddr * addr );
    int CloseConnection();
    int SendEx(const char *msg, int len, int flags);
    int SendBuff(const char *msg, int len, int flags);
    int flush();

    bool IsSendBufferFull()
    {   return m_bufOutgoing.size() >= m_iMaxBufferSize; }
    
    bool canSendDataObject(int tobesend)
    {   return m_bufOutgoing.empty() || (m_bufOutgoing.size() + tobesend < 8192); }
    
    int connectTo( const GSockAddr * pServerAddr );
    
    void SetMultiplexer( Multiplexer * pMplx )  {   m_pMultiplexer = pMplx;  }
    
    const char * GetPeerAddr() const        {  return m_peerAddr.c_str();   }
    
    virtual int handleEvents( short events );

    virtual void onTimer();

    static void SetConnTimeout( int t )     {   s_ConnTimeout = t;   }

    int sendACKPacket( int type , ReplPacketHeader * header );
    int processIncoming();

    void continueWrite();
    
    int GetConnState( ){ return m_iConnState; }
    long GetLastActiveTime () { return m_iLastActiveTime; }

    LsMcSasl * GetSasl() const             {   return _pSasl;   }
    uint8_t     GetProtocol() const         {   return _Protocol;   }
    uint8_t     GetConnFlags() const        {   return _ConnFlags;   }
    void        SetConnInternal()           {   _Protocol = MC_INTERNAL;   }
    void        SetRemoteBusy()             {   _ConnFlags |= CS_REMBUSY;   }
    void        ClrRemoteBusy()             {   _ConnFlags &= ~CS_REMBUSY;   }
    void        SetRespWait()               {   _ConnFlags |= CS_RESPWAIT;   }
    void        ClrRespWait()               {   _ConnFlags &= ~CS_RESPWAIT;   }
    void        SetCmdWait()                {   _ConnFlags |= CS_CMDWAIT;   }
    void        ClrCmdWait()                {   _ConnFlags &= ~CS_CMDWAIT;   }
    MemcacheConn * GetLink() const          {   return _pLink;   }
    void        SetLink(MemcacheConn *pLink){   _pLink = pLink;   }
    
private:    
    int onRead();
    int onWrite();
    int onHup();
    int onErr();

    int Read( char * pBuf, int len );
 
    int protocolErr();
    int forwardNoneMplx();
    
    int Flush();
    int onInitConnected();
    
    Multiplexer * getMultiplexer() const    {   return m_pMultiplexer;   }
    
private:
    int             m_iConnState;
    long            m_iLastActiveTime;
    
    AutoStr         m_peerAddr;
    int             m_iSSPort;
    GSockAddr       _ClientIP;
        
    int             m_iMaxBufferSize;
    LoopBuf         m_bufOutgoing;
    LoopBuf         m_bufIncoming;
    uint8_t         _Protocol;      // binary or ascii
    uint8_t         _ConnFlags;
    
    Multiplexer *   m_pMultiplexer;
    LsMcSasl *     _pSasl;
    MemcacheConn *  _pLink;
    static int      s_ConnTimeout;
    
};

#endif