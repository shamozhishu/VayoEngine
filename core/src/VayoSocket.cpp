#include "VayoSocket.h"
#include "rc5/RC5.hpp"
#include "zlib/zlib.h"
#pragma comment(lib, "wsock32.lib")

#define NET_MAX_CACHE_SIZE	(1024 * 512)
#define NET_MAX_BUFF_LEN	(1024 * 64)
#define BUFFER_INC_FACTOR   (2)

NS_VAYO_BEGIN

//-------------------------------------------------------------------------
static int inflateMemoryWithHint(unsigned char *in, unsigned int inLength,
	unsigned char **out, unsigned int *outLength, unsigned int outLenghtHint)
{
	/* ret value */
	int err = Z_OK;

	int bufferSize = outLenghtHint;
	*out = new unsigned char[bufferSize];

	z_stream d_stream; /* decompression stream */
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in = in;
	d_stream.avail_in = inLength;
	d_stream.next_out = *out;
	d_stream.avail_out = bufferSize;

	/* window size to hold 256k */
	if ((err = inflateInit2(&d_stream, 15 + 32)) != Z_OK)
		return err;

	for (;;)
	{
		err = inflate(&d_stream, Z_NO_FLUSH);

		if (err == Z_STREAM_END)
		{
			break;
		}

		switch (err)
		{
		case Z_NEED_DICT:
			err = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			inflateEnd(&d_stream);
			return err;
		}

		// not enough memory ?
		if (err != Z_STREAM_END)
		{
			*out = (unsigned char*)realloc(*out, bufferSize * BUFFER_INC_FACTOR);

			/* not enough memory, ouch */
			if (!*out)
			{
				printf("Zip: realloc failed");
				inflateEnd(&d_stream);
				return Z_MEM_ERROR;
			}

			d_stream.next_out = *out + bufferSize;
			d_stream.avail_out = bufferSize;
			bufferSize *= BUFFER_INC_FACTOR;
		}
	}

	*outLength = bufferSize - d_stream.avail_out;
	err = inflateEnd(&d_stream);
	return err;
}

static int inflateMemoryWithHint(unsigned char *in, unsigned int inLength,
	unsigned char **out, unsigned int outLengthHint)
{
	unsigned int outLength = 0;
	int err = inflateMemoryWithHint(in, inLength, out, &outLength, outLengthHint);

	if (err != Z_OK || *out == NULL) {
		if (err == Z_MEM_ERROR)
			printf("Zip: Out of memory while decompressing map data!");
		else
			if (err == Z_VERSION_ERROR)
				printf("Zip: Incompatible zlib version!");
			else
				if (err == Z_DATA_ERROR)
					printf("Zip: Incorrect zlib compressed data!");
				else
					printf("Zip: Unknown error while decompressing map data!");

		delete[] * out;
		*out = NULL;
		outLength = 0;
	}

	return outLength;
}

static int inflateMemory(unsigned char *in, unsigned int inLength, unsigned char **out)
{
	return inflateMemoryWithHint(in, inLength, out, 256 * 1024);
}

//-------------------------------------------------------------------------
// 线程执行函数
static unsigned long workThread(void *arg)
{
	Connector *conn = (Connector*)arg;
	if (!conn)
		return 0;

	// 检查是否能
	while (conn->canLoop())
	{
		conn->checkSocketSignal();
		Sleep(1);
	}

	return 0;
}

