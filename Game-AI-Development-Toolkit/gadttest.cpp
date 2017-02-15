/*
* a simple unit test for GADT.
*
* version: 2017/2/15
* copyright: Junkai Lu
* email: Junkai-Lu@outlook.com
*/

#include "gadttest.h"

namespace gadt
{
	namespace unittest
	{
		void TestBitBoard()
		{
			clock_t start = timer::GetClock();
			//test BitBoard<>
			BitBoard<56> temp;
			GADT_ASSERT(0, temp.total());
			GADT_ASSERT(false, temp.any());
			GADT_ASSERT(true, temp.none());
			temp.set(1);
			GADT_ASSERT(true, temp.any());
			GADT_ASSERT(false, temp.none());
			temp.set(51);
			temp.set(26);
			GADT_ASSERT(3, temp.total());
			temp.reset(1);
			temp.write(26, 0);
			GADT_ASSERT(1, temp.total());
			GADT_ASSERT(true, temp.get(51));
			GADT_ASSERT(56, temp.upper_bound());

			//test BitBoard64
			BitBoard64 temp_64;
			GADT_ASSERT(0, temp_64.total());
			GADT_ASSERT(false, temp_64.any());
			GADT_ASSERT(true, temp_64.none());
			temp_64.set(1);
			GADT_ASSERT(true, temp_64.any());
			GADT_ASSERT(false, temp_64.none());
			temp_64.set(51);
			temp_64.set(26);
			GADT_ASSERT(3, temp_64.total());
			temp_64.reset(1);
			temp_64.write(26, 0);
			GADT_ASSERT(1, temp_64.total());
			GADT_ASSERT(true, temp_64.get(51));
			GADT_ASSERT(64, temp_64.upper_bound());

			//test BitPoker
			BitPoker temp_poker;
			GADT_ASSERT(0, temp_poker.total());
			GADT_ASSERT(false, temp_poker.any());
			GADT_ASSERT(true, temp_poker.none());
			temp_poker.set(1, 1);
			GADT_ASSERT(true, temp_poker.any());
			GADT_ASSERT(false, temp_poker.none());
			temp_poker.set(2, 2);
			temp_poker.set(3, 3);
			GADT_ASSERT(6, temp_poker.total());
			temp_poker.reset(1);
			GADT_ASSERT(5, temp_poker.total());
			temp_poker.push(3);
			GADT_ASSERT(4, temp_poker.get(3));

			//test BitPoker
			BitPoker temp_mahjong;
			GADT_ASSERT(0, temp_mahjong.total());
			GADT_ASSERT(false, temp_mahjong.any());
			GADT_ASSERT(true, temp_mahjong.none());
			temp_mahjong.set(1, 1);
			GADT_ASSERT(true, temp_mahjong.any());
			GADT_ASSERT(false, temp_mahjong.none());
			temp_mahjong.set(2, 2);
			temp_mahjong.set(3, 3);
			GADT_ASSERT(6, temp_mahjong.total());
			temp_mahjong.reset(1);
			GADT_ASSERT(5, temp_mahjong.total());
			temp_mahjong.push(3);
			GADT_ASSERT(4, temp_mahjong.get(3));

			//test ValueVector
			ValueVector<14> temp_vec;
			temp_vec.push(2);
			GADT_ASSERT(1, temp_vec.length());
			GADT_ASSERT(2, temp_vec.draw_value());
			GADT_ASSERT(2, temp_vec.draw_and_remove_value());
			GADT_ASSERT(0, temp_vec.length());
			temp_vec.push(3);
			GADT_ASSERT(1, temp_vec.length());

			GADT_TEST_FINISH("bitboard.h", start);
			
		}

		void RunAllTest()
		{
			TestBitBoard();
		}
	}
}