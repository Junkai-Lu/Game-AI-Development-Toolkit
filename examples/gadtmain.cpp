/* Copyright (c) 2018 Junkai Lu <junkai-lu@outlook.com>.
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

#include "../src/gadt_config.h"		//basic config.
#include "../src/gadtlib.h"			//basic library
#include "../src/gadt_filesystem.h"	//file system 
#include "../src/game_shell.h"		//game shell
#include "../src/bitboard.hpp"		//bit board data struct
#include "../src/mcts.hpp"			//monte carlo tree search for games.
#include "../src/minimax.hpp"		//minimax,and alphabeta for games.
#include "../src/monte_carlo.hpp"	//monte carlo simulation algorithm.
#include "../src/mcts.hpp"			//monte carlo tree search for games
#include "../src/parallel_mcts.hpp"	//parallel monte carlo tree search

#include "./games/renju.h"			//game example
#include "./games/ewn.h"			//game example
#include "./games/breakthrough.h"	//game example

#include "gadt_unittest.h"			//a simple unittest

using namespace gadt;

using std::cout;
using std::endl;

void ShellDefine()
{
	//create shell
	shell::GameShell gadt("GADT");

	//add subpage.
	auto game = gadt.root()->CreateChildPage("game", "game examples");
	auto doc  = gadt.root()->CreateChildPage("doc", "lib documents");
	gadt.root()->AddFunctionWithArgs<std::string, int, int>("default", "default version", [](std::string, int, int)->void { std::cout << "hello"; });
	gadt.root()->AddFunctionWithArgs<std::string, int, int>("add", "data version", [](int& a, std::string, int, int c)->void { a += c; std::cout << a << std::endl; });

	//add game examples 
	ewn::DefineEwnShell(game);
	renju::DefineRenjuShell(game);
	breakthrough::DefineBreakthroughShell(game);

	//add unit test page
	shell::TestPage test(gadt.root(), "test", "unit test");
	test.AddFunctionList({
		{ "convert"			,unittest::TestConvertFunc		},
		{ "point"			,unittest::TestPoint			},
		{ "bitboard"		,unittest::TestBitBoard			},
		{ "bitboard64"		,unittest::TestBitBoard64		},
		{ "bitarray"		,unittest::TestBitArray			},
		{ "bitarrayset"		,unittest::TestBitArraySet		},
		{ "file"			,unittest::TestFilesystem		},
		{ "index"			,unittest::TestIndex			},
		{ "mcts_node"		,unittest::TestMctsNode			},
		{ "mcts"			,unittest::TestMctsSearch		},
		{ "visual_tree"		,unittest::TestVisualTree		},
		{ "allocator"		,unittest::TestStlAllocator		},
		{ "linear_alloc"	,unittest::TestStlLinearAlloc	},
		{ "list"			,unittest::TestStlList			},
		{ "static_matrix"	,unittest::TestStlStaticMatrix	},
		{ "dynamic_matrix"	,unittest::TestStlDynamicMatrix },
		{ "table"			,unittest::TestTable			},
		{ "random_pool"		,unittest::TestRandomPool		},
		{ "minimax"			,unittest::TestMinimax			},
		{ "monte_carlo"		,unittest::TestMonteCarlo		},
		{ "dynamic_array"	,unittest::TestDynamicArray		},
		{ "pod_io"			,unittest::TestPodFileIO		},
		{ "arg_convertor"	,unittest::TestArgConvertor		}
		});

	//start shell
	gadt.Run("mttest hello world test");
}

int main()
{
	//Create Shell
	ShellDefine();
	return 0;
}