// 检查socket的错误
static int checkSocketError(int socketid)
{
	int error = INVALID_VALUE;
	// select未发生变化
	int len = sizeof(int);
	getsockopt(socketid, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
	return error;
}

// 检查socket wouldblock
static int checkSocketWouldblock(void)
{
	return (WSAGetLastError()) == WSAEWOULDBLOCK;
}

//-------------------------------------------------------------------------
// CircularBuffer class.
//-------------------------------------------------------------------------
CircularBuffer::CircularBuffer(int capacity)
{
	_capacity = capacity;
	_cacheData = (char*)malloc(capacity);
	assert(_cacheData);
	reset();
}

CircularBuffer::~CircularBuffer(void)
{
	assert(_cacheData);
	free((void*)_cacheData);
}

// 重置数据缓冲区
void CircularBuffer::reset(void)
{
	_wpos = _rpos = 0;
	_isFull = false;
	_isEmpty = true;
}

// 写入指定长度的数据
bool CircularBuffer::pushData(char *data, int len)
{
	int cur_w_pos = _wpos;
	int cur_r_pos = _rpos;

	// 如果上次为满的标记，并且读写指针相等，则为真的满了
	if (_isFull && cur_w_pos == cur_r_pos)
		return false;

	int cur_w_len = cur_r_pos > cur_w_pos ? (cur_r_pos - cur_w_pos) : (_capacity - cur_w_pos + cur_r_pos);
	// 队列空写入空间不足，此次操作失败
	if (len > cur_w_len)
		return false;

	// 写到结尾可写入的大小
	int w_len = cur_r_pos > cur_w_pos ? (cur_r_pos - cur_w_pos) : (_capacity - cur_w_pos);

	// 这里写入数据
	if (w_len >= len)
		memcpy(_cacheData + cur_w_pos, data, len);
	else
	{
		memcpy(_cacheData + cur_w_pos, data, w_len);
		memcpy(_cacheData, data + w_len, len - w_len);
	}

	_wpos = (cur_w_pos + len) % _capacity;
	_isFull = (_wpos == cur_r_pos);

	return true;
}

// 从环形buffer中读取数据
bool CircularBuffer::popData(char *data, int len)
{
	int cur_r_pos = _rpos;
	int cur_w_pos = _wpos;

	// 如果队列未满，且读写指针相等
	if (_isEmpty && cur_w_pos == cur_r_pos)
		return false;

	int cur_r_len = cur_w_pos > cur_r_pos ? (cur_w_pos - cur_r_pos) : (_capacity - cur_r_pos + cur_w_pos);
	// 队列为空或者需要读取的长度不够,则失败
	if (cur_r_len < len)
		return false;

	int r_len = 0;
	if (cur_w_pos > cur_r_pos)
		r_len = cur_w_pos - cur_r_pos;
	else
		r_len = _capacity - cur_r_pos;

	// 直接可以读
	if (r_len >= len)
		memcpy(data, _cacheData + cur_r_pos, len);
	else
	{
		memcpy(data, _cacheData + cur_r_pos, r_len);
		memcpy(data + r_len, _cacheData, len - r_len);
	}
	_rpos = (cur_r_pos + len) % _capacity;
	_isEmpty = (_rpos == cur_w_pos);
	return true;
}

// 获得当前缓冲区数据大小
int	CircularBuffer::getSize(void)
{
	if (_isFull)
		return _capacity;

	if (_isEmpty)
		return 0;

	int cur_r_pos = _rpos;
	int cur_w_pos = _wpos;

	return cur_w_pos > cur_r_pos ? (cur_w_pos - cur_r_pos) : (_capacity - cur_r_pos + cur_w_pos);
}

//-------------------------------------------------------------------------
// Connector class.
//-------------------------------------------------------------------------
// 构造函数，申请接收和发送缓冲对象
Connector::Connector(void)
{
	_ipAddr = "";
	_encyptKey = "";
	_mcb = NULL;
	_shutdown = false;
	_thread = NULL;
	_readBuff = new CircularBuffer(NET_MAX_CACHE_SIZE);   // 读缓存
	_writeBuff = new CircularBuffer(NET_MAX_CACHE_SIZE);  // 写缓存
	reset();
}

// 析构函数释放对应的内存
Connector::~Connector(void)
{
	if (_thread)
	{
		::CloseHandle(_thread);
		_thread = NULL;
	}
	// 关闭网络连接
	closeSocket();

	// 删除对应的buff
	delete _readBuff;
	delete _writeBuff;
}

// 重置状态
void Connector::reset(void)
{
	_socketid = INVALID_VALUE;
	_status = SOCKET_STATUS_INIT;
	_surplusRecvTime = TIMEOUT_RECV;

	memset(_encodeRbuffer, 0, RECV_DATA_SIZE);
	_encodeSize = 0;
	_nextMessageHead = 0;
	_noblock = true;

	_readBuff->reset();
	_writeBuff->reset();
}

/* 初始化客户端相关的参数
 * sip          :服务器ip地址
 * port         :服务器开放的端口号
 * rttime       :接收数据超时时间
 * encypt_key   :加解秘钥
 */
bool Connector::setServerAddr(const char *sip, int port)
{
	struct in_addr ipAddr;
	struct hostent *h = NULL;

	// 赋值给具体的成员变量
	_ipAddr = sip;
	_port = port;

	// 这里需要设置sockaddr
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons((u_short)port);
	ipAddr.s_addr = inet_addr(sip);
	if (INADDR_NONE != ipAddr.s_addr)
		h = gethostbyaddr((const char*)&ipAddr, sizeof(ipAddr), AF_INET);
	else
		h = gethostbyname(sip);

	if (!h)
		return false;

	in_addr *s_in_addr = ((struct in_addr*)h->h_addr);
	if (!s_in_addr)
		return false;

	_sockaddr.sin_addr = *s_in_addr;
	memset(_sockaddr.sin_zero, 0, sizeof(_sockaddr.sin_zero));
	return true;
}

// 连接服务器
int Connector::connectServer(void)
{
	if (!canConnect())
		return 1;

	_socketid = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketid == INVALID_VALUE)
		return 2; // 创建socket失败

	// 设置地址重用
	char yes = 1;
	int optlen = sizeof(yes);
	setsockopt(_socketid, SOL_SOCKET, SO_REUSEADDR, &yes, optlen);

	// 设置非阻塞模式
	u_long ulon = 1;
	ioctlsocket((SOCKET)_socketid, FIONBIO, (unsigned long*)&ulon);

	// 异步处理，这里无需进行判定
	if (::connect(_socketid, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) != INVALID_VALUE)
		_status = SOCKET_STATUS_CONNECTED;
	else
		_status = SOCKET_STATUS_CONNECT;

	if (_noblock)
	{
		if (_thread)
		{
			::CloseHandle(_thread);
			_thread = NULL;
		}
		_thread = ::CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)workThread, this, 0, NULL);
	}

	// 设定timeout
	_surplusRecvTime = TIMEOUT_RECV;

	return 0;
}

