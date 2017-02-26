#include "RemoteIO.h"
#include "EncryptionHelper.h"

RemoteIO::RemoteIO(int32 nSocketFD)
{
	m_nSocketFD = nSocketFD;
	m_currentToSendData = nullptr;
}


RemoteIO::~RemoteIO()
{
	m_currentToSendData = nullptr;
	m_toSendDataQueue.clear();

	std::list<NS_DataPacket*>::iterator it;
	for(it = m_sendWTPQueue.begin(); it != m_sendWTPQueue.end(); ++it)
	{
		delete (*it);
	}
	m_sendWTPQueue.clear();

	for(it = m_receiveDataQueue.begin(); it != m_receiveDataQueue.end(); ++it)
	{
		delete (*it);
	}
	m_receiveDataQueue.clear();
}

void 
RemoteIO::PreLogicUpdate()
{

}

void 
RemoteIO::LogicUpdate()
{

}

void 
RemoteIO::PostLogicUpdate()
{

}

void 
RemoteIO::GraphicUpdate()
{

}


const char* 
RemoteIO::GetName()
{
	return "RemoteIO";
}

const char* 
RemoteIO::ClassName()
{
	return "RemoteIO";
}


void	
RemoteIO::AddWTPDataToSend(NS_DataPacket* pData)
{
	if(pData != nullptr)
		m_sendWTPQueue.push_back(pData);
}

void	
RemoteIO::RemoveWTPDataToSend(NS_DataPacket* pData)
{
	if(pData == nullptr)
		return;

	m_sendWTPQueue.remove(pData);
	delete pData;
}

NS_DataPacket*	
RemoteIO::GetNextDataToSend()
{
	if(m_sendWTPQueue.size() == 0)
		return nullptr;

	return *m_sendWTPQueue.begin();

}

std::list<NS_DataPacket*>* 
RemoteIO::GetSendWTPQueue()
{
	return &m_sendWTPQueue;
}

void	
RemoteIO::AddReceivedData(NS_DataPacket* pData)
{
	if(pData != nullptr)
	{
		NS_DataPacket* pLocalCopy = new NS_DataPacket();
		pLocalCopy->type = pData->type;
		pLocalCopy->size = pData->size;
		memcpy(&pLocalCopy->data[0],&pData->data[0], pData->size);
		m_receiveDataQueue.push_back(pLocalCopy);
	}
}

void	
RemoteIO::RemoveReceivedData(NS_DataPacket* pData)
{
	if(pData == nullptr)
		return;

	m_receiveDataQueue.remove(pData);
	delete pData;
}

NS_DataPacket*    
RemoteIO::GetNextReceivedData()
{
	if(m_receiveDataQueue.size() == 0)
		return nullptr;

	return *m_receiveDataQueue.begin();
}

int32 
RemoteIO::GetSocketFD()
{
	return m_nSocketFD;
}

void 
RemoteIO::AddDataToSendQueue(RefCountPtr<NS_Data> pData)
{
	m_toSendDataQueue.push_back(pData);
}

RefCountPtr<NS_Data> 
RemoteIO::GetCurrnetDataToSend()
{
	if(m_currentToSendData != nullptr)
	{
		return m_currentToSendData;
	}
	else if(m_toSendDataQueue.size() > 0)
	{
		m_currentToSendData = *m_toSendDataQueue.begin();
		m_toSendDataQueue.pop_front();
		return m_currentToSendData;
	}
	else
	{
		return nullptr;
	}
}

	
void 
RemoteIO::CurrentDataSendDone()
{
	m_currentToSendData = nullptr;
}

void
RemoteIO::AddDataOnlyToThisIO(NS_DataPacket* pData)
{
	uint32 nTotalByteSize =  4;//4 bytes at beginning for overall size
	nTotalByteSize += pData->size + 8;//4bytes for size, 4bytes for type
	int8* pPackedData = new int8[nTotalByteSize];

	//4 bytes at beginning for overall size
	*((int32*)(&pPackedData[0])) = nTotalByteSize;
	uint32 index = 4;

	*((int32*)(&pPackedData[index])) = pData->type;
	*((uint32*)(&pPackedData[index + 4])) = pData->size;
	memcpy(&pPackedData[index + 8], &pData->data[0],  pData->size);

	RefCountPtr<NS_Data> pDataToSend = new NS_Data(nTotalByteSize,pPackedData);
	AddDataToSendQueue(pDataToSend);
}

void
RemoteIO::AddDataOnlyToThisIO(NS_DataPacket* pDataPackets, int32 nSize)
{
	uint32 nTotalByteSize =  4;//4 bytes at beginning for overall size
	NS_DataPacket* pData = nullptr;

	for(int32 i = 0; i < nSize; ++i)
	{
		nTotalByteSize += (pDataPackets[i].size + 8);
	}

	int8* pPackedData = new int8[nTotalByteSize];
	//4 bytes at beginning for overall size
	*((int32*)(&pPackedData[0])) = nTotalByteSize;
	uint32 index = 4;

	for(int32 i = 0; i < nSize; ++i)
	{
		pData = &pDataPackets[i];

		*((int32*)(&pPackedData[index])) = pData->type;
		*((uint32*)(&pPackedData[index+4])) = pData->size;
		memcpy(&pPackedData[index+8], &pData->data[0],  pData->size);
		index +=  (pData->size + 8);
	}

	RefCountPtr<NS_Data> pDataToSend = new NS_Data(nTotalByteSize,pPackedData);
	EncryptionHelper::EncryptXOR(pDataToSend->pData,pDataToSend->size);
	AddDataToSendQueue(pDataToSend);
}