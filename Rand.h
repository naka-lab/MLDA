#pragma once
#include <stdlib.h>

class CRand
{
public:
	CRand()
	{
		m_pRandTableF = new double[_RAND_TABLE_NUM];
		m_pRandTableD = new unsigned int[_RAND_TABLE_NUM];
		Init();
		m_it = 0;
	}

	~CRand()
	{
		delete [] m_pRandTableF;
		delete [] m_pRandTableD;
	}

	void Init()
	{
		// 乱数テーブル作成
		for(int i=0 ; i<_RAND_TABLE_NUM ; i++ )
		{
			m_pRandTableF[i] = (double)GenRand()/RAND_MAX;
			m_pRandTableD[i] = GenRand();
		}
		m_it = 0;
	}

	inline double GetRandF()
	{
		if( ++m_it >_RAND_TABLE_NUM ) m_it = 0;
		return m_pRandTableF[m_it];
	}

	inline unsigned int GetRandD()
	{
		if( ++m_it >_RAND_TABLE_NUM ) m_it = 0;
		return m_pRandTableD[m_it];
	}

protected:
	double *m_pRandTableF;
	unsigned int *m_pRandTableD;
	int m_it;
	static const unsigned int _RAND_TABLE_NUM = 10000;
	
	unsigned int GenRand()
	{
		return rand();
		/*
		unsigned int val;
		rand_s(&val);
		return val;
		*/
	}
};