// 关闭连接
void Connector::closeSocket(void)
{
	if (!isConnected())
		return;

	closesocket(_socketid);
	reset();
}

// 发送消息包
bool Connector::sendPacket(Cmd::t_NullCmd *cmd, int len)
{
	// 未设置秘钥
	if (_encyptKey.empty())
		return _writeBuff->pushData((char*)cmd, len);

	// 加密
	char data[NET_MAX_BUFF_LEN] = { 0 };
	int size = ((len + sizeof(int) + 7) & (~7));
	*(int*)data = (size - 4) | 0;
	*(int*)data |= 0x80000000;

	memcpy(data + 4, cmd, len);
	len = RC5_EncDec((unsigned char *)_encyptKey.c_str(), 16, 8,
		(unsigned char*)data, len, true);
	return _writeBuff->pushData(data, len);
}

// 主要负责消息包处理
void Connector::update(double dt)
{
	if (!canLoop())
		return;

	// 检查网络接收是否超时
	_surplusRecvTime -= dt;
	if (!_shutdown && _surplusRecvTime <= 0.0f)
	{
		_shutdown = true;
		return;
	}

	// 异步方式
	if (!_noblock)
		checkSocketSignal();

	// 处理消息
	handleMessage();
}

// 检查读写操作
void Connector::checkSocketSignal(void)
{
	if (_shutdown)
	{
		handleClose(1);
		return;
	}

	// 超时时间
	timeval tm;
	tm.tv_sec = 0;
	if (isSendConnect())
		tm.tv_sec = 5;
	tm.tv_usec = 50 * 1000;

	// 读set
	fd_set	rset;
	FD_ZERO(&rset);
	FD_SET((unsigned int)_socketid, &rset);

	// 写set
	fd_set	wset;
	FD_ZERO(&wset);
	FD_SET((unsigned int)_socketid, &wset);

	int iret = select(_socketid, &rset, &wset, NULL, &tm);
	if (iret == -1)           // socket错误
	{
		handleClose(2);
		return;
	}

	if (iret == 0)            // socket没有变化
	{
		if (isSendConnect())
			handleClose(3);

		if (checkSocketError(_socketid) == 0)
			return;

		handleClose(4);
		return;
	}

	// 检查是否可读
	if (FD_ISSET(_socketid, &rset) > 0)
		handleRead();

	// 检查是否可写
	if (FD_ISSET(_socketid, &wset) > 0)
		handleWrite();
}

