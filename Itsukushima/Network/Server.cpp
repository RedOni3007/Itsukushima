#include "Server.h"	
#include <thread>

#include "SocketHeaders.h"
#include <Network/ServerControl.h>
#include "RemoteIO.h"
#include <Game/Scene.h>
#include <App/Timer.h>
#include "BaseNetworkStruct.h"
#include <Core/GlobalSetting.h>
#include "EncryptionHelper.h"

#define PRINT_ADDRINFO 0


Server::Server(void)
{
	m_eStatus = ServerStatus::NOT_STARTED;
	m_eSendThreadStatus = ServerStatus::STOPPED;
	m_nListenSocketFD = -1;
	m_nMaxSocketFD = -1;
	m_pMasterFDSet = nullptr;
	m_pScene = nullptr;
	m_pServerControl = nullptr;

	m_nCurrentDataLen = -1;
	m_nCurrentDataReceived = -1;

	m_pCurrentDataBuffer = new int8[SOCKET_RECV_BUFFER_SIZE * 2];
	m_pTempDataPacket = new NS_DataPacket();
}
	
Server::~Server(void)
{
	End();

	m_pScene = nullptr;
	m_pServerControl = nullptr;

	if(m_pMasterFDSet != nullptr)
	{
		delete m_pMasterFDSet;
		m_pMasterFDSet = nullptr;
	}

	if(m_pCurrentDataBuffer != nullptr)
	{
		delete[] m_pCurrentDataBuffer;
		m_pCurrentDataBuffer = nullptr;
	}

	if(m_pTempDataPacket != nullptr)
	{
		delete m_pTempDataPacket;
		m_pTempDataPacket = nullptr;
	}
}

ServerStatus 
Server::GetStatus()
{
	return m_eStatus;
}

int32 
Server::GetListenSocketFD()
{
	return m_nListenSocketFD;
}

void 
Server::SetScene(Scene* pScene)
{
	m_pScene = pScene;
}

Scene* 
Server::GetScene()
{
	return m_pScene;
}

void 
Server::StartOnThread(const char* pszHost,const char* pszPort)
{
	std::thread serverMain(&Server::Run, this, pszHost,pszPort);
	serverMain.detach();//no need to track this thread, just kick start
}

