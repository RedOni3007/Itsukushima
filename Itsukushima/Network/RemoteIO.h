/*
* RemoteIO
*
* @author: Kai Yang
*/

#ifndef REMOTE_IO_H
#define REMOTE_IO_H

#include <Core/CoreHeaders.h>
#include <Game/GameObjectComponent.h>
#include "BaseNetworkStruct.h"

#include <list>

class RemoteIO : public GameObjectComponent
{
public:
	RemoteIO(int32 nSocketFD);
	virtual  ~RemoteIO();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();

	virtual const char* GetName();
	static const char* ClassName();

	void	AddWTPDataToSend(NS_DataPacket* pData);

	//will delete the data, once called this function the passin data should not be used any more
	void	RemoveWTPDataToSend(NS_DataPacket* pData);

	//will create a local copy of the pass-in data
	void	AddReceivedData(NS_DataPacket* pData);

	//will delete the data, once called this function the passin data should not be used any more
	void	RemoveReceivedData(NS_DataPacket* pData);

	NS_DataPacket*	GetNextDataToSend();//return nullptr if nothing to send
	NS_DataPacket*  GetNextReceivedData();//return nullptr if the list is empty

	int32 GetSocketFD();

	void AddDataToSendQueue(RefCountPtr<NS_Data> pData);
	RefCountPtr<NS_Data> GetCurrnetDataToSend();
	void CurrentDataSendDone();

	void AddDataOnlyToThisIO(NS_DataPacket* pData);//the data will only send to this IO
	void AddDataOnlyToThisIO(NS_DataPacket* pDataPackets, int32 nSize);//pData is the pointer of array

	std::list<NS_DataPacket*>* GetSendWTPQueue();

private:
	int32 m_nSocketFD;

	//wait to process queue
	std::list<NS_DataPacket*> m_sendWTPQueue;
	std::list<NS_DataPacket*> m_receiveDataQueue;

	//wait to send
	std::list<RefCountPtr<NS_Data>> m_toSendDataQueue;
	RefCountPtr<NS_Data> m_currentToSendData;
};

#endif