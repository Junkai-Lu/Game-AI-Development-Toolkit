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

#include "gadtlib.hpp"		//basic library of GADT
#include "gadttest.h"		//a simple unittest
#include "gameshell.h"		//game shell
#include "bitboard.hpp"		//bit board data struct
#include "mcts.hpp"			//monte carlo tree search for games.


int main()
{
	gadt::GameShell gadt("GADT");
	auto* root = gadt::CreateShellPage<int>(gadt,"root");
	//auto* test = gadt::CreateShellPage<int>(gadt, "test");
	GADT_CREATE_SHELL_PAGE(int, test, gadt);
	root->AddInfoFunc([]() {
		gadt::console::Cprintf("=============================================\n", gadt::console::color::gray);
		gadt::console::Cprintf("       FUZHOU MAHJONG AI\n", gadt::console::color::blue);
		gadt::console::Cprintf("       Copyright @ Junkai-Lu 2017\n", gadt::console::color::blue);
		gadt::console::Cprintf("=============================================", gadt::console::color::gray);
		std::cout << endl << endl;
	});
	root->AddChildPage("test", "start unit test");
	test->AddFunction("all", [](int)->void { gadt::unittest::RunAllTest(); }, "run all test");
	test->AddFunction("bitboard", [](int)->void { gadt::unittest::TestBitBoard(); }, "run bitboard test");
	gadt.RunPage("root");
	return 0;
}