bool 
Server::Run(const char* pszHost,const char* pszPort)
{

	if(	m_eStatus != ServerStatus::NOT_STARTED)
		return false;

	while(m_pScene == nullptr)
	{
		//wait the screen to init the scene
		Sleep(16);
	}

	GameObject* pTempObject = m_pScene->GetGameObject("Server");//server must be this name
	Debug::Assert(pTempObject != nullptr);
	m_pServerControl = pTempObject->GetServerControlComponent();
	Debug::Assert(m_pServerControl != nullptr);
	m_pServerControl->SetServer(this);

	int32 status;
	struct addrinfo hints;
	struct addrinfo *servinfo;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	hints.ai_protocol = IPPROTO_TCP;


	if ((status = getaddrinfo(pszHost, pszPort, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		Debug::Assert(false);
		return false;
	}

	//bind to the fisrt one which can bind
	m_nListenSocketFD = -1;
	bool yes = true;
	struct addrinfo *ptr;
	for(ptr=servinfo; ptr != NULL ;ptr=ptr->ai_next) 
	{
		if((m_nListenSocketFD = socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol)) < 0)
		{
			continue;
		}

		//reuse socket
		if(setsockopt(m_nListenSocketFD, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(int)) != 0)
		{
			//not really matter?
		}

		if(bind(m_nListenSocketFD,ptr->ai_addr, ptr->ai_addrlen) != 0)
		{
			closesocket(m_nListenSocketFD);
			m_nListenSocketFD = -1;
			continue;
		}

#if X_PC && PRINT_ADDRINFO
		struct sockaddr_in  *sockaddr_ipv4;
		struct sockaddr_in6 *sockaddr_ipv6;
		WCHAR ipstringbuffer[INET6_ADDRSTRLEN];
		DWORD ipbufferlength = INET6_ADDRSTRLEN;
		Debug::Log("Binded Socket:\n");
		Debug::Log("\tFlags: 0x%x\n", ptr->ai_flags);
		Debug::Log("\tFamily: ");
		switch (ptr->ai_family) 
		{
			case AF_UNSPEC:
				Debug::Log("Unspecified\n");
				break;
			case AF_INET:
				Debug::Log("AF_INET (IPv4)\n");
				sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
				Debug::Log("\tIPv4 address %s\n",
					inet_ntoa(sockaddr_ipv4->sin_addr) );
				break;
			case AF_INET6:
				Debug::Log("AF_INET6 (IPv6)\n");
				// the InetNtop function is available on Windows Vista and later
				 sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
				 Debug::Log("\tIPv6 address %s\n",
				    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, INET6_ADDRSTRLEN) );
				break;
			case AF_NETBIOS:
				Debug::Log("AF_NETBIOS (NetBIOS)\n");
				break;
			default:
				Debug::Log("Other %ld\n", ptr->ai_family);
				break;
		}
		Debug::Log("\tSocket type: ");
		switch (ptr->ai_socktype) 
		{
			case 0:
				Debug::Log("Unspecified\n");
				break;
			case SOCK_STREAM:
				Debug::Log("SOCK_STREAM (stream)\n");
				break;
			case SOCK_DGRAM:
				Debug::Log("SOCK_DGRAM (datagram) \n");
				break;
			case SOCK_RAW:
				Debug::Log("SOCK_RAW (raw) \n");
				break;
			case SOCK_RDM:
				Debug::Log("SOCK_RDM (reliable message datagram)\n");
				break;
			case SOCK_SEQPACKET:
				Debug::Log("SOCK_SEQPACKET (pseudo-stream packet)\n");
				break;
			default:
				Debug::Log("Other %ld\n", ptr->ai_socktype);
				break;
		}
		Debug::Log("\tProtocol: ");
		switch (ptr->ai_protocol) 
		{
			case 0:
				Debug::Log("Unspecified\n");
				break;
			case IPPROTO_TCP:
				Debug::Log("IPPROTO_TCP (TCP)\n");
				break;
			case IPPROTO_UDP:
				Debug::Log("IPPROTO_UDP (UDP) \n");
				break;
			default:
				Debug::Log("Other %ld\n", ptr->ai_protocol);
				break;
		}
		Debug::Log("\tLength of this sockaddr: %d\n", ptr->ai_addrlen);
		Debug::Log("\tCanonical name: %s\n", ptr->ai_canonname);
#endif
		break;
	}
	freeaddrinfo(servinfo);

	if(	m_nListenSocketFD == -1)
	{
		//not found any socket to blind
		Debug::Assert(false);
		return false;
	}

	if(listen(m_nListenSocketFD,20) != 0)
	{
		Debug::Assert(false);
		return false;
	}

	{
		//turn off Nagle algorithm, hope the small size data will be sent faster
		int flag = 1;
		setsockopt(m_nListenSocketFD, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(int));
	}
	m_eStatus = ServerStatus::RUNNING;

	//everything is fine, so kick start the sending manage thread
	std::thread sendMain(&Server::SendingManageThread, this);
	sendMain.detach();//no need to track this thread, just kick start

	//kick the udp broadcast thread
	std::thread udpsendMain(&Server::UDP_Iamhere_thread, this);
	udpsendMain.detach();//no need to track this thread, just kick start

	struct sockaddr_storage remoteaddr;// client address
	socklen_t addrlen = sizeof(remoteaddr);
	int32 nAcceptedSocketFD = -1;

	char remoteIP[INET6_ADDRSTRLEN];
	char recvBuffer[SOCKET_RECV_BUFFER_SIZE];

	m_nMaxSocketFD = m_nListenSocketFD;

	if(m_pMasterFDSet == nullptr)
		m_pMasterFDSet = new fd_set();

	FD_ZERO(m_pMasterFDSet);
	FD_SET(m_nListenSocketFD,m_pMasterFDSet);

	fd_set sReadFDSet;
	FD_ZERO(&sReadFDSet);

	struct timeval tv;//every half second
	tv.tv_sec = 0;
	tv.tv_usec = 500000;

	int32 socketToRead = 0;
	int32 nBytes = -1;
	uint32 i = 0;

	bool bBreakByError = false;
	while(m_eStatus == ServerStatus::RUNNING)
	{
		sReadFDSet = *m_pMasterFDSet; // copy, hope not to slow when the master is huge 
        if (select(m_nMaxSocketFD+1, &sReadFDSet, NULL, NULL, &tv)  < 0)//0 is timeout
		{
           bBreakByError = true;
           break;
        }

		if(FD_ISSET(m_nListenSocketFD, &sReadFDSet))
		{
			//incoming connnect request
			nAcceptedSocketFD = accept(m_nListenSocketFD, (struct sockaddr *)&remoteaddr, &addrlen);
			if(nAcceptedSocketFD < 0)
			{
				//something wrong, should I just ignore it?
			}
			else
			{
				{
					//turn off Nagle algorithm, hope the small size data will be sent faster
					int flag = 1;
					setsockopt(nAcceptedSocketFD, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(int));
				}

				FD_SET(nAcceptedSocketFD, m_pMasterFDSet); // add to master set
                if (nAcceptedSocketFD > m_nMaxSocketFD) 
				{
					// keep track of the max
                    m_nMaxSocketFD = nAcceptedSocketFD;
                }

				Debug::Log("selectserver: new connection from %s on socket %d\n",
                        inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), nAcceptedSocketFD);

				std::thread processNewClientThread(&Server::AddNewClient, this, nAcceptedSocketFD);
				processNewClientThread.detach();//not sure if later accepted thread won't go ahead, but should no matter

			}
		}
		else
		{
			//incoming data
			for(i = 0; i < sReadFDSet.fd_count; ++i)
			{
				if(i == m_nListenSocketFD)
					continue;//not to recv on listening Socket

				socketToRead = sReadFDSet.fd_array[i];
				nBytes = recv(socketToRead, recvBuffer, SOCKET_RECV_BUFFER_SIZE,0);
				if(nBytes <= 0)
				{
					//disconnect, or something wrong
					Debug::Log("Client Socket %d closed.\n",socketToRead);

					std::thread processSocketCloseThread(&Server::RemoveClient, this, socketToRead);
					processSocketCloseThread.detach();

					closesocket(socketToRead);
					FD_CLR(socketToRead, m_pMasterFDSet); 
				}
				else
				{
					//get some data
					//Debug::Log("Received %d bytes on Client Socket %d.\n", nBytes, socketToRead);

					ReceivedData(socketToRead, recvBuffer, nBytes);

					//too expensive to create the thread in realtime, need a thread pool
					//std::thread processSocketDataReceivedThread(&Server::ReceivedData, this, socketToRead, recvBuffer, nBytes);
					//processSocketDataReceivedThread.detach();
				}
			
			}//for(uint32 i = 0; i < sReadFDSet.fd_count; ++i)
		}//if(FD_ISSET(m_nListenSocketFD, &sReadFDSet))
	}//while(m_eStatus == ServerStatus::RUNNING)

	closesocket(m_nListenSocketFD);
	m_nListenSocketFD = -1;

	m_eStatus = ServerStatus::STOPPED;

	return true;
}

