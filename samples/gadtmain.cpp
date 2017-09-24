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


#include "../src/gadtlib.h"			//basic library of GADT
#include "../src/game_shell.h"		//game shell
#include "../src/bitboard.hpp"		//bit board data struct
#include "../src/mcts.hpp"			//monte carlo tree search for games.
#include "../src/minimax.hpp"		//minimax,and alphabeta for games.
#include "../src/monte_carlo.hpp"	//monte carlo simulation algorithm.
#include "../src/mcts.hpp"			//monte carlo tree search for games, new version.
#include "./games/renju.h"

#include "gadt_unittest.h"			//a simple unittest

using namespace gadt;

using std::cout;
using std::endl;

void ShellDefine()
{
	shell::GameShell gadt("GADT");

	//Define Root Page.
	gadt.SetDefaultInfoFunc([]()->void{
		console::Cprintf("=============================================\n", console::GRAY);
		console::Cprintf("       Game AI Development Toolkit\n", console::YELLOW);
		console::Cprintf("       Copyright @ Junkai-Lu 2017\n", console::YELLOW);
		console::Cprintf("=============================================", console::GRAY);
		std::cout << endl << endl;
	});

	auto* root = gadt.CreateShellPage("root");
	auto* test = gadt.CreateShellPage("test");
	auto* mcts = gadt.CreateShellPage("mcts");
	auto* game = gadt.CreateShellPage("game");
	auto* renju = gadt.CreateShellPage<renju::RenjuState>("renju");

	root->AddChildPage("test", "start unit test");
	root->AddChildPage("game", "game examples");

	game->AddChildPage("renju", "renju game");

	renju->AddFunction("print", "print state", [](renju::RenjuState& state) {state.Print(); });
	renju->AddFunction("take", "take action", [](renju::RenjuState& state, const shell::ParamsList& params) {
		if (params.size() >= 3)
		{
			renju::RenjuAction action = {
				{ console::ToInt(params[0]),console::ToInt(params[1]) },
				renju::RenjuPlayer(console::ToInt(params[2]))
			};
			if (state.is_legal_action(action))
			{
				state.TakeAction(action);
				state.Print();
				return;
			}
		}
		std::cout << "illegal action" << std::endl;
	});
	renju->AddFunction("winner", "show winner", [](renju::RenjuState& state) {
		std::cout << "winner = " << (int)state.winner() << std::endl; 
	});
	renju->AddFunction("reset", "reset state", [](renju::RenjuState& state) {
		state = renju::RenjuState();
	});
	renju->AddFunction("actions", "show actions", [](renju::RenjuState& state, const shell::ParamsList& params) {
		renju::RenjuActionGenerator generator(state);
		if (params.size() > 0)
		{
			if (params[0] == "all")
			{
				generator.Print(generator.GetAllActions());
			}
			else
			{
				size_t dist = size_t(console::ToInt(params[0]));
				generator.Print(generator.GetNearbyActions(dist));
			}
		}
		else
		{
			std::cout << "need 1 parameter, 'all' or number" << std::endl;
		}
		
	});

	//Unit Test Page
	for (auto p : unittest::func_list)
	{
		test->AddFunction(p.first, "test lib " + p.first, [=](){
			unittest::RunTest(p);
		});
	}
	test->AddFunction("all","run all test", [](){
		for (auto p : unittest::func_list)
		{
			unittest::RunTest(p);
		}
	});
	test->AddChildPage("mcts", "monte carlo tree search test");
	mcts->AddFunction("node", "node part", [](){unittest::TestMctsNode(); });
	mcts->AddFunction("search","search part", [](){unittest::TestMctsSearch(); });

	//Start Shell
	gadt.StartFromPage("root", "game/renju");
}

int main()
{
	//Create Shell
	ShellDefine();
	return 0;
}