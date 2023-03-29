#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H


#include <direct.h>
#include "conio.h"
#include <time.h>
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <vector>

template< class T>
inline T rotl( T v, int shift){
    std::size_t m = sizeof(v)*std::numeric_limits<T>::digits;
    T s = shift>=0? shift%m: -((-shift)%m);
    return (v<<s) | (v>>(m-s));
}

template< class T>
inline T rotr(T v, int shift){
    std::size_t m = sizeof(v)*std::numeric_limits<T>::digits;
    T s = shift>=0? shift%m: -((-shift)%m);
    return (v>>s) | (v<<(m-s));
}

class StatusManager
{
protected:
	StatusManager() : m_state(0x00){};

public:
	enum ST_STATUS { ST_PLAY = 0x00000001,
					 ST_PAUSE = 0x00000002
	};

	static StatusManager& instance()
	{
		static StatusManager obj;
		return obj;
	}

	void setState(std::string _st)
	{
		setState(fromString(_st));

	}
	void setState(ST_STATUS _p)
	{
		//activate ST_STATUS,  rest remains the same
		m_state = m_state | _p;
	}
	void setOn(std::string _p)
	{
		setOn(fromString(_p));

	}
	void setOn(ST_STATUS _p)
	{
		//the rest of the states to 0
		m_state =  _p;
	}
	void unsetState(ST_STATUS _p)
	{
		m_state = m_state & ~_p;
	}
	void clearType() { m_state = 0x00; }

	std::vector<ST_STATUS> getType()
	{
		std::vector<ST_STATUS> v;
		if (m_state & ST_PAUSE)
			v.push_back(ST_PAUSE);
		if (m_state & ST_PLAY)
			v.push_back(ST_PLAY);


		return v;
	}
	bool is(ST_STATUS _t)
	{
		return m_state & _t;
	}
	void change(ST_STATUS _t)
	{
		m_state = m_state^ _t;
	}
	
	void next()
	{

		m_state = rotl<int>(m_state, 1);


	}
	void prev()
	{
		m_state = rotr<int>(m_state, 1);

	}
	
	static std::string toString(ST_STATUS _p)
	{
		
		if (_p == ST_PLAY) return std::string("ST_PLAY");
		if (_p == ST_PAUSE) return std::string("ST_PAUSE");
	}

	static ST_STATUS fromString(std::string _s)
	{
		if (!_s.compare("ST_PLAY")) return ST_PLAY;
		if (!_s.compare("ST_PAUSE")) return ST_PAUSE;
	}
protected:
	int m_state;
};

#endif
