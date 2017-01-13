#include "stdafx.h"
#include "gameshell.h"
#include "bitboard.h"
#include "mcts.h"

using namespace gadt;

int main()
{
	GameShell gadt("GADT");
	ShellPage<int>* root = gadt.CreatePage<int>("root", 1);
	root->AddFunction("hello", [](int) {std::cout << "Thanks for using GADT." << std::endl; }, "say hello");
	gadt.RunPage("root");
	return 0;
}

