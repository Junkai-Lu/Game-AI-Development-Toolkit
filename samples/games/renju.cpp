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

#include "renju.h"

namespace gadt
{
	namespace renju
	{
		void UpdateState(RenjuState & state, const RenjuAction & action)
		{
			state.TakeAction(action);
		}

		void MakeAction(const RenjuState & state, RenjuActionList & list)
		{
			RenjuActionGenerator generator(state);
			list = generator.GetNearbyActions(state.next_player());
		}

		RenjuPlayer DetemineWinner(const RenjuState & state)
		{
			if (state.is_draw())
				if (state.winner() != 0)
					return (RenjuPlayer)state.winner();
				else
					return DRAW;
			return (RenjuPlayer)state.winner();
		}

		RenjuResult StateToResult(const RenjuState & state, AgentIndex winner)
		{
			return (RenjuPlayer)state.winner();
		}

		bool AllowUpdateValue(const RenjuState & state, RenjuResult winner)
		{
			if (state.next_player() == BLACK && winner == WHITE)
			{
				return true;
			}
			if (state.next_player() == WHITE && winner == BLACK)
			{
				return true;
			}
			return false;
		}

		std::string StateToString(const RenjuState & state)
		{
			return "...";
		}

		std::string ActionToString(const RenjuAction & action)
		{
			return action.to_string();
		}

		std::string ResultToString(const RenjuResult & result)
		{
			return gadt::ToString(int(result));
		}

		void PrintRenjuState(const RenjuState& state)
		{
			log::ConsoleTable tb(state.width(), state.height());
			tb.set_width(1);
			std::stringstream ss;
			ss << "player:" << (int)state.next_player() << " winner:" << (int)state.winner();
			tb.enable_title({ ss.str() });
			for (auto coord : state)
			{
				if (state.piece(coord) == BLACK)
				{
					tb.set_cell({ "¡ñ" }, coord);
				}
				else if (state.piece(coord) == WHITE)
				{
					tb.set_cell({ "¡ð" }, coord);
				}
				else if (state.piece(coord) == FLAG)
				{
					tb.set_cell({ "¡î" }, coord);
				}
				else
				{
					if (coord.x == 0)
					{
						if (coord.y == 0)
							tb.set_cell({ "©³" }, coord);
						else if (coord.y == state.height() - 1)
							tb.set_cell({ "©»" }, coord);
						else
							tb.set_cell({ "©Ç" }, coord);
					}
					else if (coord.x == state.width() - 1)
					{
						if (coord.y == 0)
							tb.set_cell({ "©·" }, coord);
						else if (coord.y == state.height() - 1)
							tb.set_cell({ "©¿" }, coord);
						else
							tb.set_cell({ "©Ï" }, coord);
					}
					else
					{
						if (coord.y == 0)
							tb.set_cell({ "©×" }, coord);
						else if (coord.y == state.height() - 1)
							tb.set_cell({ "©ß" }, coord);
						else
							tb.set_cell({ "©í" }, coord);
					}
				}
			}
			tb.print(log::DISABLE_FRAME, log::INDEX_FROM_ZERO);
		}

		void PrintRenjuActions(const RenjuState& state, RenjuActionList action_list)
		{
			RenjuState temp = state;
			for (auto act : action_list)
			{
				temp.TakeAction({act.coord, FLAG});
			}
			std::cout << "Action Num: " << action_list.size();
			PrintRenjuState(temp);
		}

		void DefineRenjuShell(shell::GameShell& shell)
		{
			auto* renju = shell.CreateShellPage<RenjuState>("renju", 1);
			renju->AddFunction("print", "print state", [](RenjuState& state) {
				PrintRenjuState(state);
			});
			renju->AddFunction("take", "take action", [](RenjuState& state, const shell::ParamsList& params) {
				if (params.size() >= 3)
				{
					RenjuAction action = {
						{ ToInt(params[0]),ToInt(params[1]) },
						RenjuPlayer(ToInt(params[2]))
					};
					if (state.IsLegalAction(action))
					{
						state.TakeAction(action);
						PrintRenjuState(state);
						return;
					}
				}
				std::cout << "illegal action" << std::endl;
			});
			renju->AddFunction("winner", "show winner", [](RenjuState& state) {
				std::cout << "winner = " << (int)state.winner() << std::endl;
			});
			renju->AddFunction("reset", "reset state", [](RenjuState& state) {
				state = RenjuState(1);
			});
			renju->AddFunction("actions", "show actions", [](RenjuState& state, const shell::ParamsList& params) {
				renju::RenjuActionGenerator generator(state);
				if (params.size() > 0)
				{
					if (params[0] == "all")
					{
						PrintRenjuActions(state, generator.GetAllActions(state.next_player()));
						return;
					}
				}
				PrintRenjuActions(state, generator.GetNearbyActions(state.next_player()));
			});
			renju->AddFunction("mcts", "run mcts", [](renju::RenjuState& state) {
				RenjuMCTS mcts(
					UpdateState,
					MakeAction,
					DetemineWinner,
					StateToResult,
					AllowUpdateValue);
				mcts.InitLog(StateToString, ActionToString, ResultToString);
				mcts.EnableLog();
				mcts::MctsSetting setting;
				setting.max_node_per_thread = 100000;
				setting.max_iteration_per_thread = 100000;
				setting.no_winner_index = 0;
				setting.simulation_warning_length = 382;
				setting.thread_num = 4;
				setting.timeout = 15;
				auto act = mcts.DoMcts(state, setting);
				std::cout << "input 0 to take action: ";
				size_t n;
				std::cin >> n;
				if (n == 0)
				{
					state.TakeAction(act);
					PrintRenjuState(state);
				}
			});
		}
	}
}

