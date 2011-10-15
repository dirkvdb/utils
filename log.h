//    Copyright (C) 2009 Dirk Vanden Boer <dirk.vdb@gmail.com>
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

#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#include <string>
#include <iostream>
#include <sstream>

class Log
{
public:
    template<typename T1>
	static void info(const T1& t1)
	{
		std::stringstream ss;
		ss << t1;

		outputInfo(ss.str());
	}

	template<typename T1, typename T2>
	static void info(const T1& t1, const T2& t2)
	{
		std::stringstream ss;
		ss << t1 << " " << t2;

		outputInfo(ss.str());
	}

	template<typename T1, typename T2, typename T3>
	static void info(const T1& t1, const T2& t2, const T3& t3)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3;

		outputInfo(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4>
	static void info(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4;

		outputInfo(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	static void info(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5;

		outputInfo(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	static void info(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << " " << t6;

		outputInfo(ss.str());
	}



	template<typename T1>
	static void warn(const T1& t1)
	{
		std::stringstream ss;
		ss << t1;

		outputWarn(ss.str());
	}

	template<typename T1, typename T2>
	static void warn(const T1& t1, const T2& t2)
	{
		std::stringstream ss;
		ss << t1 << " " << t2;

		outputWarn(ss.str());
	}

	template<typename T1, typename T2, typename T3>
	static void warn(const T1& t1, const T2& t2, const T3& t3)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3;

		outputWarn(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4>
	static void warn(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4;

		outputWarn(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	static void warn(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5;

		outputWarn(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	static void warn(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << " " << t6;

		outputWarn(ss.str());
	}

	template<typename T1>
	static void critical(const T1& t1)
	{
		std::stringstream ss;
		ss << t1;

		outputCritical(ss.str());
	}

	template<typename T1, typename T2>
	static void critical(const T1& t1, const T2& t2)
	{
		std::stringstream ss;
		ss << t1 << " " << t2;

		outputCritical(ss.str());
	}

	template<typename T1, typename T2, typename T3>
	static void critical(const T1& t1, const T2& t2, const T3& t3)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3;

		outputCritical(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4>
	static void critical(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4;

		outputCritical(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	static void critical(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5;

		outputCritical(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	static void critical(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << " " << t6;

		outputCritical(ss.str());
	}

	template<typename T1>
	static void error(const T1& t1)
	{
		std::stringstream ss;
		ss << t1;

		outputError(ss.str());
	}

	template<typename T1, typename T2>
	static void error(const T1& t1, const T2& t2)
	{
		std::stringstream ss;
		ss << t1 << " " << t2;

		outputError(ss.str());
	}

	template<typename T1, typename T2, typename T3>
	static void error(const T1& t1, const T2& t2, const T3& t3)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3;

		outputError(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4>
	static void error(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4;

		outputError(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	static void error(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5;

		outputError(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	static void error(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << " " << t6;

		outputError(ss.str());
	}

	template<typename T1>
	static void debug(const T1& t1)
	{
		std::stringstream ss;
		ss << t1;

		outputDebug(ss.str());
	}

	template<typename T1, typename T2>
	static void debug(const T1& t1, const T2& t2)
	{
		std::stringstream ss;
		ss << t1 << " " << t2;

		outputDebug(ss.str());
	}

	template<typename T1, typename T2, typename T3>
	static void debug(const T1& t1, const T2& t2, const T3& t3)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3;

		outputDebug(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4>
	static void debug(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4;

		outputDebug(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	static void debug(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5;

		outputDebug(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	static void debug(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << " " << t6;

		outputDebug(ss.str());
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	static void debug(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7)
	{
		std::stringstream ss;
		ss << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << " " << t6 << " " << t7;

		outputDebug(ss.str());
	}

private:
	static void outputInfo(const std::string& message);
	static void outputWarn(const std::string& message);
	static void outputError(const std::string& message);
	static void outputCritical(const std::string& message);
	static void outputDebug(const std::string& message);
};

#endif
