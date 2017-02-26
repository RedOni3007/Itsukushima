#include <Network/ServerControl.h>
#include "Server.h"
#include "RemoteIO.h"
#include <Game/Scene.h>
#include "EncryptionHelper.h"

ServerControl::ServerControl()
{
	m_pServer = nullptr;
}


ServerControl::~ServerControl()
{
	m_pServer = nullptr;
}

void 
ServerControl::PreLogicUpdate()
{

}

void 
ServerControl::LogicUpdate()
{

}

void 
ServerControl::PostLogicUpdate()
{

}

void 
ServerControl::GraphicUpdate()
{

}

void 
ServerControl::NetworkUpdate()
{
	StartSend();
}


const char* 
ServerControl::GetName()
{
	return "ServerControl";
}

const char* 
ServerControl::ClassName()
{
	return "ServerControl";
}

void 
ServerControl::AddNewClient(int32 nSocketFD)
{

}

void 
ServerControl::RemoveClient(int32 nSocketFD)
{

}

void 
ServerControl::AddGlobalData(NS_DataPacket* pDataPacket)
{
	if(pDataPacket != nullptr)
		m_globalSendQueue.push_back(pDataPacket);
}

void 
ServerControl::PackData()
{
	if(m_pServer == nullptr)
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

	RemoteIO* pRemoteIO = nullptr;
	std::list<void*> allRemoteIO;
	//GetComponnets could be slow if object's number is huge, because of string compare, need a cache in the future
	m_pServer->GetScene()->GetComponents(RemoteIO::ClassName(), allRemoteIO);
	RefCountPtr<NS_Data> pDataToSend = new NS_Data(nTotalByteSize,pPackedData);
	EncryptionHelper::EncryptXOR(pDataToSend->pData,pDataToSend->size);
	for(std::list<void*>::iterator it = allRemoteIO.begin(); it != allRemoteIO.end(); ++it)
	{
		pRemoteIO = (RemoteIO*)(*it);
		pRemoteIO->AddDataToSendQueue(pDataToSend);
	}
}

void 
ServerControl::StartSend()
{
	if(m_pServer == nullptr)
		return;

	m_pServer->StartSend();
}

void 
ServerControl::SetServer(Server* pServer)
{
	m_pServer = pServer;
}

void 
ServerControl::ReceiveData(NS_DataPacket* pDataPacket)
{

}