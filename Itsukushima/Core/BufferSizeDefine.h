/**
* define char string buffer size, so I won't need to change every where
*
* @author: Kai Yang
**/

#ifndef BUFFER_SIZE_DEFINE_H
#define BUFFER_SIZE_DEFINE_H

#define NAME_CBUFFER_SIZE 256

#define LINE_CBUFFER_SIZE 1024

#define SOCKET_RECV_BUFFER_SIZE 2048

//the MTU limit is 1460 bytes, must less than the MTU size, so the the data will not be sent by multiple times
#define NS_PACKET_DATA_MAX_SIZE 1024

#define NS_DATA_QUEUE_SIZE 32

#endif