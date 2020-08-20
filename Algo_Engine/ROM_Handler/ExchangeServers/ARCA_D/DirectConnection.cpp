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
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "ArcaDirect.h"
#include "DirectConnection.h"


DirectConnection::DirectConnection(const CString& name,
                                   const CString& appName,
                                   const CString& path) 
    :CBaseConnection(), CAsyncSocket(),
     data_(std::stringstream::binary | std::stringstream::in | std::stringstream::out)
{
    m_Name = name;
    m_hSocket = INVALID_SOCKET;
    m_RecvBufSize = 0x4000;
    ReadParameters(appName, path);
        

    m_LastReception		= CTime::GetCurrentTime();
    m_LastTransmission	= m_LastReception;
	
    CMessageApp *pApp = (CMessageApp *) AfxGetApp();
    SetLogonStatus(FALSE);

    m_Reconnect = GetPrivateProfileInt(m_Name + " Connection",
                                       "Retry", 1, pApp->DatFile());
    ConnectToHost(m_LocalPort);

    m_Char = '\n';
    memset(wholeMessage_, '\0', 0x4000);
    m_lastBytesRead = 0;
}

DirectConnection::DirectConnection(const CString& name, 
                                   const CString& hostName,
                                   const int port,
                                   const int localPort)
                                   :CBaseConnection(),
                                    CAsyncSocket(),
                                    data_(std::stringstream::binary | std::stringstream::in | std::stringstream::out)
{
    m_Name = name;
    m_Host = hostName;
    m_Port = port;
    m_LocalPort = localPort;
    m_hSocket = INVALID_SOCKET;
    m_RecvBufSize = 0x4000;

    m_LastReception = CTime::GetCurrentTime();
    m_LastTransmission	= m_LastReception;
	
    CMessageApp *pApp = (CMessageApp *) AfxGetApp();
    SetLogonStatus(FALSE);
    m_Reconnect = GetPrivateProfileInt(m_Name + " Connection",
                                       "Retry", 1, pApp->DatFile());
    ConnectToHost(m_LocalPort);

    m_Char = '\n';
    memset(wholeMessage_, '\0', 0x4000);
    m_lastBytesRead = 0;

}

DirectConnection::DirectConnection()
    :CBaseConnection(), CAsyncSocket(),
     data_(std::stringstream::binary | std::stringstream::in | std::stringstream::out)
{
    m_Connected = TRUE;
    m_ConnectionTime = 
        CTime::GetCurrentTime().Format("%H:%M:%S");
    m_RecvBufSize = 0x4000;
    
    m_Char = '\n';
    memset(wholeMessage_, '\0', 0x4000);
    m_lastBytesRead = 0;
}



void DirectConnection::ReadParameters(const CString& appName,
                                      const CString& path)
{
    
    CArcaDirectApp *pApp = (CArcaDirectApp*)AfxGetApp();
    CString localCat = m_Name + " Connection";
    CString datFile = pApp->DatFile();
    m_Port = GetPrivateProfileInt(
        localCat, 
        "Port", 0, datFile);
    m_LocalPort = GetPrivateProfileInt(
        localCat, "LocalPort", 0, datFile);
    GetPrivateProfileString(localCat, "Host","",
        m_Host.GetBuffer(128), 128, datFile);
    m_Host.ReleaseBuffer();       
}


BOOL DirectConnection::SendDirectMessage(const char* message, int length,
                                   BOOL force)
{
    BOOL success = FALSE;
    if(!BlockSend() || force) {
        
        int bytesSent = 0;
        int localLength = static_cast<int>(length);
        int remainSent = localLength;
        
        if(m_Connected) {
            BOOL disconnect = FALSE;
            while(remainSent > 0 && !disconnect) {           
                if((bytesSent = Send(message, remainSent)) == localLength) {
                    m_Sent++;
                    success = TRUE;    
                } else if(bytesSent == SOCKET_ERROR) {
                    PostError(GetLastError());
                    disconnect = TRUE;
                }
                remainSent -= bytesSent;
                message += bytesSent;              
            }
        }
    }
    return success;
}