void 
Server::StartSend()
{
	if(m_eSendThreadStatus != ServerStatus::STOPPED)
	{
		//todo: find out why the previous sending is not finished
		//Debug::Assert(false,"previous sending does not finished yet, so skip this sending");
		return;
	}

	m_sendQueue.clear();

	std::list<void*> allRemoteIO;
	//GetComponnets could be slow if object's number is huge, because of string compare, need a cache in the future
	m_pScene->GetComponents(RemoteIO::ClassName(), allRemoteIO);

	RemoteIO* pRemoteIO = nullptr;
	RefCountPtr<NS_Data> pData = nullptr;

	//check which remoteIOs need to send data
	for(std::list<void*>::iterator it = allRemoteIO.begin(); it != allRemoteIO.end(); ++it)
	{
		pRemoteIO = (RemoteIO*)(*it);
		pData = pRemoteIO->GetCurrnetDataToSend();
		if(pData != nullptr)
		{
			m_sendQueue.push_back(pRemoteIO);
		}
	}

	m_eSendThreadStatus = ServerStatus::READY;
}

bool 
Server::SendingManageThread()
{
	fd_set sSendFDSet;
	FD_ZERO(&sSendFDSet);

	//timeout is 90% of single frame time
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = (long)(Timer::Instance()->GetDeltaTime() * 900);//should always be fixed framerate

	NS_Data* pData = nullptr;

	int32 nSocketFDToSend = -1;
	bool bBreakByError = false;
	while(m_eStatus == ServerStatus::RUNNING)
	{
		if(m_sendQueue.size() == 0)
		{
			StartSend();

			if(m_sendQueue.size() == 0)
				Sleep(10);//don't do a forever check, sleep a little little while
		}

		while(m_sendQueue.size() > 0 && m_eSendThreadStatus == ServerStatus::READY)
		{
			m_eSendThreadStatus =  ServerStatus::RUNNING;

			sSendFDSet = *m_pMasterFDSet; // copy, hope not to slow when the master is huge 
			if (select(m_nMaxSocketFD+1, NULL, &sSendFDSet, NULL, &tv)  < 0)//0 is timeout
			{
				bBreakByError = true;
				break;
			}

			//select return something, start check if the socket is ready to send
			std::list<RemoteIO*>::iterator it;
			for(it = m_sendQueue.begin(); it != m_sendQueue.end();)
			{
				nSocketFDToSend = (*it)->GetSocketFD();
				if(nSocketFDToSend < 0)
				{
					it = m_sendQueue.erase(it);
				}
				else if(FD_ISSET(nSocketFDToSend, &sSendFDSet))
				{
					//ceate thread is too slow, need a way to pool it, current just send the data in this thread
					SendData((*it));
					it = m_sendQueue.erase(it);
				}
				else
				{
					if((*it)->GetGameObject()->IsDead())
					{
						it = m_sendQueue.erase(it);
					}
					else
					{
						++it;
					}
				}
			}

			if(m_eStatus != ServerStatus::RUNNING)
				break;			
		}

		m_eSendThreadStatus = ServerStatus::STOPPED;
	}

	m_eSendThreadStatus = ServerStatus::STOPPED;

	return true;
}

