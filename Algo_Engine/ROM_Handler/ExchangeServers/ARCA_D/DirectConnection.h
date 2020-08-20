/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *  : Defines the class behaviors for the application.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *     Parnell     
 *
 *******************************************************************************/
#ifndef DIRECTCONNECTION_H__
#define DIRECTCONNECTION_H__

#include <sstream>
#include "afxsock.h"

#include "BaseConnection.h"

static const int LEFT_OVER_LEN = 66;
/**
 * DirectConnection is an implementation of CBaseConnection that does not
 * use CString to format the data given to the Send function inherited from
 * CAsyncSocket.
 */
class DirectConnection : public CBaseConnection, public CAsyncSocket {

private:

    char leftOver_[LEFT_OVER_LEN];
    std::stringstream data_;
    CString  m_Host;
    int m_Port;
    int m_LocalPort;
    int m_RecvBufSize;
    char wholeMessage_[0x4000];
    CString m_DatFile;
    int m_lastBytesRead;
    /*Inherited from CBaseConnection.*/
    void ReadParameters(const CString& AppName,
                        const CString& Path);
    CString GetErrorText(int nErrorNum);
    void PostError(int ErrorCode,
                   const char* message, BOOL bClose = TRUE);
    void ProcessMessage(std::stringstream& data, int length);

public:
    /*Constructor destructor*/
    DirectConnection(const CString& name, const CString& appName, 
                     const CString& path);
    DirectConnection(const CString& name, const CString& hostName,
                     const int port, const int localPort = 0);
    DirectConnection();

    virtual ~DirectConnection() {}

    /*Functions inherited from BaseConnection.*/
    virtual void ConnectToHost(int port = 0);
    /*Gets the we are connected to or returns the error.*/
    virtual CString GetConnector() {
        CString ip;
        unsigned int port;
        if(GetPeerName(ip, port)) {
            return ip;
        }
        return GetErrorText(GetLastError());
    }
    virtual void Close() { 
        if (m_hSocket != INVALID_SOCKET)
        {
            CAsyncSocket::AsyncSelect(0);
            CAsyncSocket::Close();
            OnClose(0);
        }
    }

    virtual	void OnClose(int nErrorCode);
    virtual	void OnConnect(int nErrorCode);
    virtual	void OnReceive(int nErrorCode);
    virtual void OnSend(int nErrorCode);
    virtual	void PostError(int ErrorCode,
                               BOOL bClose = TRUE, CString l_ErrorMsg = "");


    
    /*Getters for the private members.*/
    const CString& Host() const { return m_Host; }
    const int Port() const { return m_Port; }
    const int LocalPort() const { return m_LocalPort; }
    const BOOL IsConnected() const { return m_Connected; }

    /*Are SendMessage function the does not format the "message".*/
    virtual BOOL SendDirectMessage(const char* message,
                                   int length, BOOL force = FALSE);
};


#endif //DIRECTCONNECTION_H__