void DirectConnection::ConnectToHost(int port)
{
    if(port < 0) {
        port = m_Port;
    }
    int error;
    if(m_hSocket != INVALID_SOCKET) {
        Close();
    } else {
        if(Create(port)) {
            AsyncSelect(FD_READ | FD_WRITE |
                FD_CONNECT | FD_CLOSE );
            if(!Connect(m_Host, m_Port)) {
                if((error = GetLastError()) != WSAEWOULDBLOCK) {
                    PostError(error);
                } 
            } else {
                ((CArcaDirectApp*)AfxGetApp())->ProcessLogEntry(
                    m_Name, "Connecting To Host...");
            } 
            
        } else {
            PostError(GetLastError());
        }
    }  
}

void DirectConnection::OnClose(int errorCode)
{
    m_Connected = FALSE;
    m_LoggedOn = FALSE;
    if(errorCode) {
        PostError(errorCode, FALSE,
            GetErrorText(errorCode) +"Connection Lost OnClose DirectConnection ");
    } 
    
    ((CArcaDirectApp*)AfxGetApp())->ProcessLogEntry(
            m_Name, "Closing connection in OnClose in DirectConnection: " + m_Host);
    CAsyncSocket::OnClose(errorCode); 
}

void DirectConnection::OnConnect(int errorCode)
{
    CMessageApp	*pApp = (CMessageApp *)AfxGetApp();
    if(!errorCode) {
        m_Connected = TRUE;
        m_ConnectionTime = 
            (CTime::GetCurrentTime()).Format("%Y%m%d-%H:%M:%S");
        ((CArcaDirectApp*)AfxGetApp())->ProcessLogEntry(
            m_Name, "Connected To " + m_Host);
    } else {
        m_Connected = FALSE;
        ((CArcaDirectApp*)AfxGetApp())->ProcessLogEntry(
            m_Name, "Could Not Connect To " + m_Host);
        PostError(GetLastError());
    }
    CAsyncSocket::OnConnect(errorCode);
    if(!errorCode) {
        if(SetSockOpt(SO_SNDBUF,&m_RecvBufSize,sizeof(int),SOL_SOCKET) == 0)
            PostError(GetLastError(),FALSE,"SetSockOpt fail");
        if(SetSockOpt(SO_RCVBUF,&m_RecvBufSize,sizeof(int),SOL_SOCKET) == 0)
            PostError(GetLastError(),FALSE,"SetSockOpt fail");
		BOOL bValue = TRUE;
		SetSockOpt(TCP_NODELAY, (void*)&bValue, sizeof(bValue), IPPROTO_TCP);

        PostError(0,FALSE,"Connected to host");
    }
    pApp->OnHostConnect(*this, errorCode);
}

void DirectConnection::OnReceive(int errorCode)
{
    int bytesRead = 0;
    bool disconnect = false;   
    if(m_hSocket != INVALID_SOCKET) {
        /*Queue events while we are processing.*/
        AsyncSelect(0);
        memset(wholeMessage_, '\0',m_lastBytesRead); 
        /*Grab as much as we can from the buffer.*/
        bytesRead = Receive(wholeMessage_, m_RecvBufSize);
        m_lastBytesRead = bytesRead;
        /*Check for errors.*/
        if(bytesRead == SOCKET_ERROR) {
            PostError(GetLastError());
        } else if(bytesRead == 0) {
            /*If we get here we know that the other side
              has gone away.*/
            PostError(GetLastError());
            ((CArcaDirectApp*)AfxGetApp())->ProcessLogEntry(
                m_Name, "Host Closed Socket");
        } else {
            data_.write(wholeMessage_, bytesRead);
            int length = data_.tellp();
            data_.seekg (0, std::ios::beg); 
            DirectConnection::ProcessMessage(data_, length);
        }
        // turn events back on
        if (m_hSocket != INVALID_SOCKET) {
            /* check to make sure socket wasn't
               closed on us during read 
               (error or bytesread = 0) */
            AsyncSelect();
        }
    }
    
}

