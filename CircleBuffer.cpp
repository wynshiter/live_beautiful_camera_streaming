#include "stdafx.h"
#include "CircleBuffer.h"
#include "assert.h"

#define CIC_WAITTIMEOUT  3000

CPs_CircleBuffer::CPs_CircleBuffer(const unsigned int iBufferSize)
{
	m_iBufferSize = iBufferSize;
	m_pBuffer = (unsigned char*)malloc(iBufferSize);
	m_iReadCursor = 0;
	m_iWriteCursor = 0;
	m_bComplete = false;
	m_evtDataAvailable = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CPs_CircleBuffer::~CPs_CircleBuffer()
{
	Uninitialise();
}

// Public functions
void CPs_CircleBuffer::Uninitialise()//没有必要public这个接口函数,long120817
{
	CloseHandle(m_evtDataAvailable);
	free(m_pBuffer);
}

//Write前一定要调用m_pCircleBuffer->GetFreeSize()，如果FreeSize不够需要等待,long120817

void  CPs_CircleBuffer::Write(const void* _pSourceBuffer, const unsigned int _iNumBytes)
{
	unsigned int iBytesToWrite = _iNumBytes;
	unsigned char* pSourceReadCursor = (unsigned char*)_pSourceBuffer;

	//CP_ASSERT(iBytesToWrite <= GetFreeSize());//修改为没有足够空间就返回，write前一定要加GetFreeSize判断，否则进入到这里相当于丢掉数据，         // long120817
	if (iBytesToWrite > GetFreeSize())
	{
		return;
	}
	//_ASSERT(m_bComplete == false);

	m_csCircleBuffer.Lock();

	if (m_iWriteCursor >= m_iReadCursor)
	{
		//              0                                            m_iBufferSize
		//              |-----------------|===========|--------------|
		//                                pR->        pW-> 
		// 计算尾部可写空间iChunkSize,long120817
		unsigned int iChunkSize = m_iBufferSize - m_iWriteCursor;

		if (iChunkSize > iBytesToWrite)
		{
			iChunkSize = iBytesToWrite;
		}

		// Copy the data
		memcpy(m_pBuffer + m_iWriteCursor,pSourceReadCursor, iChunkSize);

		pSourceReadCursor += iChunkSize;

		iBytesToWrite -= iChunkSize;

		// 更新m_iWriteCursor
		m_iWriteCursor += iChunkSize;

		if (m_iWriteCursor >= m_iBufferSize)//如果m_iWriteCursor已经到达末尾
			m_iWriteCursor -= m_iBufferSize;//返回到起点0位置,long120817

	}

	//剩余数据从Buffer起始位置开始写
	if (iBytesToWrite)
	{
		memcpy(m_pBuffer + m_iWriteCursor,pSourceReadCursor, iBytesToWrite);
		m_iWriteCursor += iBytesToWrite;
		//_ASSERT(m_iWriteCursor < m_iBufferSize);//这个断言没什么意思，应该_ASSERT(m_iWriteCursor <= m_iReadCursor);long20120817
	}

	SetEvent(m_evtDataAvailable);//设置数据写好信号量

	m_csCircleBuffer.UnLock();
}

bool  CPs_CircleBuffer::Read(void* pDestBuffer, const size_t _iBytesToRead, size_t* pbBytesRead)
{
	size_t iBytesToRead = _iBytesToRead;
	size_t iBytesRead = 0;
	DWORD dwWaitResult;
	bool bComplete = false;

	while (iBytesToRead > 0 && bComplete == false)
	{
		dwWaitResult = WaitForSingleObject(m_evtDataAvailable, CIC_WAITTIMEOUT);//等待数据写好,long120817

		if (dwWaitResult == WAIT_TIMEOUT)
		{
			//TRACE_INFO2("Circle buffer - did not fill in time!");
			*pbBytesRead = iBytesRead;
			return FALSE;//等待超时则返回
		}

		m_csCircleBuffer.Lock();

		if (m_iReadCursor > m_iWriteCursor)
		{
			//              0                                                    m_iBufferSize
			//              |=================|-----|===========================|
			//                                pW->  pR-> 
			unsigned int iChunkSize = m_iBufferSize - m_iReadCursor;

			if (iChunkSize > iBytesToRead)
				iChunkSize = (unsigned int)iBytesToRead;

			//读取操作
			memcpy((unsigned char*)pDestBuffer + iBytesRead,m_pBuffer + m_iReadCursor,iChunkSize);

			iBytesRead += iChunkSize;
			iBytesToRead -= iChunkSize;

			m_iReadCursor += iChunkSize;

			if (m_iReadCursor >= m_iBufferSize)//如果m_iReadCursor已经到达末尾
				m_iReadCursor -= m_iBufferSize;//返回到起点0位置,long120817
		}

		if (iBytesToRead && m_iReadCursor < m_iWriteCursor)
		{
			unsigned int iChunkSize = m_iWriteCursor - m_iReadCursor;

			if (iChunkSize > iBytesToRead)
				iChunkSize = (unsigned int)iBytesToRead;

			//读取操作
			memcpy((unsigned char*)pDestBuffer + iBytesRead,m_pBuffer + m_iReadCursor,iChunkSize);

			iBytesRead += iChunkSize;
			iBytesToRead -= iChunkSize;
			m_iReadCursor += iChunkSize;
		}

		//如果有更多的数据要写
		if (m_iReadCursor == m_iWriteCursor)
		{
			if (m_bComplete)//跳出下一个while循环，该值通过SetComplete()设置，此逻辑什么意思？long120817
				bComplete = true;
		}
		else//还有数据可以读，SetEvent，在下一个while循环开始可以不用再等待,long120817
			SetEvent(m_evtDataAvailable);

		m_csCircleBuffer.UnLock();
	}

	*pbBytesRead = iBytesRead;

	return bComplete ? false : true;

}
//  0                                                m_iBufferSize
//  |------------------------------------------------|
//  pR
//  pW
//读写指针归零
void  CPs_CircleBuffer::Flush()
{
	m_csCircleBuffer.Lock();
	m_iReadCursor = 0;
	m_iWriteCursor = 0;
	m_csCircleBuffer.UnLock();

}
//获取已经写的内存
unsigned int CPs_CircleBuffer::GetUsedSize()
{
	return m_iBufferSize - GetFreeSize();

}


unsigned int CPs_CircleBuffer::GetFreeSize()
{
	unsigned int iNumBytesFree;

	m_csCircleBuffer.Lock();

	if (m_iWriteCursor < m_iReadCursor)
	{
		//              0                                                    m_iBufferSize
		//              |=================|-----|===========================|
		//                                pW->  pR-> 
		iNumBytesFree = (m_iReadCursor - 1) - m_iWriteCursor;
	}
	else if (m_iWriteCursor == m_iReadCursor)
	{
		iNumBytesFree = m_iBufferSize;
	}
	else
	{
		//              0                                                    m_iBufferSize
		//              |-----------------|=====|---------------------------|
		//                                pR->   pW-> 
		iNumBytesFree = (m_iReadCursor - 1) + (m_iBufferSize - m_iWriteCursor);
	}

	m_csCircleBuffer.UnLock();

	return iNumBytesFree;

}
//该函数什么时候调用？long120817
void  CPs_CircleBuffer::SetComplete()
{
	m_csCircleBuffer.Lock();
	m_bComplete = true;
	SetEvent(m_evtDataAvailable);
	m_csCircleBuffer.UnLock();
}