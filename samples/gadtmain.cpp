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
#include "./games/ewn.h"
#include "./games/breakthrough.h"

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
	auto* game = gadt.CreateShellPage("game");
	auto* doc = gadt.CreateShellPage("doc");
	auto* mcts = gadt.CreateShellPage("mcts");


	root->AddChildPage("test", "start unit test");
	root->AddChildPage("game", "game examples");
	root->AddChildPage("doc", "lib documents");

	breakthrough::DefineBreakthoughShell(gadt);
	renju::DefineRenjuShell(gadt);
	ewn::DefineEwnShell(gadt);
	

	game->AddChildPage("renju", "renju game");
	game->AddChildPage("ewn", "EinSteinw¨¹rfeltnicht! game");
	game->AddChildPage("breakthough", "breakthough game");

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
	gadt.StartFromPage("root","game/breakthough/ls");
}

int main()
{
	//Create Shell
	ShellDefine();
	return 0;
}