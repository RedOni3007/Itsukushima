#include <Network/ClientControl.h>
#include "BaseNetworkStruct.h"
#include "RemoteIO.h"
#include <Game/Scene.h>
#include "Client.h"
#include "EncryptionHelper.h"

ClientControl::ClientControl()
{
	m_pClient = nullptr;
	m_pRemoteIO = nullptr;
}


ClientControl::~ClientControl()
{
	m_pClient = nullptr;
	m_pRemoteIO = nullptr;
}

void 
ClientControl::PreLogicUpdate()
{

}

void 
ClientControl::LogicUpdate()
{

}

void 
ClientControl::PostLogicUpdate()
{

}

void 
ClientControl::GraphicUpdate()
{

}

void 
ClientControl::NetworkUpdate()
{
	StartSend();
}

const char* 
ClientControl::GetName()
{
	return "ClientControl";
}

const char* 
ClientControl::ClassName()
{
	return "ClientControl";
}


void 
ClientControl::PackData()
{
	if(m_pClient == nullptr)
		return;

	if(m_globalSendQueue.size() == 0)
		return;

	int32 nTotalByteSize = 4;//4 bytes at beginning for overall size
	NS_DataPacket* pData = nullptr;

	for(std::list<NS_DataPacket*>::iterator it = m_globalSendQueue.begin(); it != m_globalSendQueue.end(); ++it)
	{
		pData = *it;
		nTotalByteSize += (pData->size + 8);//4bytes for size, 4bytes for type
	}

	int8* pPackedData = new int8[nTotalByteSize];

	//4 bytes at beginning for overall size
	*((int32*)(&pPackedData[0])) = nTotalByteSize;
	uint32 index = 4;

	for(std::list<NS_DataPacket*>::iterator it = m_globalSendQueue.begin(); it != m_globalSendQueue.end(); ++it)
	{
		pData = *it;
		*((int32*)(&pPackedData[index])) = pData->type;
		*((uint32*)(&pPackedData[index+4])) = pData->size;
		memcpy(&pPackedData[index+8], &pData->data[0],  pData->size);
		index +=  (pData->size + 8);

		//data is packed, no longer need it
		delete pData;
	}
	m_globalSendQueue.clear();
	RefCountPtr<NS_Data> pDataToSend = new NS_Data(nTotalByteSize,pPackedData);
	EncryptionHelper::EncryptXOR(pDataToSend->pData,pDataToSend->size);
	m_pRemoteIO->AddDataToSendQueue(pDataToSend);
}

void 
ClientControl::StartSend()
{
	if(m_pClient == nullptr)
		return;

	m_pClient->StartSend();
}

void 
ClientControl::SetClient(Client* pClient)
{
	m_pClient = pClient;
}


void 
ClientControl::ReceiveData(NS_DataPacket* pDataPacket)
{

}

void 
ClientControl::AddNewClient(int32 nSocketFD)
{

}

void 
ClientControl::RemoveClient(int32 nSocketFD)
{

}

void 
ClientControl::AddGlobalData(NS_DataPacket* pDataPacket)
{
	if(pDataPacket != nullptr)
		m_globalSendQueue.push_back(pDataPacket);
}

RemoteIO* 
ClientControl::GetRemoteIO()
{
	return m_pRemoteIO;
}