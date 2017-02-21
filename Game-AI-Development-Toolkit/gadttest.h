#include "gadtlib.hpp"
#include "bitboard.hpp"

#pragma once

#define GADT_ASSERT(fir,sec) if(fir != sec)std::cout<<"Assert Failed, func = "<<__FUNCTION__<<" line = "<<__LINE__<<std::endl;
#define GADT_TEST_FINISH(name, start) std::cout<<">> test "<<name<<" finish, time = "<<gadt::timer::GetTimeDifference(start)<<std::endl;

namespace gadt
{
	namespace unittest
	{
		void TestBitBoard();
		void RunAllTest();
	}
}