void DirectConnection::OnSend(int errrorCode)
{
}


CString DirectConnection::GetErrorText(int nErrorCode)
{
    switch (nErrorCode)
    {
    case 0:
        return "";
        break;
    case WSANOTINITIALISED:
        return("A successful AfxSocketInit must occur before using this API.");
        break;
    case WSAENETDOWN:
        return("The Windows Sockets implementation detected that the network subsystem failed.");
        break;
    case WSAEADDRINUSE:
        return("The specified address is already in use.");
        break;
    case WSAEINPROGRESS:
        return("A blocking Windows Sockets call is in progress.");
        break;
    case WSAEADDRNOTAVAIL:
        return("The specified address is not available from the local machine.");
        break;
    case WSAEAFNOSUPPORT:
        return("Addresses in the specified family cannot be used with this socket.");
        break;
    case WSAECONNREFUSED:
        return("The attempt to connect was rejected.");
        break;
    case WSAEDESTADDRREQ:
        return("A destination address is required.");
        break;
    case WSAEFAULT:
        return("The nSockAddrLen argument is incorrect.");
        break;
    case WSAEINVAL:
        return("The socket is not already bound to an address.");
        break;
    case WSAEISCONN:
        return("The socket is already connected.");
        break;
    case WSAEMFILE:
        return("No more file descriptors are available.");
        break;
    case WSAENETUNREACH:
        return("The network cannot be reached from this host at this time.");
        break;
    case WSAENOBUFS:
        return("No buffer space is available. The socket cannot be connected.");
        break;
    case WSAENOTSOCK:
        return("The descriptor is not a socket.");
        break;
    case WSAETIMEDOUT:
        return("Attempt to connect timed out without establishing a connection.");
        break;
    case WSAEWOULDBLOCK:
        return("The socket is marked as nonblocking and the connection cannot be completed immediately");
        break;
    case WSAECONNABORTED:
        return("An established connection was aborted by the software in your host machine");
        break;
    case WSAECONNRESET:
        return("An existing connection was forcibly closed by the remote host");
        break;
    default:
        return("Unkown Socket Error");
        break;
    };
}




void DirectConnection::PostError(int errorCode, 
                                 BOOL close, CString l_ErrorMsg)
{
    if(IsHostConnection() && m_Connected) {
        ((CMessageApp*)AfxGetApp())->ProcessErrorEx(
            M_ERR_NOTIFY_EL_INFORMATION,
            m_Name, GetErrorText(errorCode) + " "+ l_ErrorMsg);
    } else {
        ((CMessageApp*)AfxGetApp())->ProcessErrorEx(
            MODE_ERR_BASIC, m_Name,
            GetErrorText(errorCode) + " "+ l_ErrorMsg);
    }
    
    if(close) {
        Close();
        m_Connected = FALSE;
    }
}

void DirectConnection::ProcessMessage(std::stringstream& data,
                                      int length)
{
    CArcaDirectApp *pApp = (CArcaDirectApp*)AfxGetApp();   
    int totalUsed = pApp->ProcessHostMessage(*this, data, length);
    
    if(totalUsed >= length) {
        data_.seekg(0, std::ios::beg);
        data_.seekp(0, std::ios::beg);
    } else {
        int unused = length - totalUsed;
        if(unused > LEFT_OVER_LEN) {
            pApp->ProcessErrorEx(
                MODE_ERR_BASIC, m_Name,
                "LEFT OVER MESSAGE IS LONGER THAN THE LONGEST MESSAGE TYPE!!!");
            data_.seekg(0, std::ios::beg);
            data_.seekp(0, std::ios::beg);
            return;
            
        }
        memset(leftOver_, '\0', LEFT_OVER_LEN);
        data_.read(leftOver_, unused);
        data_.seekp(0, std::ios::beg);
        data_.write(leftOver_, unused);
        data_.seekg(0, std::ios::beg);
    }
}
