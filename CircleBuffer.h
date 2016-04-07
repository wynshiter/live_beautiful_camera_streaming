#ifndef CIRCLE_BUFFER
#define  CIRCLE_BUFFER

#define CIC_READCHUNKSIZE 1024*16

#ifdef WIN32
#include <windows.h>

#define  V_MUTEX			CRITICAL_SECTION //利用临界区实现的锁变量
#define  V_MUTEX_INIT(m)		InitializeCriticalSection(m)
#define  V_MUTEX_LOCK(m)		EnterCriticalSection(m)
#define  V_MUTEX_UNLOCK(m)		LeaveCriticalSection(m)
#define  V_MUTEX_DESTORY(m)		DeleteCriticalSection(m)

#else

#define  V_MUTEX				pthread_mutex_t
#define  V_MUTEX_INIT(m)		pthread_mutex_init(m,NULL)
#define  V_MUTEX_LOCK(m)		pthread_mutex_Lock(m)
#define  V_MUTEX_UNLOCK(m)		pthread_mutex_unLock(m)
#define  V_MUTEX_DESTORY(m)		pthread_mutex_destroy(m)

#endif


class  Vlock
{
public:
	Vlock(void)
	{
		V_MUTEX_INIT(&m_Lock);
	}
	~Vlock(void)
	{
		V_MUTEX_DESTORY(&m_Lock);
	}
public:
	void Lock(){V_MUTEX_LOCK(&m_Lock);}
	void UnLock(){V_MUTEX_UNLOCK(&m_Lock);}
private:
	V_MUTEX m_Lock;
};


class  CPs_CircleBuffer
{
public:
	CPs_CircleBuffer(const unsigned int iBufferSize);
	~CPs_CircleBuffer();
public:
	// Public functions
	void  Uninitialise();
	void  Write(const void* pSourceBuffer, const unsigned int iNumBytes);
	bool  Read(void* pDestBuffer, const size_t iBytesToRead, size_t* pbBytesRead);
	void  Flush();
	unsigned int GetUsedSize();
	unsigned int GetFreeSize();
	void  SetComplete();
	bool  IsComplete();

private:       
	unsigned char*	m_pBuffer;
	unsigned int	m_iBufferSize;
	unsigned int	m_iReadCursor;
	unsigned int	m_iWriteCursor;
	HANDLE			m_evtDataAvailable;
	Vlock			m_csCircleBuffer;
	bool			m_bComplete;      
};



//
//1、创建对象
//	CPs_CircleBuffer* m_pCircleBuffer;
//m_pCircleBuffer = new CPs_CircleBuffer（bufsize）；
//	2、写
//	if (m_pCircleBuffer->GetFreeSize() < CIC_READCHUNKSIZE)
//	{
//		Sleep(20);
//		continue;
//	}
//	m_pCircleBuffer->Write(internetbuffer.lpvBuffer,internetbuffer.dwBufferLength);
//	3、读
//		m_pCircleBuffer->Read(pDestBuffer,iBytesToRead, piBytesRead);
//
//	4、其他调用
//		if(m_pCircleBuffer->IsComplete())
//			break;        
//	iUsedSpace =m_pCircleBuffer->GetUsedSize();
//	m_pCircleBuffer->SetComplete();
//
//
//
#endif