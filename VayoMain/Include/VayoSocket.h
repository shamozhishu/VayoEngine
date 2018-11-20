/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2018 authored by ��Ӻ�
* ����ͻ���
\*************************************************************************/
#ifndef __VAYO_SOCKET_H__
#define __VAYO_SOCKET_H__

#include "VayoSupport.h"
#include <winsock.h>
NS_VAYO_BEGIN

/* ����:����buffer,�̰߳�ȫ�����ҽ�����һ�������ߺ�һ�������ߵ������������������ȫ��
 * �����߶�wptr���и�д�������߶�rptr���и�д�������ܱ�֤�̰߳�ȫ��
 * �����ж�Ӧд����ʱ�Զ�����ˣ����ݵĳ��ȣ�������ʱ�����������ʱ�ڲ��Զ�������䳤��
 */
class _VayoExport CircularBuffer
{
public:
	CircularBuffer(int capacity);
	~CircularBuffer(void);
	// ����buffer��صĽӿ�
	void    reset(void);                            // �������ݻ�����
	bool    pushData(char *data, int len);          // д������
	bool    popData(char *data, int len);           // ��ȡָ�����ȵ�����
	int     getSize(void);                          // ��õ�ǰ���ݴ�С

private:
	int     _wpos;          // ��ǰдλ��
	int     _rpos;          // ��ǰ��λ��
	int     _capacity;      // ������������
	char    *_cacheData;    // ������ָ��
	bool    _isFull;        // ������
	bool    _isEmpty;       // ���п�
};

#define INVALID_VALUE   -1                      // ��Чֵ
#define TIMEOUT_RECV	30.0f                   // ���ճ�ʱʱ��
#define RECV_DATA_SIZE  2048                    // ���ν������ݳ���

// �ⲿ��
namespace Cmd
{
	class t_NullCmd;
};

// ����ָ��(��Ϣ����ص�)
typedef void(*FuncMessageCallBack)(Cmd::t_NullCmd*, int);

// ������������
class _VayoExport Connector
{
public:
	// ���캯����ص�
	Connector(void);
	~Connector(void);

	// �趨�ϲ���Ϣ������(�������, �˺����̰߳�ȫ)
	void setMessageCallback(FuncMessageCallBack mcb) { _mcb = mcb; }

	// �趨��������ص���Ϣ(�������)
	bool setServerAddr(const char *sip, int port);

	// �ú������趨�Ƿ����ȫ�첽�ķ�ʽ(�����Լ��������趨�ò������߼���������ڶ��̻߳���������)
	void setNoBlock(bool noblock) { _noblock = noblock; }

	// ���ü�����Կ(ѡ���Ե���)
	void setEncyptkey(const char *key) { _encyptKey = key; }

	// �ͻ��������Ͽ�����(ֻͨ���ýӿڣ���������)
	void close(void) { _shutdown = true; };

	// ���ӷ�����
	int	 connectServer(void);

	// ������Ϣ��
	bool sendPacket(Cmd::t_NullCmd *cmd, int len);

	// ������ڿͻ��˼����������״̬(�����Լ��߼����е���)
	bool isClosed(void) { return _status == SOCKET_STATUS_CLOSED; }

	// ��ѯ����(��Ҫ����ѭ������ã������)
	void update(double dt);                     // ��Ҫ������Ϣ������

	// �������������ϲ�������������
	void checkSocketSignal(void);               // ����д����
	bool canLoop(void) { return (isSendConnect() || isConnected()); }

private:
	void closeSocket(void);                     // �ر�����
	void reset(void);                           // ����״̬

	// �˴�����socket��صĺ���
	void handleRead(void);                      // ��Ϣ��������
	void handleWrite(void);                     // �����������
	void handleClose(int errorCode);            // �ͻ������ӶϿ���errorCodeΪ�Ͽ��Ĵ�����
	void handleMessage(void);                   // ������������Ҫ���صķ���

	// ״̬�ж�����
	bool canConnect(void) { return _status == SOCKET_STATUS_INIT || isClosed(); }
	bool isSendConnect(void) { return _status == SOCKET_STATUS_CONNECT; }
	bool isConnected(void) { return _status == SOCKET_STATUS_CONNECTED; }

private:
	enum
	{
		SOCKET_STATUS_INIT = 0,             // ��ʼ��OPEN
		SOCKET_STATUS_CONNECT,              // ����������
		SOCKET_STATUS_CONNECTED,            // �Ѿ����ӳɹ�
		SOCKET_STATUS_CLOSED,               // SOCKET������
	};

	FuncMessageCallBack _mcb;               // ��Ϣ����ص������ӿ�
	bool                _shutdown;          // �˳���־
	bool                _noblock;           // �첽��־
	int                 _status;            // �Ƿ�������
	std::string         _ipAddr;            // Զ�̵�ַ
	int                 _port;              // Զ�̶˿�
	HANDLE              _thread;

	std::string         _encyptKey;         // �ӽ���Կ
	double              _surplusRecvTime;   // ��¼���ճ�ʱʱ��

	int                 _socketid;          // socket������
	struct sockaddr_in  _sockaddr;          // ��������ַ

	CircularBuffer      *_readBuff;         // ������
	CircularBuffer      *_writeBuff;        // д����

	// ���ڽ��ս��ܵĻ���
	char                _encodeRbuffer[RECV_DATA_SIZE];
	int                 _encodeSize;
	int                 _nextMessageHead;
};

NS_VAYO_END

#endif // __VAYO_SOCKET_H__
