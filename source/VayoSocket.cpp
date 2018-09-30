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
// �߳�ִ�к���
static unsigned long workThread(void *arg)
{
	Connector *conn = (Connector*)arg;
	if (!conn)
		return 0;

	// ����Ƿ���
	while (conn->canLoop())
	{
		conn->checkSocketSignal();
		Sleep(1);
	}

	return 0;
}

// ���socket�Ĵ���
static int checkSocketError(int socketid)
{
	int error = INVALID_VALUE;
	// selectδ�����仯
	int len = sizeof(int);
	getsockopt(socketid, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
	return error;
}

// ���socket wouldblock
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

// �������ݻ�����
void CircularBuffer::reset(void)
{
	_wpos = _rpos = 0;
	_isFull = false;
	_isEmpty = true;
}

// д��ָ�����ȵ�����
bool CircularBuffer::pushData(char *data, int len)
{
	int cur_w_pos = _wpos;
	int cur_r_pos = _rpos;

	// ����ϴ�Ϊ���ı�ǣ����Ҷ�дָ����ȣ���Ϊ�������
	if (_isFull && cur_w_pos == cur_r_pos)
		return false;

	int cur_w_len = cur_r_pos > cur_w_pos ? (cur_r_pos - cur_w_pos) : (_capacity - cur_w_pos + cur_r_pos);
	// ���п�д��ռ䲻�㣬�˴β���ʧ��
	if (len > cur_w_len)
		return false;

	// д����β��д��Ĵ�С
	int w_len = cur_r_pos > cur_w_pos ? (cur_r_pos - cur_w_pos) : (_capacity - cur_w_pos);

	// ����д������
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

// �ӻ���buffer�ж�ȡ����
bool CircularBuffer::popData(char *data, int len)
{
	int cur_r_pos = _rpos;
	int cur_w_pos = _wpos;

	// �������δ�����Ҷ�дָ�����
	if (_isEmpty && cur_w_pos == cur_r_pos)
		return false;

	int cur_r_len = cur_w_pos > cur_r_pos ? (cur_w_pos - cur_r_pos) : (_capacity - cur_r_pos + cur_w_pos);
	// ����Ϊ�ջ�����Ҫ��ȡ�ĳ��Ȳ���,��ʧ��
	if (cur_r_len < len)
		return false;

	int r_len = 0;
	if (cur_w_pos > cur_r_pos)
		r_len = cur_w_pos - cur_r_pos;
	else
		r_len = _capacity - cur_r_pos;

	// ֱ�ӿ��Զ�
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

// ��õ�ǰ���������ݴ�С
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
// ���캯����������պͷ��ͻ������
Connector::Connector(void)
{
	_ipAddr = "";
	_encyptKey = "";
	_mcb = NULL;
	_shutdown = false;
	_thread = NULL;
	_readBuff = new CircularBuffer(NET_MAX_CACHE_SIZE);   // ������
	_writeBuff = new CircularBuffer(NET_MAX_CACHE_SIZE);  // д����
	reset();
}

// ���������ͷŶ�Ӧ���ڴ�
Connector::~Connector(void)
{
	if (_thread)
	{
		::CloseHandle(_thread);
		_thread = NULL;
	}
	// �ر���������
	closeSocket();

	// ɾ����Ӧ��buff
	delete _readBuff;
	delete _writeBuff;
}

// ����״̬
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

/* ��ʼ���ͻ�����صĲ���
 * sip          :������ip��ַ
 * port         :���������ŵĶ˿ں�
 * rttime       :�������ݳ�ʱʱ��
 * encypt_key   :�ӽ���Կ
 */
bool Connector::setServerAddr(const char *sip, int port)
{
	struct in_addr ipAddr;
	struct hostent *h = NULL;

	// ��ֵ������ĳ�Ա����
	_ipAddr = sip;
	_port = port;

	// ������Ҫ����sockaddr
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

// ���ӷ�����
int Connector::connectServer(void)
{
	if (!canConnect())
		return 1;

	_socketid = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketid == INVALID_VALUE)
		return 2; // ����socketʧ��

	// ���õ�ַ����
	char yes = 1;
	int optlen = sizeof(yes);
	setsockopt(_socketid, SOL_SOCKET, SO_REUSEADDR, &yes, optlen);

	// ���÷�����ģʽ
	u_long ulon = 1;
	ioctlsocket((SOCKET)_socketid, FIONBIO, (unsigned long*)&ulon);

	// �첽����������������ж�
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

	// �趨timeout
	_surplusRecvTime = TIMEOUT_RECV;

	return 0;
}

// �ر�����
void Connector::closeSocket(void)
{
	if (!isConnected())
		return;

	closesocket(_socketid);
	reset();
}

// ������Ϣ��
bool Connector::sendPacket(Cmd::t_NullCmd *cmd, int len)
{
	// δ������Կ
	if (_encyptKey.empty())
		return _writeBuff->pushData((char*)cmd, len);

	// ����
	char data[NET_MAX_BUFF_LEN] = { 0 };
	int size = ((len + sizeof(int) + 7) & (~7));
	*(int*)data = (size - 4) | 0;
	*(int*)data |= 0x80000000;

	memcpy(data + 4, cmd, len);
	len = RC5_EncDec((unsigned char *)_encyptKey.c_str(), 16, 8,
		(unsigned char*)data, len, true);
	return _writeBuff->pushData(data, len);
}

// ��Ҫ������Ϣ������
void Connector::update(double dt)
{
	if (!canLoop())
		return;

	// �����������Ƿ�ʱ
	_surplusRecvTime -= dt;
	if (!_shutdown && _surplusRecvTime <= 0.0f)
	{
		_shutdown = true;
		return;
	}

	// �첽��ʽ
	if (!_noblock)
		checkSocketSignal();

	// ������Ϣ
	handleMessage();
}

// ����д����
void Connector::checkSocketSignal(void)
{
	if (_shutdown)
	{
		handleClose(1);
		return;
	}

	// ��ʱʱ��
	timeval tm;
	tm.tv_sec = 0;
	if (isSendConnect())
		tm.tv_sec = 5;
	tm.tv_usec = 50 * 1000;

	// ��set
	fd_set	rset;
	FD_ZERO(&rset);
	FD_SET((unsigned int)_socketid, &rset);

	// дset
	fd_set	wset;
	FD_ZERO(&wset);
	FD_SET((unsigned int)_socketid, &wset);

	int iret = select(_socketid, &rset, &wset, NULL, &tm);
	if (iret == -1)           // socket����
	{
		handleClose(2);
		return;
	}

	if (iret == 0)            // socketû�б仯
	{
		if (isSendConnect())
			handleClose(3);

		if (checkSocketError(_socketid) == 0)
			return;

		handleClose(4);
		return;
	}

	// ����Ƿ�ɶ�
	if (FD_ISSET(_socketid, &rset) > 0)
		handleRead();

	// ����Ƿ��д
	if (FD_ISSET(_socketid, &wset) > 0)
		handleWrite();
}

// �ͻ������ӶϿ���errorCodeΪ�Ͽ��Ĵ�����
void Connector::handleClose(int errorCode)
{
	// �ر�socket
	closeSocket();

	// ��socket��Ϊ�ر�״̬
	_status = SOCKET_STATUS_CLOSED;
}

// socket�ɶ�
void Connector::handleRead(void)
{
	// ����Ƿ���������״̬����ֻ�������
	if (isSendConnect())
	{
		_status = SOCKET_STATUS_CONNECTED;
		return;
	}

	// ��������ͨѶ״̬
	int rsize = ::recv(_socketid, _encodeRbuffer + _encodeSize, RECV_DATA_SIZE - _encodeSize, 0);
	if (rsize <= 0)
	{
		if (!checkSocketWouldblock())
			handleClose(5);
		return;
	}

	// �����������
	int total_size = _encodeSize + rsize;
	if (_encyptKey.empty())
	{
		_readBuff->pushData(_encodeRbuffer, rsize);
		return;
	}

	// ���ﴦ���������
	int w_enable_sise = (total_size / 8) * 8;
	if (w_enable_sise == 0)
	{
		_encodeSize += rsize;
		return;
	}

	// ����
	RC5_EncDec((unsigned char *)_encyptKey.c_str(), 16, 8, (unsigned char*)_encodeRbuffer, w_enable_sise, false);
	_encodeSize = total_size - w_enable_sise;

	// ���������push��memmove
	_readBuff->pushData(_encodeRbuffer, w_enable_sise);
	if (_encodeSize == 0)
		return;

	memmove(_encodeRbuffer, _encodeRbuffer + w_enable_sise, _encodeSize);
}

// socket��дʱ�����ｫд��������ݷ���ȥ
void Connector::handleWrite(void)
{
	int len = _writeBuff->getSize();
	if (!len)
		return;    // ����û������

	char rsb[NET_MAX_CACHE_SIZE] = { 0 };
	if (!_writeBuff->popData(rsb, len))
		return;

	// ������뱣֤������ɣ�����ȷʵ��Щð����,�д��Ľ�
	int ssize_total = 0;
	while (true)
	{
		int ssize = ::send(_socketid, rsb + ssize_total, len - ssize_total, 0);
		if (ssize <= 0)
		{
			// δ�����겢��socket�����ˣ�������Ҫ����
			if (checkSocketWouldblock())
				continue;
			handleClose(6);
			break;
		}

		// ��������
		ssize_total += ssize;
		if (ssize_total == len)
			break;
	}
}

// ������Ϣ��
void Connector::handleMessage(void)
{
	if (!_mcb)
		return;

	const int hlen = sizeof(int);
	int data_size = _readBuff->getSize();

	// ���ݳ��Ȳ���
	if (data_size <= hlen)
		return;

	int msize = 0;
	// ��Ҫ�ȶ�ȡ����
	if (_nextMessageHead == 0)
	{
		if (!_readBuff->popData((char*)&_nextMessageHead, hlen))
			return;
		msize &= 0x0000ffff; // �õ����ݳ���
	}

	// û�������İ�
	if (msize > data_size - hlen)
		return;

	// ��������
	char data[NET_MAX_BUFF_LEN] = { 0 };
	if (!_readBuff->popData(data, msize))
		return;

	_surplusRecvTime = TIMEOUT_RECV;

	unsigned char *msg = NULL;
	if (_nextMessageHead & 0x40000000) // ��ѹ������
	{
		// ��Ҫ��ѹ��
		int unzipedlen = inflateMemory((unsigned char*)data, msize, &msg);
		if (!msg || unzipedlen <= 0)
			return;

		// ��Ϣ������
		_mcb((Cmd::t_NullCmd*)msg, msize);
		SAFE_DELETE_ARRAY(msg);
	}
	else
	{
		// ����δѹ������Ϣ��
		_mcb((Cmd::t_NullCmd*)data, msize);
	}
}

NS_VAYO_END
