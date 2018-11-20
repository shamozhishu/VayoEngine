/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* 网络客户端
\*************************************************************************/
#ifndef __VAYO_SOCKET_H__
#define __VAYO_SOCKET_H__

#include "VayoSupport.h"
#include <winsock.h>
NS_VAYO_BEGIN

/* 描述:环形buffer,线程安全（有且仅有在一个生产者和一个消费者的情况下能做到无锁安全）
 * 生产者对wptr进行改写，消费者对rptr进行改写，所以能保证线程安全性
 * 本例中对应写数据时自动添加了，数据的长度，读数据时如果读出数据时内部自动解除了其长度
 */
class _VayoExport CircularBuffer
{
public:
	CircularBuffer(int capacity);
	~CircularBuffer(void);
	// 环形buffer相关的接口
	void    reset(void);                            // 重置数据缓冲区
	bool    pushData(char *data, int len);          // 写入数据
	bool    popData(char *data, int len);           // 读取指定长度的数据
	int     getSize(void);                          // 获得当前数据大小

private:
	int     _wpos;          // 当前写位置
	int     _rpos;          // 当前读位置
	int     _capacity;      // 缓冲区的容量
	char    *_cacheData;    // 数据首指针
	bool    _isFull;        // 队列满
	bool    _isEmpty;       // 队列空
};

#define INVALID_VALUE   -1                      // 无效值
#define TIMEOUT_RECV	30.0f                   // 接收超时时间
#define RECV_DATA_SIZE  2048                    // 单次接收数据长度

// 外部类
namespace Cmd
{
	class t_NullCmd;
};

// 函数指针(消息处理回调)
typedef void(*FuncMessageCallBack)(Cmd::t_NullCmd*, int);

// 连接器对象类
class _VayoExport Connector
{
public:
	// 构造函数相关的
	Connector(void);
	~Connector(void);

	// 设定上层消息处理函数(必须调用, 此函数线程安全)
	void setMessageCallback(FuncMessageCallBack mcb) { _mcb = mcb; }

	// 设定服务器相关的信息(必须调用)
	bool setServerAddr(const char *sip, int port);

	// 该函数是设定是否采用全异步的方式(根据自己的需求，设定该参数后逻辑和网络会在多线程环境下运行)
	void setNoBlock(bool noblock) { _noblock = noblock; }

	// 设置加密秘钥(选择性调用)
	void setEncyptkey(const char *key) { _encyptKey = key; }

	// 客户端主动断开连接(只通过该接口，结束网络)
	void close(void) { _shutdown = true; };

	// 连接服务器
	int	 connectServer(void);

	// 发送消息包
	bool sendPacket(Cmd::t_NullCmd *cmd, int len);

	// 这个用于客户端检查网络连接状态(根据自己逻辑进行调用)
	bool isClosed(void) { return _status == SOCKET_STATUS_CLOSED; }

	// 轮询函数(需要在主循环里调用，必须的)
	void update(double dt);                     // 主要负责消息包处理

	// 下面两个函数上层调用者无需调用
	void checkSocketSignal(void);               // 检查读写操作
	bool canLoop(void) { return (isSendConnect() || isConnected()); }

private:
	void closeSocket(void);                     // 关闭连接
	void reset(void);                           // 重置状态

	// 此处定义socket相关的函数
	void handleRead(void);                      // 消息包处理函数
	void handleWrite(void);                     // 处理接收数据
	void handleClose(int errorCode);            // 客户端连接断开，errorCode为断开的错误码
	void handleMessage(void);                   // 这个是子类必须要重载的方法

	// 状态判定函数
	bool canConnect(void) { return _status == SOCKET_STATUS_INIT || isClosed(); }
	bool isSendConnect(void) { return _status == SOCKET_STATUS_CONNECT; }
	bool isConnected(void) { return _status == SOCKET_STATUS_CONNECTED; }

private:
	enum
	{
		SOCKET_STATUS_INIT = 0,             // 初始化OPEN
		SOCKET_STATUS_CONNECT,              // 发起了连接
		SOCKET_STATUS_CONNECTED,            // 已经连接成功
		SOCKET_STATUS_CLOSED,               // SOCKET被遗弃
	};

	FuncMessageCallBack _mcb;               // 消息处理回调函数接口
	bool                _shutdown;          // 退出标志
	bool                _noblock;           // 异步标志
	int                 _status;            // 是否已连接
	std::string         _ipAddr;            // 远程地址
	int                 _port;              // 远程端口
	HANDLE              _thread;

	std::string         _encyptKey;         // 加解秘钥
	double              _surplusRecvTime;   // 记录接收超时时间

	int                 _socketid;          // socket描述符
	struct sockaddr_in  _sockaddr;          // 服务器地址

	CircularBuffer      *_readBuff;         // 读缓存
	CircularBuffer      *_writeBuff;        // 写缓存

	// 用于接收解密的缓存
	char                _encodeRbuffer[RECV_DATA_SIZE];
	int                 _encodeSize;
	int                 _nextMessageHead;
};

NS_VAYO_END

#endif // __VAYO_SOCKET_H__
