#include "gadtlib.h"
#include "gadttest.h"
#include "gameshell.h"
#include "bitboard.h"
#include "mcts.h"

using namespace gadt;
using namespace std;

int main()
{
	GameShell gadt("GADT");
	ShellPage<int>* root = gadt.CreatePage<int>("root");
	ShellPage<int>* test = gadt.CreatePage<int>("test");
	root->AddChildPage("test", "start unit test");
	test->AddFunction("all", [](int)->void { gadt::unittest::RunAllTest(); }, "run all test");
	test->AddFunction("bitboard", [](int)->void { gadt::unittest::TestBitBoard(); }, "run bitboard test");
	gadt.RunPage("root");
	return 0;
}

