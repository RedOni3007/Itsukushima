#include "Client.h"
#include <thread>

#include "SocketHeaders.h"
#include "BaseNetworkStruct.h"
#include <Game/GameObject.h>
#include <Game/Scene.h>
#include <Network/ClientControl.h>
#include <App/Timer.h>
#include "RemoteIO.h"
#include <Core/StringHelper.h>
#include "EncryptionHelper.h"


void ClientMain(Client* pClient, const char* pszHost,const char* pszPort)
{
	pClient->Run(pszHost,pszPort);
}

Client::Client(void)
{
	m_eConnectStatus = ClientStatus::NOT_CONNECTED;
	m_eSendThreadStatus = ClientStatus::STOPPED;
	m_nConnectedSocketfd = -1;
	m_pMasterFDSet = nullptr;

	m_pScene = nullptr;
	m_pClientControl = nullptr;

	m_nCurrentDataLen = -1;
	m_nCurrentDataReceived = -1;

	m_pCurrentDataBuffer = nullptr;

	m_bConnectionInfoFromUDP = false;
	m_bKillUDP = false;

	m_pCurrentDataBuffer = new int8[SOCKET_RECV_BUFFER_SIZE * 2];
	m_pTempDataPacket = new NS_DataPacket();
}


Client::~Client(void)
{
	End();

	m_pClientControl = nullptr;
	m_pScene = nullptr;

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

void 
Client::SetScene(Scene* pScene)
{
	m_pScene = pScene;
}
	
Scene* 
Client::GetScene()
{
	return m_pScene;
}

ClientStatus 
Client::GetStatus()
{
	return m_eConnectStatus;
}

void 
Client::StartOnThread(const char* pszHost,const char* pszPort)
{
	std::thread udpMain(&Client::UDP_Listen_thread,this);
	udpMain.detach();//no need to track this thread, just kick start

	std::thread clientMain(ClientMain,this,pszHost,pszPort);
	clientMain.detach();//no need to track this thread, just kick start
}


bool Client::Run(const char* pszHost,const char* pszPort)
{
	if(m_eConnectStatus != ClientStatus::NOT_CONNECTED)
		return false;

	while(m_pScene == nullptr)
	{
		//wait the screen to init the scene
		Sleep(16);
	}

	int32 breakCount = 0;
	bool bNothingFromUDP = false;
	while(m_bConnectionInfoFromUDP == false)
	{
		Sleep(50);
		if(++breakCount > 100)
		{
			bNothingFromUDP = true;
			m_bKillUDP = true;
			break;
		}
	}

	GameObject* pTempObject = m_pScene->GetGameObject("Client");//server must be this name
	Debug::Assert(pTempObject != nullptr);
	m_pClientControl = pTempObject->GetClientControlComponent();
	Debug::Assert(m_pClientControl != nullptr);
	m_pClientControl->SetClient(this);

	int32 status;
	struct addrinfo hints, *res, *p;

	// first, load up address structs with getaddrinfo():

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if(bNothingFromUDP || m_bConnectionInfoFromUDP == false)
	{
		if ((status = getaddrinfo(pszHost, pszPort, &hints, &res)) != 0)
		{
			fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
			Debug::Assert(false);
			return false;
		}
	}
	else
	{
		if ((status = getaddrinfo(m_pszHostFromUDP, m_pszPortFromUDP, &hints, &res)) != 0)
		{
			fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
			Debug::Assert(false);
			return false;
		}
	}

	m_eConnectStatus = ClientStatus::CONNECTING;
	// loop through all the results and connect to the first we can
    for(p = res; p != NULL; p = p->ai_next) 
	{
        if ((m_nConnectedSocketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
		{
            continue;
        }

        if (connect(m_nConnectedSocketfd, p->ai_addr, p->ai_addrlen) != 0) 
		{
			closesocket(m_nConnectedSocketfd);
			m_nConnectedSocketfd = -1;
            continue;
        }

        break;
    }

	if(m_nConnectedSocketfd < 0)
	{
		m_eConnectStatus =  ClientStatus::NOT_CONNECTED;
		Debug::Log("FAILED to connect server.\n");
		//Debug::Assert(false);
		return false;
	}

	{
		//turn off Nagle algorithm, hope the small size data will be sent faster
		int flag = 1;
		setsockopt(m_nConnectedSocketfd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(int));
	}

	char s[INET6_ADDRSTRLEN];
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
	Debug::Log("Client: connecting to %s\n", s);
	Debug::Log("Server connected.\n");

	freeaddrinfo(res);

	if(m_pMasterFDSet == nullptr)
		m_pMasterFDSet = new fd_set();

	FD_ZERO(m_pMasterFDSet);
	FD_SET(m_nConnectedSocketfd,m_pMasterFDSet);

	fd_set sReadFDSet;
	FD_ZERO(&sReadFDSet);

	struct timeval tv;//every half second
	tv.tv_sec = 0;
	tv.tv_usec = 500000;

	bool bBreakByError = false;
	int32 nBytes = -1;
	char recvBuffer[SOCKET_RECV_BUFFER_SIZE];
	memset(recvBuffer,0,SOCKET_RECV_BUFFER_SIZE);

	if(	m_eConnectStatus != ClientStatus::CONNECTING)
	{
		//something changed while connecting, so just stop here
		closesocket(m_nConnectedSocketfd);
		m_nConnectedSocketfd = -1;
		m_eConnectStatus = ClientStatus::STOPPED;
		return false;
	}

	m_eConnectStatus = ClientStatus::CONNECTED;

	std::thread sendMain(&Client::SendingManageThread, this);
	sendMain.detach();//no need to track this thread, just kick start


	while(m_eConnectStatus == ClientStatus::CONNECTED)
	{
		sReadFDSet = *m_pMasterFDSet;
		if (select(m_nConnectedSocketfd+1, &sReadFDSet, NULL, NULL, &tv)  < 0)//0 is timeout
		{
           bBreakByError = true;
           break;
        }

		if(FD_ISSET(m_nConnectedSocketfd, &sReadFDSet))
		{
			nBytes = recv(m_nConnectedSocketfd, recvBuffer, SOCKET_RECV_BUFFER_SIZE, 0);
			if(nBytes <= 0)
			{
				//disconnect, or something wrong
				Debug::Log("Server connection stopped.\n");

				End();
				break;
			}
			else
			{
				//got some data
				//Debug::Log("Received %d bytes From Server.\n", nBytes);

				ReceivedData(recvBuffer, nBytes);
				
				//unpooled thread is too expensive
				//std::thread processSocketDataReceivedThread(&Client::ReceivedData, this, recvBuffer, nBytes);
				//processSocketDataReceivedThread.detach();
			}
		}
    }


	closesocket(m_nConnectedSocketfd);
	m_nConnectedSocketfd = -1;
	m_eConnectStatus = ClientStatus::STOPPED;
	Debug::Log("\nReceiving Thread Stopped\n");

	return true;
}

void 
Client::End()
{
	if(	m_eConnectStatus != ClientStatus::CONNECTED)
		return;

	m_eConnectStatus = ClientStatus::STOPPING;
}


void 
Client::ReceivedData(char* pData, int32 nByteLen)
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
				Debug::Assert(false);
				break;
			}

			memcpy(&m_pTempDataPacket->data[0], &m_pCurrentDataBuffer[nIndex + nDataOffset],m_pTempDataPacket->size);

			m_pClientControl->ReceiveData(m_pTempDataPacket);
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

void 
Client::StartSend()
{
	if(m_eSendThreadStatus != ClientStatus::STOPPED)
	{
		//todo: find out why the previous sending is not finished
		//Debug::Assert(false,"previous sending does not finished yet, so skip this sending");
		return;
	}

	m_sendQueue.clear();

	RemoteIO* pRemoteIO = m_pClientControl->GetRemoteIO();
	if(pRemoteIO == nullptr)
		return;

	RefCountPtr<NS_Data> pData = pRemoteIO->GetCurrnetDataToSend();
	if(pData != nullptr)
	{
		m_sendQueue.push_back(pRemoteIO);
	}

	m_eSendThreadStatus = ClientStatus::READY;
}

bool 
Client::SendingManageThread()
{
	fd_set sSendFDSet;
	FD_ZERO(&sSendFDSet);

	//timeout is 90% of single frame time
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = (long)(Timer::Instance()->GetDeltaTime() * 900);//should always be fixed framerate

	while(m_eConnectStatus == ClientStatus::CONNECTED)
	{
		if(m_sendQueue.size() == 0)
		{
			StartSend();

			if(m_sendQueue.size() == 0)
				Sleep(10);//don't do a forever check, sleep a little little while
		}

		while(m_sendQueue.size() > 0 && m_eSendThreadStatus == ClientStatus::READY)
		{
			m_eSendThreadStatus =  ClientStatus::RUNNING;

			std::list<RemoteIO*>::iterator it;
			for(it = m_sendQueue.begin(); it != m_sendQueue.end();)
			{
				SendData((*it));
				it = m_sendQueue.erase(it);
			}

			if(m_eConnectStatus != ClientStatus::CONNECTED)
				break;			
		}

		m_eSendThreadStatus = ClientStatus::STOPPED;
	}

	m_eSendThreadStatus = ClientStatus::STOPPED;

	Debug::Log("\nSending Thread Stopped\n");

	return true;
}

bool 
Client::SendData(RemoteIO* pRemoteIO)
{
	RefCountPtr<NS_Data> pData = pRemoteIO->GetCurrnetDataToSend();

	int32 nBytesLeft = pData->size;
	int32 nBytesSent = 0;
	int32 nLastSentBytes = 0;
	while(nBytesLeft > 0)
	{
		nLastSentBytes = send(m_nConnectedSocketfd, &pData->pData[nBytesSent], nBytesLeft,0);
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

bool 
Client::IsStopped()
{
	//Debug::LogValue("Client Status1 ", (int32)m_eConnectStatus);
	//Debug::LogValue("Client Status2 ", (int32)m_eSendThreadStatus);
	return 	(m_eConnectStatus == ClientStatus::STOPPED || m_eConnectStatus == ClientStatus::NOT_CONNECTED) && (m_eSendThreadStatus == ClientStatus::STOPPED);
}


bool 
Client::UDP_Listen_thread()
{
	m_bConnectionInfoFromUDP = false;
	int32 RecvSocket = 0;
    sockaddr_in RecvAddr;
	int32  iResult = 0;
	struct timeval tv ;
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	fd_set master_fds;
	FD_ZERO(&master_fds);

	fd_set fds;
	FD_ZERO(&fds);

	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (RecvSocket <= 0) 
	{
        return false;
    }

	char flag = 'a';
	if(setsockopt(RecvSocket, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) < 0)
    {
        closesocket(RecvSocket);
        return false;
    }

	RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(UDP_PORT);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = bind(RecvSocket, (SOCKADDR *) & RecvAddr, sizeof (RecvAddr));
    if (iResult != 0) 
	{
		return 1;
    }

	char pszMessage[NAME_CBUFFER_SIZE];
	int32 nMessageLen = NAME_CBUFFER_SIZE;


	sockaddr_storage SenderAddr;
    int SenderAddrSize = sizeof (SenderAddr);

	FD_SET(RecvSocket, &master_fds);
	int32 nByteReceived = 0;
	while(m_eConnectStatus != ClientStatus::STOPPED)
	{
		fds = master_fds;
		iResult = select ( RecvSocket + 1, &fds, NULL, NULL, &tv );
		if(iResult < 0)//0 is timeout
		{
			break;
		}

		if(FD_ISSET(RecvSocket, &fds))
		{
			nByteReceived = recvfrom(RecvSocket, pszMessage, nMessageLen, 0, (SOCKADDR *) & SenderAddr, &SenderAddrSize);
			if(nByteReceived <= 0)
			{
				break;
			}
			else
			{
				pszMessage[nByteReceived] = '\0';
				Debug::Log("UDP received %s\n",pszMessage);

				if(StringHelper::StartWith(pszMessage,"IAH"))
				{
					std::vector<char*> splitResult;
					splitResult.push_back(m_pszHostFromUDP);
					splitResult.push_back(m_pszHostFromUDP);
					splitResult.push_back(m_pszPortFromUDP);

					int32 tokenCount = StringHelper::Split(pszMessage,' ',splitResult, NAME_CBUFFER_SIZE);
					if(tokenCount == 3)
					{
						Debug::Log("host %s\n",m_pszHostFromUDP);
						Debug::Log("port %s\n",m_pszPortFromUDP);
					}

					//not useful
					/*int32 rc = getnameinfo((struct sockaddr *)&SenderAddr, SenderAddrSize, m_pszHostFromUDP, sizeof(m_pszHostFromUDP), m_pszPortFromUDP, sizeof(m_pszPortFromUDP), NI_NUMERICHOST | NI_NUMERICSERV);
					if(rc == 0)
					{
						Debug::Log("host %s\n",m_pszHostFromUDP);
						Debug::Log("port %s\n",m_pszPortFromUDP);
					}*/

					m_bConnectionInfoFromUDP = true;
					break;
				}
			}
		}

		if(m_eConnectStatus == ClientStatus::CONNECTED || m_bConnectionInfoFromUDP || m_bKillUDP)//stop once connected
			break;
	}

	closesocket(RecvSocket);
	return true;
}

void 
Client::NetworkUpdate()
{

}