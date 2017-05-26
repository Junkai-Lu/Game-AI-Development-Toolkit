/*
* a simple unit test for GADT.
*
*/

/* Copyright (c) 2017 Junkai Lu <junkai-lu@outlook.com>.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "gadttest.h"

using std::cout;
using std::endl;

namespace gadt
{
	namespace unittest
	{
		void TestBitBoard()
		{
			//test BitBoard<>
			BitBoard<56> temp;
			BitBoard<56> subtemp;
			subtemp.set(1);
			GADT_ASSERT(0, temp.total());
			GADT_ASSERT(false, temp.any());
			GADT_ASSERT(true, temp.none());
			temp.set(1);
			GADT_ASSERT(true, subtemp.is_subset_of(temp));
			GADT_ASSERT(true, temp.exist_subset(subtemp));
			GADT_ASSERT(true, temp.any());
			GADT_ASSERT(false, temp.none());
			temp.set(51);
			temp.set(26);
			subtemp.set(51);
			GADT_ASSERT(true, subtemp.is_subset_of(temp));
			GADT_ASSERT(true, temp.exist_subset(subtemp));
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
			BitPoker subtemp_poker;
			GADT_ASSERT(0, temp_poker.total());
			GADT_ASSERT(false, temp_poker.any());
			GADT_ASSERT(true, temp_poker.none());
			temp_poker.set(1, 1);
			GADT_ASSERT(true, temp_poker.any());
			GADT_ASSERT(false, temp_poker.none());
			temp_poker.set(2, 2);
			temp_poker.set(3, 3);
			subtemp_poker.set(1, 1);
			subtemp_poker.set(2, 1);
			subtemp_poker.set(3, 1);
			GADT_ASSERT(true,temp_poker.exist_subset(subtemp_poker));
			GADT_ASSERT(true,subtemp_poker.is_subset_of(temp_poker));
			GADT_ASSERT(6, temp_poker.total());
			temp_poker.reset(1);
			GADT_ASSERT(5, temp_poker.total());
			temp_poker.push(3);
			GADT_ASSERT(4, temp_poker.get(3));
			temp_poker.decrease(3);
			GADT_ASSERT(3, temp_poker.get(3));
			temp_poker.increase(4);
			GADT_ASSERT(1, temp_poker.get(4));

			//test BitMahjong
			BitMahjong temp_mahjong;
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
			temp_mahjong.decrease(3);
			GADT_ASSERT(3, temp_mahjong.get(3));
			temp_mahjong.increase(4);
			GADT_ASSERT(1, temp_mahjong.get(4));
			temp_mahjong.push(25);
			GADT_ASSERT(1, temp_mahjong.get(25));
			temp_mahjong.decrease(25);
			GADT_ASSERT(0, temp_mahjong.get(25));
			temp_mahjong.increase(25);
			GADT_ASSERT(1, temp_mahjong.get(25));
			GADT_ASSERT(7, temp_mahjong.total());

			//test ValueVector
			ValueVector<14> temp_vec;
			temp_vec.push(2);
			GADT_ASSERT(1, temp_vec.length());
			GADT_ASSERT(2, temp_vec.draw_value());
			GADT_ASSERT(2, temp_vec.draw_and_remove_value());
			GADT_ASSERT(0, temp_vec.length());
			temp_vec.push(3);
			GADT_ASSERT(1, temp_vec.length());
		}
		void TestFileLib()
		{
			srand((unsigned int)time(NULL));
			std::string path = "./test_dir"  + console::IntergerToString(rand() % 99999999);
			if (!file::DirExist(path))
			{
				GADT_ASSERT(true, file::MakeDir(path));
				GADT_ASSERT(true, file::DirExist(path));
				GADT_ASSERT(true, file::RemoveDir(path));
				GADT_ASSERT(false, file::DirExist(path));
			}
		}
		void TestIndex()
		{
			player::PlayerIndex<1, 5> index(2);
			index.to_next();
			GADT_ASSERT(3, index.current());
			GADT_ASSERT(4, index.get_next());
			GADT_ASSERT(2, index.get_prev());
			GADT_ASSERT(5, index.get_jump(2));
			GADT_ASSERT(1, index.get_jump(3));
			GADT_ASSERT(2, index.get_jump(14));
			GADT_ASSERT(2, index.get_jump(24));

			player::PlayerGroup<1, 5, int> players(2);
			players.data(2) = 1;
			GADT_ASSERT(1, players.data());

			players.to_next();
			GADT_ASSERT(3, players.current());
			GADT_ASSERT(4, players.get_next());
			GADT_ASSERT(2, players.get_prev());
			GADT_ASSERT(5, players.get_jump(2));
			GADT_ASSERT(1, players.get_jump(3));
			GADT_ASSERT(2, players.get_jump(14));
			GADT_ASSERT(2, players.get_jump(24));

		}

		const std::vector<FuncPair> func_list = {
			{ "bitboard", TestBitBoard },
			{ "file", TestFileLib },
			{ "index", TestIndex}
		};
		void RunTest(FuncPair func_pair)
		{
			cout << endl << ">> test start, target = ";
			ccout << console::GREEN << "<" << func_pair.first << ">" << console::DEFAULT;
			auto t = timer::GetClock();
			cout << endl;
			func_pair.second();
			cout << ">> test comlete, time = ";
			console::Cprintf(timer::GetTimeDifference(t), console::RED);
			cout << endl;
		}
	}
}