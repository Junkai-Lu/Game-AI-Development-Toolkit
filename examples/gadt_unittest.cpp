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

#include "gadt_unittest.h"

using std::cout;
using std::endl;

namespace gadt
{
	namespace unittest
	{
		namespace tic_tac_toe
		{
			void UpdateState(State& state, const Action& action)
			{
				state.dot[action.x][action.y] = action.player;
				state.next_player = action.player == WHITE ? BLACK : WHITE;
			}

			void MakeAction(const State& state, ActionSet& as)
			{
				for (size_t x = 0; x < 3; x++)
				{
					for (size_t y = 0; y < 3; y++)
					{
						if (state.dot[x][y] == EMPTY)
						{
							as.push_back({ x,y,state.next_player });
						}
					}
				}
			}

			Player DetemineWinner(const State& state)
			{
				bool all_filled = true;
				for (size_t i = 0; i < 3; i++)
				{
					for (size_t y = 0; y < 3; y++)
					{
						if (state.dot[i][y] == EMPTY)
						{
							all_filled = false;
						}
					}
					if (state.dot[i][0] == state.dot[i][1] && state.dot[i][1] == state.dot[i][2] && state.dot[i][0] != EMPTY)
					{
						return state.dot[i][0];
					}
					if (state.dot[0][i] == state.dot[1][i] && state.dot[1][i] == state.dot[2][i] && state.dot[0][i] != EMPTY)
					{
						return state.dot[0][i];
					}
				}
				if (state.dot[0][0] == state.dot[1][1] && state.dot[1][1] == state.dot[2][2] && state.dot[1][1] != EMPTY)
				{
					return state.dot[0][0];
				}
				if (state.dot[2][0] == state.dot[1][1] && state.dot[1][1] == state.dot[0][2] && state.dot[1][1] != EMPTY)
				{
					return state.dot[2][0];
				}
				if (all_filled)
				{
					return DRAW;
				}
				return EMPTY;
			}

			Result StateToResult(const State& state, AgentIndex winner)
			{
				return (Result)winner;
			}

			bool AllowUpdateValue(const State& state, Result winner)
			{
				if (state.next_player == BLACK && winner == BLACK)
				{
					return true;
				}
				if (state.next_player == WHITE && winner == WHITE)
				{
					return true;
				}
				return false;
			}

			std::string StateToStr(const State& state)
			{
				std::stringstream ss;
				for (size_t x = 0; x < 3; x++)
				{
					for (size_t y = 0; y < 3; y++)
					{
						switch (state.dot[x][y])
						{
						case BLACK:
							ss << "O";
							break;
						case WHITE:
							ss << "X";
							break;
						default:
							ss << "_";
							break;
						}
						//ss << state.dot[x][y];
					}
					ss << " ";
				} 
				ss << "  [" << state.next_player << "] is next";
				return ss.str();
			}

			std::string ActionToStr(const Action& action)
			{
				std::stringstream ss;
				ss << "player "<<action.player << " [" << action.x << "," << action.y << "]";
				return ss.str();
			}

			std::string ResultToStr(const Result& result)
			{
				std::stringstream ss;
				ss << result;
				return ss.str();
			}

			int EvaluateState(const State& state, const AgentIndex winner)
			{
				if (winner == DRAW)
				{
					return 0;
				}
				return state.next_player == winner ? INT32_MAX : -INT32_MAX;
			}
		}

