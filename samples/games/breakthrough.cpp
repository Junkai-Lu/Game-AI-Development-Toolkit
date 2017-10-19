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

#include "breakthrough.h"

namespace gadt
{
	namespace breakthrough
	{
		void DefineBreakthoughShell(shell::GameShell & shell)
		{
			auto bt = shell.CreateShellPage<BtState>("breakthough");
			bt->AddFunction("print", "print state", [](BtState& state)->void {state.Print(); });
			bt->AddFunction("random", "get random action", [](BtState& state)->void {
				BtActionGenerator generator(state);
				BtAction action = func::GetRandomElement(generator.GetAllAction());
				state.TakeAction(action);
				std::cout << action.to_string() << std::endl;
				state.Print();
			});
			bt->AddFunction("actions", "get all actions", [](BtState& state)->void {
				BtActionGenerator(state).Print();
			});
			bt->AddFunction("winner", "show winner", [](BtState& state)->void {
				switch (state.Winner())
				{
				case BLACK:
					std::cout << "winner = BLACK" << std::endl;
					break;
				case WHITE:
					std::cout << "winner = WHITE" << std::endl;
					break;
				default:
					std::cout << "no winner" << std::endl;
					break;
				}
			});
			bt->AddFunction("mcts", "get mcts move", [](BtState& state)->void {
				mcts::MonteCarloTreeSearch<BtState, BtAction, BtPlayer, true> mcts(
					UpdateState,
					MakeAction,
					DetemineWinner,
					StateToResult,
					AllowUpdateValue
					);
				mcts::MctsSetting setting;
				setting.max_iteration_per_thread = 100000;
				setting.max_node_per_thread = 100000;
				setting.thread_num = 4;
				setting.timeout = 30;
				mcts.InitLog(StateToString, ActionToString, ResultToString);
				mcts.EnableLog();
				BtAction action = mcts.DoMcts(state, setting);
			});
		}

		void UpdateState(BtState & state, const BtAction & action)
		{
			state.TakeAction(action);
		}
		void MakeAction(const BtState & state, BtActionList & list)
		{
			list = BtActionGenerator(state).GetAllAction();
		}
		BtPlayer DetemineWinner(const BtState & state)
		{
			return state.Winner();
		}
		BtPlayer StateToResult(const BtState & state, AgentIndex winner)
		{
			if (winner == BLACK)
				return BLACK;
			else if (winner == WHITE)
				return WHITE;
			return NO_PLAYER;
		}
		bool AllowUpdateValue(const BtState & state, BtPlayer result)
		{
			if (state.next_player() == WHITE && result == BLACK)
				return true;
			if (state.next_player() == BLACK && result == WHITE)
				return true;
			return false;
		}
		std::string StateToString(const BtState & state)
		{
			return std::string();
		}
		std::string ActionToString(const BtAction & action)
		{
			return std::string();
		}
		std::string ResultToString(const BtPlayer & result)
		{
			return std::string();
		}
	}
}

