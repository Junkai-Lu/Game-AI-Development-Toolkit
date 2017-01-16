#include "stdafx.h"
#include "gameshell.h"
#include "bitboard.h"
#include "mcts.h"

using namespace gadt;
using namespace std;

void test()
{

}

int main()
{
	test();
	system("pause");
	GameShell gadt("GADT");
	ShellPage<int>* root = gadt.CreatePage<int>("root");
	gadt.RunPage("root");
	return 0;
}