		void TestConvertFunc()
		{
			int t = 0;
			GADT_ASSERT(ToString(t), "0");
			t = INT32_MAX;
			t = ToInt(ToString(t));
			GADT_ASSERT(t, INT32_MAX);

			size_t st = UINT64_MAX;
			st = ToSizeT(ToString(st));
			GADT_ASSERT(st, UINT64_MAX);

			int8_t t8 = INT8_MAX;
			t8 = ToInt8(ToString(t8));
			GADT_ASSERT(t8, INT8_MAX);

			int16_t t16 = INT16_MAX;
			t16 = ToInt16(ToString(t16));
			GADT_ASSERT(t16, INT16_MAX);

			int32_t t32 = INT32_MAX;
			t32 = ToInt32(ToString(t32));
			GADT_ASSERT(t32, INT32_MAX);

			int64_t t64 = INT64_MAX;
			t64 = ToInt64(ToString(t64));
			GADT_ASSERT(t64, INT64_MAX);

			uint8_t ut8 = UINT8_MAX;
			ut8 = ToUInt8(ToString(ut8));
			GADT_ASSERT(ut8, UINT8_MAX);

			uint16_t ut16 = UINT16_MAX;
			ut16 = ToUInt16(ToString(ut16));
			GADT_ASSERT(ut16, UINT16_MAX);

			uint32_t ut32 = UINT32_MAX;
			ut32 = ToUInt32(ToString(ut32));
			GADT_ASSERT(ut32, UINT32_MAX);

			uint64_t ut64 = UINT64_MAX;
			ut64 = ToUInt64(ToString(ut64));
			GADT_ASSERT(ut64, UINT64_MAX);
		}
		void TestPoint()
		{
			//test signed point
			BasicPoint<int64_t> point64(-100, -200);
			BasicPoint<int8_t> point8(-1, -2);

			//comparison
			GADT_ASSERT(point64 == BasicPoint<int64_t>(-100, -200), true);
			GADT_ASSERT(point64 != BasicPoint<int64_t>(-100, 0), true);
			GADT_ASSERT(point64 != BasicPoint<int64_t>(0, -200), true);

			//integer
			GADT_ASSERT(point64 * -10 == BasicPoint<int16_t>(1000, 2000), true);
			GADT_ASSERT(point64 / -10 == BasicPoint<int16_t>(10, 20), true);
			point64 *= -10;
			GADT_ASSERT(point64 == BasicPoint<int16_t>(1000, 2000), true);
			point64 /= -10;
			GADT_ASSERT(point64 == BasicPoint<int16_t>(-100, -200), true);

			//point
			GADT_ASSERT((point64 + point8) == BasicPoint<int16_t>(-101, -202), true);
			GADT_ASSERT((point64 - point8) == BasicPoint<int16_t>(-99, -198), true);
			point64 += point8;
			GADT_ASSERT(point64 == BasicPoint<int16_t>(-101, -202), true);
			point64 -= point8;
			GADT_ASSERT(point64 == BasicPoint<int16_t>(-100, -200), true);
			GADT_ASSERT(point64 * point8, 500);

			//test unsigned point
			BasicUPoint<uint8_t> upoint8(1, 2);
			BasicUPoint<uint64_t> upoint64(100, 200);

			//comparison
			GADT_ASSERT(upoint64 == BasicUPoint<uint64_t>(100, 200), true);
			GADT_ASSERT(upoint64 != BasicUPoint<uint64_t>(100, 0), true);
			GADT_ASSERT(upoint64 != BasicUPoint<uint64_t>(0, 200), true);

			//uinteger
			GADT_ASSERT(upoint64 * 10 == BasicUPoint<uint16_t>(1000, 2000), true);
			GADT_ASSERT(upoint64 / 10 == BasicUPoint<uint16_t>(10, 20), true);
			upoint64 *= 10;
			GADT_ASSERT(upoint64 == BasicUPoint<uint16_t>(1000, 2000), true);
			upoint64 /= 10;
			GADT_ASSERT(upoint64 == BasicUPoint<uint16_t>(100, 200), true);

			//upoint
			GADT_ASSERT((upoint64 + upoint8) == BasicUPoint<uint16_t>(101, 202), true);
			GADT_ASSERT((upoint64 - upoint8) == BasicUPoint<uint16_t>(99, 198), true);
			upoint64 += upoint8;
			GADT_ASSERT(upoint64 == BasicUPoint<uint16_t>(101, 202), true);
			upoint64 -= upoint8;
			GADT_ASSERT(upoint64 == BasicUPoint<uint16_t>(100, 200), true);
			GADT_ASSERT(upoint64 * upoint8, 500);
		}
		void TestBitBoard()
		{
			//test BitBoard<>
			bitboard::BitBoard<56> temp;
			bitboard::BitBoard<56> subtemp;
			subtemp.set(1);
			size_t count = 0;
			for (bool v : subtemp)
			{
				GADT_ASSERT(v, (count == 1 ? true : false));
				count++;
			}
			GADT_ASSERT(count, temp.upper_bound());
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
			temp = bitboard::BitBoard<56>{ 0,1,2,3,4,5 };
			GADT_ASSERT(6, temp.total());

			//test BitBoard64
			bitboard::BitBoard64 temp_64;
			GADT_ASSERT(0, temp_64.total());
			GADT_ASSERT(false, temp_64.any());
			GADT_ASSERT(true, temp_64.none());
			temp_64.set(1);
			GADT_ASSERT((temp_64 ^ temp_64).total(), 0);
			GADT_ASSERT((~temp_64).get(1), false);
			GADT_ASSERT((~temp_64).get(0), true);
			GADT_ASSERT((~temp_64).get(2), true);
			count = 0;
			for (bool v : temp_64)
			{
				GADT_ASSERT(v, (count == 1 ? true : false));
				count++;
			}
			GADT_ASSERT(count, temp_64.upper_bound());
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
			temp_64 = bitboard::BitBoard64(0xFF);
			GADT_ASSERT(8, temp_64.total());
			temp_64 = bitboard::BitBoard64{0,1,2,3,4,5,6,7};
			GADT_ASSERT(8, temp_64.total());
			temp_64 &= bitboard::BitBoard64{ 0,1,2,3,4,5,6,7,8 };
			GADT_ASSERT(8, temp_64.total());
			temp_64 |= bitboard::BitBoard64{ 0,1,2,3,4,5,6,7,8 };
			GADT_ASSERT(9, temp_64.total());

			//test BitPoker
			bitboard::BitPoker temp_poker;
			bitboard::BitPoker subtemp_poker;
			GADT_ASSERT(0, temp_poker.total());
			GADT_ASSERT(false, temp_poker.any());
			GADT_ASSERT(true, temp_poker.none());
			temp_poker.set(1, 1);
			GADT_ASSERT(true, temp_poker.any());
			GADT_ASSERT(false, temp_poker.none());
			temp_poker.set(2, 2);
			temp_poker.set(3, 3);
			count = 0;
			for (size_t v : temp_poker)
			{
				GADT_ASSERT(v, (count > 0 && count < 4 ? count : 0));
				count++;
			}
			GADT_ASSERT(count, temp_poker.upper_bound());
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
			temp_poker = bitboard::BitPoker(0xFF);
			GADT_ASSERT(30, temp_poker.total());
			temp_poker = bitboard::BitPoker{ {0,3}, {1,2}, {2,3} };
			GADT_ASSERT(8, temp_poker.total());
			temp_poker += bitboard::BitPoker{ { 0,1 },{ 1,1 },{ 2,1 } };
			GADT_ASSERT(11, temp_poker.total());
			temp_poker -= bitboard::BitPoker{ { 0,1 },{ 1,1 },{ 2,1 } };
			GADT_ASSERT(8, temp_poker.total());


			//test BitMahjong
			bitboard::BitMahjong temp_mahjong;
			GADT_ASSERT(0, temp_mahjong.total());
			GADT_ASSERT(false, temp_mahjong.any());
			GADT_ASSERT(true, temp_mahjong.none());
			temp_mahjong.set(1, 1);
			GADT_ASSERT(true, temp_mahjong.any());
			GADT_ASSERT(false, temp_mahjong.none());
			temp_mahjong.set(2, 2);
			temp_mahjong.set(3, 3);
			count = 0;
			for (size_t v : temp_mahjong)
			{
				GADT_ASSERT(v, (count > 0 && count < 4 ? count : 0));
				count++;
			}
			GADT_ASSERT(count, temp_mahjong.upper_bound());
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
			temp_mahjong = bitboard::BitMahjong(0x3F, 0);
			GADT_ASSERT(14, temp_mahjong.total());
			temp_mahjong = bitboard::BitMahjong{ { 0,3 },{ 1,2 },{ 2,3 } };
			GADT_ASSERT(8, temp_mahjong.total());
			temp_mahjong -= bitboard::BitMahjong{ { 0,1 },{ 1,1 },{ 2,1 } };
			GADT_ASSERT(5, temp_mahjong.total());
			temp_mahjong += bitboard::BitMahjong{ { 0,1 },{ 1,1 },{ 2,1 } };
			GADT_ASSERT(8, temp_mahjong.total());

			//test ValueVector
			bitboard::ValueVector<14> temp_vec;
			temp_vec.push(2);
			count = 0;
			for (auto v : temp_vec)
			{
				GADT_ASSERT(v, 2);
				count++;
			}
			GADT_ASSERT(count, temp_vec.length());
			GADT_ASSERT(1, temp_vec.length());
			GADT_ASSERT(2, temp_vec.draw_value());
			GADT_ASSERT(2, temp_vec.draw_and_remove_value());
			GADT_ASSERT(0, temp_vec.length());
			temp_vec.push(3);
			GADT_ASSERT(1, temp_vec.length());

			//test ValueVector
			bitboard::ValueVector<5> temp_vec_5{1,2,3,4,5,6};
			GADT_ASSERT(temp_vec_5.get(4), 5);
			GADT_ASSERT(temp_vec_5.length(), 5);
			GADT_ASSERT(temp_vec_5.is_full(), true);
			GADT_ASSERT(temp_vec_5[0], 1);
			GADT_ASSERT(temp_vec_5.upper_bound(), 5);
		}
		void TestFilesystem()
		{
			srand((unsigned int)time(NULL));
			std::string path = "./test_dir"  + ToString(rand() % 99999999);
			if (!gadt::filesystem::exist_directory(path))
			{
				GADT_ASSERT(true, filesystem::create_directory(path));
				GADT_ASSERT(true, filesystem::exist_directory(path));
				GADT_ASSERT(true, filesystem::remove_directory(path));
				GADT_ASSERT(false, filesystem::exist_directory(path));
				GADT_ASSERT(true, filesystem::create_directory(path));
				std::string file_path = path + "/test_file.dat";
				std::ofstream ofs(file_path, std::ios::trunc);
				ofs << "line1" << std::endl << "line2" << std::endl << "line3" << std::endl;
				std::string str = filesystem::load_file_as_string(file_path);
				//std::cout << str;
				GADT_ASSERT(true, filesystem::remove_file(file_path));
				GADT_ASSERT(true, filesystem::remove_directory(path));
			}
			
		}
		void TestIndex()
		{
			player::PlayerRange<AgentIndex, 1, 5> index;
			AgentIndex player = 2;
			player = index.get_next(player);
			GADT_ASSERT(3, player);
			GADT_ASSERT(4, index.get_next(player));
			GADT_ASSERT(2, index.get_prev(player));
			GADT_ASSERT(5, index.get_jump(player,2));
			GADT_ASSERT(1, index.get_jump(player, 3));
			GADT_ASSERT(2, index.get_jump(player, 14));
			GADT_ASSERT(2, index.get_jump(player, 24));
		}
		void TestMctsNode()
		{
			tic_tac_toe::State state;
			mcts::MctsFuncPackage<tic_tac_toe::State, tic_tac_toe::Action, tic_tac_toe::Result, true> func(
				tic_tac_toe::UpdateState,
				tic_tac_toe::MakeAction,
				tic_tac_toe::DetemineWinner,
				tic_tac_toe::StateToResult,
				tic_tac_toe::AllowUpdateValue
			);
			mcts::MctsNode<tic_tac_toe::State, tic_tac_toe::Action, tic_tac_toe::Result, true> node(state, nullptr, func, mcts::MctsSetting());
			gadt::stl::StackAllocator<mcts::MctsNode<tic_tac_toe::State, tic_tac_toe::Action, tic_tac_toe::Result, true>, true> alloc(100);

			auto p = alloc.construct(state, nullptr, func, mcts::MctsSetting());
			GADT_ASSERT(node.action_count(), 9);
			GADT_ASSERT(p->action_count(), 9);
		}
		void TestMctsSearch()
		{
			mcts::MctsSetting setting;
			setting.max_thread = 4;
			setting.max_node_per_thread = 10000;
			setting.max_iteration_per_thread = 10000;
			setting.timeout = 0;
			mcts::MonteCarloTreeSearch<tic_tac_toe::State, tic_tac_toe::Action, tic_tac_toe::Result, true> mcts
			(
				tic_tac_toe::UpdateState,
				tic_tac_toe::MakeAction,
				tic_tac_toe::DetemineWinner,
				tic_tac_toe::StateToResult,
				tic_tac_toe::AllowUpdateValue
			);

			mcts::LockFreeMCTS<tic_tac_toe::State, tic_tac_toe::Action, tic_tac_toe::Result, true> lf_mcts
			(
				tic_tac_toe::UpdateState,
				tic_tac_toe::MakeAction,
				tic_tac_toe::DetemineWinner,
				tic_tac_toe::StateToResult,
				tic_tac_toe::AllowUpdateValue
			);

			mcts::MultiTreeMCTS<tic_tac_toe::State, tic_tac_toe::Action, tic_tac_toe::Result, true> mt_mcts
			(
				tic_tac_toe::UpdateState,
				tic_tac_toe::MakeAction,
				tic_tac_toe::DetemineWinner,
				tic_tac_toe::StateToResult,
				tic_tac_toe::AllowUpdateValue
			);
			//mcts.InitLog(tic_tac_toe::StateToStr, tic_tac_toe::ActionToStr, tic_tac_toe::ResultToStr);
			//mcts.EnableJsonOutput();
			//mcts.EnableLog();

			//lf_mcts.InitLog(tic_tac_toe::StateToStr, tic_tac_toe::ActionToStr, tic_tac_toe::ResultToStr);
			//lf_mcts.EnableJsonOutput();
			//lf_mcts.EnableLog();

			//mt_mcts.InitLog(tic_tac_toe::StateToStr, tic_tac_toe::ActionToStr, tic_tac_toe::ResultToStr);
			//mt_mcts.EnableJsonOutput();
			//mt_mcts.EnableLog();

			tic_tac_toe::State state;
			tic_tac_toe::Action action;
			action = mcts.Run(state, setting);
			GADT_ASSERT((action.x == 1 && action.y == 1), true);

			action = lf_mcts.Run(state, setting);
			GADT_ASSERT((action.x == 1 && action.y == 1), true);

			action = mt_mcts.Run(state, setting);
			GADT_ASSERT((action.x == 1 && action.y == 1), true);

			state.dot[0][0] = tic_tac_toe::WHITE;
			/*for (size_t i = 1; i <= 16; i++)
			{
				setting.thread_num = i;
				tic_tac_toe::Action action = mcts.Run(state, setting);
			}*/
			action = mcts.Run(state, setting);
			GADT_ASSERT((action.x == 1 && action.y == 1), true);

			action = lf_mcts.Run(state, setting);
			GADT_ASSERT((action.x == 1 && action.y == 1), true);

			action = mt_mcts.Run(state, setting);
			GADT_ASSERT((action.x == 1 && action.y == 1), true);
			
		}
		void TestVisualTree()
		{
			visual_tree::VisualTree tree;
			visual_tree::VisualNode::pointer ptr = tree.root_node();
			const size_t ub = 100;
			for (size_t  i = 0; i < ub; i++)
			{
				ptr->set_value("depth", ptr->depth());
				ptr->add_value("sqrt", sqrt((double)ptr->depth()));
				ptr->set_value("name", "hello ");
				ptr->add_value("name", "world!");
				GADT_ASSERT(ptr->string_value("depth"), "");
				GADT_ASSERT((size_t)ptr->integer_value("depth"), ptr->depth());
				ptr->create_child();
				ptr->last_child()->set_value("depth", ptr->last_child()->depth());
				ptr = ptr->create_child();
			}
			ptr->add_value("hello", "world");
			visual_tree::VisualTree new_tree = tree;
			std::ofstream os("JsonTest1.dat");
			new_tree.output_json(os);
			GADT_ASSERT(new_tree.size(), ub*2 +1);
			GADT_ASSERT(new_tree.root_node()->first_child()->count(), 1);
			GADT_ASSERT(new_tree.root_node()->last_child()->count(), ub * 2 - 1);
			tree.clear();
			ptr = tree.root_node()->create_child();
			for (size_t i = 0; i < 10; i++)
			{
				ptr->set_value("number", i);
				ptr->create_child()->set_value("is_leaf", true);
				ptr = ptr->create_child();
			}
			GADT_ASSERT(tree.root_node()->first_child()->first_child()->boolean_value("is_leaf"), true);
			std::ofstream os2("JsonTest2.dat");
			tree.output_json(os2);
		}
		void TestStlList()
		{
			using ActionList = gadt::stl::List<tic_tac_toe::Action>;
			ActionList list(1000);
			for (size_t i = 0; i < 9; i++)
			{
				GADT_ASSERT(list.size(), i);
				list.push_back({ i / 3,i % 3,tic_tac_toe::BLACK });
			}
			size_t count = 0;
			for (list.reset_iterator(); list.is_end() == false; list.to_next_iterator())
			{
				GADT_ASSERT(list.iterator().x, count / 3);
				GADT_ASSERT(list.iterator().y, count % 3);
				count++;
			}
			GADT_ASSERT(count, 9);
			list.clear();
			GADT_ASSERT(list.begin(), nullptr);
			GADT_ASSERT(list.end(), nullptr);
			GADT_ASSERT(list.to_next_iterator(), false);
		}
		void TestStlAllocator()
		{
			struct TestClass
			{
				size_t a;
				size_t b;
				size_t c;
				std::vector<size_t> num;

				TestClass(size_t _a, size_t _b, size_t _c) :
					a(_a),
					b(_b),
					c(_c),
					num{ _a,_b,_c }
				{
				}
			};
			using TestAlloc = stl::StackAllocator<TestClass, true>;
			const size_t ub = 10000;
			TestAlloc alloc(ub);
			for (size_t i = 0; i < ub / 2; i++)
			{
				TestClass* p = alloc.construct(i, i + i, i*i);
				GADT_ASSERT(p->a, i);
				alloc.destory(p);
			}
			GADT_ASSERT(alloc.is_full(), false);
			GADT_ASSERT(alloc.remain_size(), alloc.total_size());
			for (size_t i = 0; i < ub; i++)
			{
				TestClass* p = alloc.construct(i, i + i, i*i);
				GADT_ASSERT(p->a, i);
			}
			GADT_ASSERT(alloc.is_full(), true);
			alloc.flush();
			GADT_ASSERT(alloc.is_empty(), true);
		}
		void TestStlLinearAlloc()
		{
			struct TestClass
			{
				size_t a;
				size_t b;
				size_t c;
				std::vector<size_t> num;

				TestClass(size_t _a, size_t _b, size_t _c) :
					a(_a),
					b(_b),
					c(_c),
					num{ _a,_b,_c }
				{
				}
			};
			using TestAlloc = stl::LinearAllocator<TestClass, true>;
			const size_t ub = 10000;
			TestAlloc alloc(ub);
			for (size_t i = 0; i < ub / 2; i++)
			{
				TestClass* p = alloc.construct(i, i + i, i*i);
				GADT_ASSERT(p->a, i);
				//GADT_ASSERT(p->num[0], i);
				alloc.destory_last();
			}
			GADT_ASSERT(alloc.is_full(), false);
			GADT_ASSERT(alloc.remain_size(), alloc.total_size());
			for (size_t i = 0; i < ub; i++)
			{
				TestClass* p = alloc.construct(i, i + i, i*i);
			}
			GADT_ASSERT(alloc.is_full(), true);
			alloc.flush();
			GADT_ASSERT(alloc.is_empty(), true);
		}
		void TestStlDynamicMatrix()
		{
			stl::DynamicMatrix<size_t>::ElementToStringFunc ElemToString = [](const size_t& i)->std::string {
				return gadt::ToString(i);
			};
			stl::DynamicMatrix<size_t>::StringToElementFunc StringToElem = [](const std::string& str)->size_t {
				return (size_t)gadt::ToInt(str);
			};
			stl::DynamicMatrix<size_t>::ElementToJsonFunc ElemToJson = [](const size_t& i)->json11::Json {
				return json11::Json{ (int)i };
			};
			stl::DynamicMatrix<size_t>::JsonToElementFunc JsonToElem = [](const json11::Json& json)->size_t {
				return (size_t)json.int_value();
			};
			stl::DynamicMatrix<size_t> init_matrix(4, 4, { { 1 }, { 1, 2 }, { 1,2,3 }, { 1,2,3,4,5 }, {6,6} });
			//init_matrix.Print(ElemToString);
			GADT_ASSERT(init_matrix.element(0, 0), 1);
			GADT_ASSERT(init_matrix.element(3, 0), 0);
			GADT_ASSERT(init_matrix.element(3, 3), 4);
			
			stl::DynamicMatrix<size_t> matrix(4, 4);
			for (auto point : matrix)
				matrix[point] = point.x * point.y;
			
			std::string str_json = matrix.ConvertToJsonObj(ElemToString).dump();
			//std::cout << str_json << std::endl;
			matrix.IncreaseRow(1);
			//matrix.Print(ElemToString);
			GADT_ASSERT(matrix.LoadFromJson(str_json, StringToElem), true);
			//matrix.Print(ElemToString);
			std::string int_json = matrix.ConvertToJsonObj(ElemToJson).dump();
			//std::cout << int_json << std::endl;
			matrix.IncreaseRow(1);
			//matrix.Print(ElemToString);
			GADT_ASSERT(matrix.LoadFromJson(int_json, JsonToElem), true);
			//matrix.Print(ElemToString);
			GADT_ASSERT(matrix.element(3, 3), 9);
			matrix.IncreaseRow(5, 0);
			GADT_ASSERT(matrix.element(3, 8), 0);
			GADT_ASSERT(matrix.height(), 9);
			matrix.DecreaseRow(4);
			GADT_ASSERT(matrix.element(3, 4), 0);
			GADT_ASSERT(matrix.height(), 5);
			matrix.IncreaseColumn(3, 0);
			GADT_ASSERT(matrix.element(6, 3), 0);
			GADT_ASSERT(matrix.width(), 7);
			matrix.DecreaseColumn(4);
			GADT_ASSERT(matrix.width(), 3);
			matrix.DecreaseColumn(4);
			GADT_ASSERT(matrix.width(), 0);
			matrix.DecreaseRow(5);
			GADT_ASSERT(matrix.height(), 0);
			matrix.Resize(5, 5);
			GADT_ASSERT(matrix.element(4, 4), 0);
			matrix.Resize(4, 4);
			GADT_ASSERT(matrix.element(3, 3), 0);
			matrix.Resize(0, 0);
			std::string json = matrix.ConvertToJsonObj(ElemToString).dump();
			//std::cout << json << std::endl;
			GADT_ASSERT(matrix.LoadFromJson(json, StringToElem), true);
			matrix.Resize(5, 0);
			json = matrix.ConvertToJsonObj(ElemToString).dump();
			//std::cout << json << std::endl;
			GADT_ASSERT(matrix.LoadFromJson(json, StringToElem), true);
			matrix.Resize(0, 0);
			json = matrix.ConvertToJsonObj(ElemToString).dump();
			//std::cout << json << std::endl;
			GADT_ASSERT(matrix.LoadFromJson(json, StringToElem), true);
			matrix.Resize(5, 5);
			GADT_ASSERT(matrix.LoadFromJson("[1]", StringToElem), false);
			GADT_ASSERT(matrix.LoadFromJson("[[1]]", StringToElem), false);
			GADT_ASSERT(matrix.LoadFromJson("[[\"1\"]]", StringToElem), true);
			//matrix.Print(ElemToString);
			GADT_ASSERT(matrix.width(), 1);
		}
		void TestStlStaticMatrix()
		{
			using Matrix = stl::StaticMatrix<size_t, 4, 4>;
			Matrix matrix;
			for (auto point : matrix)
				matrix[point] = point.x * point.y;
			auto sub = matrix.SubMatrix<3, 3>(0, 0);
			typename Matrix::ElementToStringFunc ElemToString = [](const size_t& i)->std::string {
				return gadt::ToString(i);
			};
			typename Matrix::StringToElementFunc StringToElem = [](const std::string& str)->size_t {
				return (size_t)gadt::ToInt(str);
			};
			typename Matrix::ElementToJsonFunc ElemToJson = [](const size_t& i)->json11::Json {
				return json11::Json{ (int)i };
			};
			typename Matrix::JsonToElementFunc JsonToElem = [](const json11::Json& json)->size_t {
				return (size_t)json.int_value();
			};
			GADT_ASSERT(sub.element(1, 1), 1);
			GADT_ASSERT(sub.element(2, 2), 4);
			sub = matrix.SubMatrix<3, 3>(3, 3);
			GADT_ASSERT(sub.element(0, 0), 9);
			GADT_ASSERT(sub.element(2, 2), 0);
			sub = matrix.SubMatrix<3, 3>(4, 4);
			GADT_ASSERT(sub.element(0, 0), 0);
			GADT_ASSERT(sub.element(2, 2), 0);
			std::string str_json = matrix.ConvertToJsonObj(ElemToString).dump();
			//std::cout << str_json << std::endl;
			//matrix.Print(ElemToString);
			GADT_ASSERT(matrix.LoadFromJson(str_json, StringToElem), true);
			//matrix.Print(ElemToString);
			std::string int_json = matrix.ConvertToJsonObj(ElemToJson).dump();
			//std::cout << int_json << std::endl;
			//matrix.Print(ElemToString);
			GADT_ASSERT(matrix.LoadFromJson(int_json, JsonToElem), true);
			GADT_ASSERT(matrix.LoadFromJson("[1]", StringToElem), false);
			GADT_ASSERT(matrix.LoadFromJson("[[1]]", StringToElem), false);
			GADT_ASSERT(matrix.LoadFromJson("[[\"1\"]]", StringToElem), false);
			GADT_ASSERT(matrix.LoadFromJson("[[\"1\"],[\"1\"],[\"1\"],[\"1\"]]", StringToElem), false);
			//matrix.Print(ElemToString);
			GADT_ASSERT(matrix.width(), 4);

			stl::StaticMatrix<size_t, 1, 4> matrix14;
			std::string json = matrix14.ConvertToJsonObj(ElemToString).dump();
			GADT_ASSERT(matrix.LoadFromJson(json, StringToElem), false);

			stl::StaticMatrix<size_t, 4, 1> matrix41;
			json = matrix41.ConvertToJsonObj(ElemToString).dump();
			GADT_ASSERT(matrix.LoadFromJson(json, StringToElem), false);
			
			
		}
		void TestTable()
		{
			GADT_ASSERT(console::TableCell().str, "");
			GADT_ASSERT(console::TableCell("1").str, "1");
			GADT_ASSERT(console::TableCell("1", console::ConsoleColor::Red).str, "1");
			GADT_ASSERT(console::TableCell("1", console::TableAlign::Middle).str, "1");
			GADT_ASSERT(console::TableCell("1", console::ConsoleColor::Red, console::TableAlign::Middle).str, "1");
			GADT_ASSERT(console::TableCell(int16_t(1), console::ConsoleColor::Red, console::TableAlign::Middle).str, "1");
			GADT_ASSERT(console::TableCell(uint16_t(1), console::ConsoleColor::Red, console::TableAlign::Middle).str, "1");
			GADT_ASSERT(console::TableCell(int64_t(1), console::ConsoleColor::Red, console::TableAlign::Middle).str, "1");
			GADT_ASSERT(console::TableCell(uint64_t(1), console::ConsoleColor::Red, console::TableAlign::Middle).str, "1");
			GADT_ASSERT(console::TableCell(true, console::ConsoleColor::Red, console::TableAlign::Middle).str, "1");
			GADT_ASSERT(console::TableCell(false, console::ConsoleColor::Red, console::TableAlign::Middle).str, "0");
			GADT_ASSERT(console::TableCell(float(1.1), console::ConsoleColor::Red, console::TableAlign::Middle).str, "1.1");
			GADT_ASSERT(console::TableCell(double(1.1), console::ConsoleColor::Red, console::TableAlign::Middle).str, "1.1");

			std::ios::sync_with_stdio(false);
			console::Table table(3, 5,{
				{ "1","2","3" },
				{ "4","5","6" },
				{ "7","8","9" },
				{ "10","11","12" }
			});
			table.set_width({ 4,4,4,4 });
			GADT_ASSERT(table.get_row(0).size(), 3);
			GADT_ASSERT(table.get_column(0).size(), 5);
			GADT_ASSERT(table.get_cell(1,0).str, "2");
			table.set_cell_in_row(0, { "hello", console::ConsoleColor::Blue, console::TableAlign::Right });
			table.set_cell_in_column(1, { "world", console::ConsoleColor::Red, console::TableAlign::Middle });
			GADT_ASSERT(table.get_cell(0, 0).str, "hello");
			GADT_ASSERT(table.get_cell(2, 0).str, "hello");
			GADT_ASSERT(table.get_cell(1, 2).str, "world");
			//table.print();
			//std::cout << table.ConvertToString();
			stl::StaticMatrix<size_t, 5, 5>::ElementToStringFunc ElemToString = [](const size_t& elem)->std::string {
				return gadt::ToString(elem);
			};
			stl::StaticMatrix<size_t, 5, 5> static_matrix(20);
			table.LoadFromStaticMatrix<size_t, 5, 5>(static_matrix, ElemToString);
			GADT_ASSERT(table.get_cell(4, 4).str, "20");
			//table.Print();
			stl::DynamicMatrix<size_t> dynamic_matrix(6, 6, 25);
			table.LoadFromDynamicMatrix<size_t>(dynamic_matrix, ElemToString);
			GADT_ASSERT(table.get_cell(5, 5).str, "25");
			//table.Print();
		}
		void TestMinimax()
		{
			const double timeout = 10;
			const size_t max_depth = 10;
			const bool enable_ab = false;
			
			using Minimax = minimax::MinimaxSearch<tic_tac_toe::State, tic_tac_toe::Action, int64_t, INT64_MAX, INT64_MIN, true>;
			Minimax minimax(
				tic_tac_toe::UpdateState,
				tic_tac_toe::MakeAction,
				tic_tac_toe::DetemineWinner, 
				tic_tac_toe::EvaluateState
				);
			minimax.InitLog(tic_tac_toe::StateToStr, tic_tac_toe::ActionToStr);
			//minimax.EnableJsonOutput();
			//minimax.EnableLog();
			tic_tac_toe::State state;
			state.dot[0][0] = tic_tac_toe::WHITE;
			//state.dot[1][0] = tic_tac_toe::WHITE;
			//state.dot[1][1] = tic_tac_toe::BLACK;
			auto action = minimax.RunNegamax(state, typename Minimax::Setting{ timeout, max_depth });
			GADT_ASSERT((action.x == 1 && action.y == 1), true);
			action = minimax.RunAlphabeta(state, typename Minimax::Setting{ timeout, max_depth });
			GADT_ASSERT((action.x == 1 && action.y == 1), true);
			GADT_ASSERT(0, minimax.GetEvalType(state, typename Minimax::Setting{ timeout, max_depth }));
		}
		void TestRandomPool()
		{
			const size_t ub = 20;
			stl::RandomPool<size_t> pool(ub);
			for (size_t i = 0; i < ub; i++)
			{
				pool.add(i, i);
				GADT_ASSERT(pool.get_weight(i), i);
				GADT_ASSERT(pool[i], i);
				GADT_ASSERT(pool.get_element(i), i);
			}
			//std::cout << pool.info();
			GADT_ASSERT(pool.size(), ub);
			GADT_ASSERT(pool.random() > 0, true);
		}
		void TestMonteCarlo()
		{
			monte_carlo::MonteCarloSetting setting;
			monte_carlo::MonteCarloSimulation<tic_tac_toe::State, tic_tac_toe::Action, tic_tac_toe::Result, true> mc(
				tic_tac_toe::UpdateState,
				tic_tac_toe::MakeAction,
				tic_tac_toe::DetemineWinner,
				tic_tac_toe::StateToResult,
				tic_tac_toe::AllowUpdateValue
			);
			setting.thread_count = 4;
			setting.simulation_times = 10000;
			setting.timeout = 0;
			setting.enable_action_policy = true;

			tic_tac_toe::State state;
			state.dot[0][0] = tic_tac_toe::WHITE;
			mc.InitLog(tic_tac_toe::StateToStr, tic_tac_toe::ActionToStr);
			//mc.EnableLog();
			tic_tac_toe::Action action = mc.DoMonteCarlo(state, setting);
			//GADT_ASSERT((action.x == 1 && action.y == 1), true);
		}
		void TestDynamicArray()
		{
			stl::DynamicArray<size_t> alloc0;
			alloc0.allocate(128);
			for (size_t i = 0; i < 40; i++)
			{
				alloc0.push_back(100);
				alloc0.push_back(101);
				alloc0.push_back(102);
			}
			stl::DynamicArray<size_t> alloc1(alloc0);
			GADT_ASSERT(120, alloc1.size());
			GADT_ASSERT(100, alloc1.front());
			GADT_ASSERT(101, alloc1[1]);
			GADT_ASSERT(102, alloc1.back());
			alloc1.swap(0, 1);
			GADT_ASSERT(101, alloc1[0]);
			GADT_ASSERT(100, alloc1[1]);
			for (size_t i = 0; i < alloc1.size(); i++)
			{
				if (alloc1[i] == 101)
				{
					alloc1.swap(i, alloc1.size() - 1);
					alloc1.pop_back();
					i--;
				}
			}
			GADT_ASSERT(80, alloc1.size());
		}