// 客户端连接断开，errorCode为断开的错误码
void Connector::handleClose(int errorCode)
{
	// 关闭socket
	closeSocket();

	// 将socket置为关闭状态
	_status = SOCKET_STATUS_CLOSED;
}

// socket可读
void Connector::handleRead(void)
{
	// 如果是发出了连接状态这里只检测连接
	if (isSendConnect())
	{
		_status = SOCKET_STATUS_CONNECTED;
		return;
	}

	// 处理连接通讯状态
	int rsize = ::recv(_socketid, _encodeRbuffer + _encodeSize, RECV_DATA_SIZE - _encodeSize, 0);
	if (rsize <= 0)
	{
		if (!checkSocketWouldblock())
			handleClose(5);
		return;
	}

	// 处理接收数据
	int total_size = _encodeSize + rsize;
	if (_encyptKey.empty())
	{
		_readBuff->pushData(_encodeRbuffer, rsize);
		return;
	}

	// 这里处理解密数据
	int w_enable_sise = (total_size / 8) * 8;
	if (w_enable_sise == 0)
	{
		_encodeSize += rsize;
		return;
	}

	// 解密
	RC5_EncDec((unsigned char *)_encyptKey.c_str(), 16, 8, (unsigned char*)_encodeRbuffer, w_enable_sise, false);
	_encodeSize = total_size - w_enable_sise;

	// 这里必须先push再memmove
	_readBuff->pushData(_encodeRbuffer, w_enable_sise);
	if (_encodeSize == 0)
		return;

	memmove(_encodeRbuffer, _encodeRbuffer + w_enable_sise, _encodeSize);
}

// socket可写时，这里将写缓存的数据发出去
void Connector::handleWrite(void)
{
	int len = _writeBuff->getSize();
	if (!len)
		return;    // 这里没有数据

	char rsb[NET_MAX_CACHE_SIZE] = { 0 };
	if (!_writeBuff->popData(rsb, len))
		return;

	// 这里必须保证发送完成，这里确实有些冒险了,有待改进
	int ssize_total = 0;
	while (true)
	{
		int ssize = ::send(_socketid, rsb + ssize_total, len - ssize_total, 0);
		if (ssize <= 0)
		{
			// 未发送完并且socket出错了，这里需要处理
			if (checkSocketWouldblock())
				continue;
			handleClose(6);
			break;
		}

		// 正常发送
		ssize_total += ssize;
		if (ssize_total == len)
			break;
	}
}

// 处理消息包
void Connector::handleMessage(void)
{
	if (!_mcb)
		return;

	const int hlen = sizeof(int);
	int data_size = _readBuff->getSize();

	// 数据长度不够
	if (data_size <= hlen)
		return;

	int msize = 0;
	// 需要先读取长度
	if (_nextMessageHead == 0)
	{
		if (!_readBuff->popData((char*)&_nextMessageHead, hlen))
			return;
		msize &= 0x0000ffff; // 得到数据长度
	}

	// 没有完整的包
	if (msize > data_size - hlen)
		return;

	// 弹出数据
	char data[NET_MAX_BUFF_LEN] = { 0 };
	if (!_readBuff->popData(data, msize))
		return;

	_surplusRecvTime = TIMEOUT_RECV;

	unsigned char *msg = NULL;
	if (_nextMessageHead & 0x40000000) // 是压缩数据
	{
		// 需要解压缩
		int unzipedlen = inflateMemory((unsigned char*)data, msize, &msg);
		if (!msg || unzipedlen <= 0)
			return;

		// 消息处理函数
		_mcb((Cmd::t_NullCmd*)msg, msize);
		SAFE_DELETE_ARRAY(msg);
	}
	else
	{
		// 处理未压缩的消息包
		_mcb((Cmd::t_NullCmd*)data, msize);
	}
}

NS_VAYO_END
