#include "gadtlib.h"
#include "gameshell.h"
#include "bitboard.h"
#include "mcts.h"

using namespace gadt;
using namespace std;

int main()
{
	GameShell gadt("GADT");
	ShellPage<int>* root = gadt.CreatePage<int>("root");
	gadt.RunPage("root");
	return 0;
}

