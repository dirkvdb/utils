//    Copyright (C) 2012 Dirk Vanden Boer <dirk.vdb@gmail.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef SIMPLE_SUBSCRIBER_H
#define SIMPLE_SUBSCRIBER_H

#include "utils/subscriber.h"

namespace utils
{

template <typename T>
class SimpleSubscriber : public ISubscriber<const T&>
{
public:
	T& getItem()
	{
		return m_item;
	}

	void onItem(const T& item, void* pData = nullptr)
	{
		m_item = item;
	}

private:
	T m_item;
};

}

#endif