		const std::vector<FuncPair> func_list = {
			{ "convert"			,TestConvertFunc		},
			{ "point"			,TestPoint				},
			{ "bitboard"		,TestBitBoard			},
			{ "file"			,TestFilesystem			},
			{ "index"			,TestIndex				},
			{ "mcts_node"		,TestMctsNode			},
			{ "mcts"			,TestMctsSearch			},
			{ "visual_tree"		,TestVisualTree			},
			{ "allocator"		,TestStlAllocator		},
			{ "linear_alloc"	,TestStlLinearAlloc		},
			{ "list"			,TestStlList			},
			{ "static_matrix"	,TestStlStaticMatrix	},
			{ "dynamic_matrix"	,TestStlDynamicMatrix	},
			{ "table"			,TestTable				},
			{ "random_pool"		,TestRandomPool			},
			{ "minimax"			,TestMinimax			},
			{ "monte_carlo"		,TestMonteCarlo			},
			{ "dynamic_array"	,TestDynamicArray		}
		};
		void RunTest(FuncPair func_pair)
		{
			cout << endl << ">> test start, target = ";
			console::Cprintf(func_pair.first, console::ConsoleColor::Green);
			timer::TimePoint tp;
			cout << endl;
			func_pair.second();
			cout << ">> test complete, time = ";
			console::Cprintf(tp.time_since_created(), console::ConsoleColor::Red);
			cout << endl;
		}
	}
}