bool 
Server::SendData(RemoteIO* pRemoteIO)
{
	RefCountPtr<NS_Data> pData = pRemoteIO->GetCurrnetDataToSend();

	int32 nSocketFD = pRemoteIO->GetSocketFD();
	int32 nBytesLeft = pData->size;
	int32 nBytesSent = 0;
	int32 nLastSentBytes = 0;
	while(nBytesLeft > 0)
	{
		nLastSentBytes = send(nSocketFD, &pData->pData[nBytesSent], nBytesLeft,0);
		if(nLastSentBytes <= 0)
		{
			//Debug::Assert(false,"closed while sending");
			return false;
		}

		nBytesSent += nLastSentBytes;
		nBytesLeft -= nLastSentBytes;
	}

	pData = nullptr;
	pRemoteIO->CurrentDataSendDone();

	return true;
}

void
Server::End()
{
	if(	m_eStatus != ServerStatus::RUNNING)
		return;

	if(m_pMasterFDSet!=nullptr)
	{
		FD_ZERO(m_pMasterFDSet);
	}
	
	m_eStatus = ServerStatus::STOPING;
}

bool 
Server::IsStopped()
{
	return 	(m_eStatus == ServerStatus::STOPPED) && (m_eSendThreadStatus == ServerStatus::STOPPED);
}

void 
Server::AddNewClient(int32 nSocketFD)
{
	m_pServerControl->AddNewClient(nSocketFD);
}

void 
Server::RemoveClient(int32 nSocketFD)
{
	m_pServerControl->RemoveClient(nSocketFD);
}

