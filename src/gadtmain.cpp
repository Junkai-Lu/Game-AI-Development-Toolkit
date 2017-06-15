/* 
* Game AI Development Toolkit
*
* The game ai development toolkit(gadt) is a tiny library for C++11, providing tools for game AI development.
* 
* The core content provided by the library include three kinds of functions. they are data 
* struct for games such as bitboard, bitset for pokers; algorithm for games, such as monte
* carlo tree search, minimax search, alpha-beta search and other famous algorithms for game
* ai. the final part include some tiny functions, such a simple shell-based GUI library, a
* tiny state machine template and so on.
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


#include "gadtlib.h"		//basic library of GADT
#include "gadttest.h"		//a simple unittest
#include "gameshell.h"		//game shell
#include "bitboard.hpp"		//bit board data struct
#include "mcts.hpp"			//monte carlo tree search for games.
#include "minimax.hpp"		//minimax,and alphabeta for games.

#include "mcts_new.hpp"

using namespace gadt;

using std::cout;
using std::endl;

void ShellDefine()
{
	GameShell gadt("GADT");
	auto* root = CreateShellPage<int>(gadt, "root");
	auto* test = CreateShellPage<int>(gadt, "test");

	//Define Root Page.
	root->AddInfoFunc([]() {
		console::Cprintf("=============================================\n", console::GRAY);
		console::Cprintf("       Game AI Development Toolkit\n", console::YELLOW);
		console::Cprintf("       Copyright @ Junkai-Lu 2017\n", console::YELLOW);
		console::Cprintf("=============================================", console::GRAY);
		std::cout << endl << endl;
	});
	root->AddChildPage("test", "start unit test");

	//Unit Test Page
	for (auto p : unittest::func_list)
	{
		test->AddFunction(p.first, [=](int)->void {
			unittest::RunTest(p);
		}, "test lib " + p.first);
	}
	test->AddFunction("all", [](int)->void {
		for (auto p : unittest::func_list)
		{
			unittest::RunTest(p);
		}
	}, "run all test");

	test->AddFunction("hey", [](int)->void {
	}, "hey");

	//Start Shell
	gadt.RunPage("root");
}

int main()
{
	//Create Shell
	ShellDefine();
	return 0;
}