void 
Server::ReceivedData(int32 nSocketFD, char* pData, int32 nByteLen)
{
	EncryptionHelper::DecryptXOR(pData,nByteLen);

	//WinSocket will combine the data, so need to check
	int currentIndex = 0;

	while(true)
	{
		if(m_nCurrentDataLen < 0)
		{
			m_nCurrentDataLen = *(int32*)&pData[currentIndex];
		}
		if(m_nCurrentDataReceived < 0)
		{
			m_nCurrentDataReceived = 0;
		}

		if(m_nCurrentDataLen <= 0 || m_nCurrentDataLen > 2048)
		{
			//something corrupted, skip this one
			m_nCurrentDataLen = -1;
			m_nCurrentDataReceived = -1;
			return;
		}

		if(m_nCurrentDataLen <= nByteLen)
		{
			memcpy(&m_pCurrentDataBuffer[m_nCurrentDataReceived], &pData[currentIndex], m_nCurrentDataLen);
			m_nCurrentDataReceived += m_nCurrentDataLen;
			currentIndex += m_nCurrentDataLen;
		}
		else
		{
			Debug::Assert(false, "if this happen, means WinSocket combine the data no matter whether the data can send in one go or not, bad MS");
			//hasn't test, hope it correct (unlikely)
			int32 nBytesToCopy = nByteLen - currentIndex -1;
			memcpy(&m_pCurrentDataBuffer[m_nCurrentDataReceived], &pData[currentIndex], nBytesToCopy);
			currentIndex += nBytesToCopy;
			m_nCurrentDataReceived += nBytesToCopy;

			return;//wait until a whole packet data received
		}

		int32 nLen = m_nCurrentDataLen - sizeof(int32);
		int32 nIndex = 4;
		int32 nSizeOffset = sizeof(int32);
		int32 nDataOffset = sizeof(int32) + sizeof(int32);
		while(nIndex < nLen)
		{
			m_pTempDataPacket->type = *(int32*)(&m_pCurrentDataBuffer[nIndex]);
			m_pTempDataPacket->size = *(int32*)(&m_pCurrentDataBuffer[nIndex + nSizeOffset]);

			//some temp check, before I find out which part corrupt the memory
			if(m_pTempDataPacket->type < 0 || m_pTempDataPacket->type > 10 || m_pTempDataPacket->size < 0 || m_pTempDataPacket->size > SOCKET_RECV_BUFFER_SIZE)
			{
				//something corrupted discard this packet
				//Debug::Assert(false);
				break;
			}

			memcpy(&m_pTempDataPacket->data[0], &m_pCurrentDataBuffer[nIndex + nDataOffset],m_pTempDataPacket->size);
			
			m_pServerControl->ReceiveData(m_pTempDataPacket);
			nIndex += m_pTempDataPacket->size + nDataOffset;
		}
		m_nCurrentDataLen = -1;
		m_nCurrentDataReceived = -1;

		if(currentIndex >= nByteLen)
		{
			break;
		}
	}
}


//I donot want UDP to send important data, so just use UDP to broadcast the server address
bool 
Server::UDP_Iamhere_thread()
{
	char pszMessage[NAME_CBUFFER_SIZE];
	int32 nMessageLen = sprintf_s(pszMessage,"IAH %s %s", GlobalSetting::pszHostName, GlobalSetting::pszPort);
	sockaddr_in RecvAddr;

	int32 SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(SendingSocket <= 0)
		return false;

	char flag = 'a';
	if(setsockopt(SendingSocket, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) < 0)
    {
        closesocket(SendingSocket);
        return false;
    }

	RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(UDP_PORT);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);//htonl(INADDR_ANY);

	int32 nBytesSent = 0;
	while(m_eStatus == ServerStatus::RUNNING)
	{
		nBytesSent = sendto(SendingSocket, pszMessage, nMessageLen, 0, (SOCKADDR *) & RecvAddr, sizeof (RecvAddr));
		if(nBytesSent <= 0)
		{
			break;
		}

		Sleep(1000);
	}

	closesocket(SendingSocket);
	return true;
}

void
Server::NetworkUpdate()
{

}