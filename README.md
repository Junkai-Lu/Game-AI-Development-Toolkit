# Paradoxes, A Game AI Development Toolkit 
---------------------------------
What is Paradoxes?
---------------------------------
Paradoxes is a tiny, cross-platform library for game AI development, which is wrote by C++11. As the name “Game AI Development Toolkit”, it offers a series tools for Game AI development under C++11. 

I have implement AI for many games, most of them are board game. When I try to implement an AI for a new game, I need to rewrite the algorithms like minimax or MCTS for the new game. The first and main aim of Paradoxes is to offer templates of some frequently-used algorithms in Game AI, like traditional minimax search and Monte Carlo tree search. We use a State-Action-Result model in Paradoxes, and most game can be defined by this model.

This may sounds like General Game Playing (GGP) but there is great difference. GGP focus on the problem that how to play various game by one algorithm, what they mainly concerned about is the average performance in various games but not in one particular game. In Paradoxes, we offers various algorithm templates for users to choose. We do not decide which one is better, the user of Paradoxes can define a game then run different algorithm on it to decide which algorithm is suit of their requirement. All the algorithms template in Paradoxes also offers interfaces that allow user to modify the algorithm quickly, to make the algorithm running more fast or efficient in particular game.

Except the algorithm templates, Paradoxes also include a series development tool for game AI like bitboard template, visual search tree and other powerful tools. More details, see documents.


---------------------------------
Where can I use Paradoxes?
---------------------------------

When you develop an AI for a game by using common game AI algorithms.

Most games can be defined as the form of State-Action-Result. Users are able to use common game AI algorithms framework like Minimax, Monte Carlo simulation and Monte Carlo tree search after basic definition of game logic. If debug model are enable explicitly, Paradoxes would do some basic check to make sure the definition of the game logic is correct while those algorithms are running. Paradoxes would output the search tree as a JSON file after the search finished if the user enable the JsonOutput option explicitly. Some third-party tools can be used to analysis the JSON file of search tree, or use the analysis tool that is contained in Paradoxes.

----------------------------------
Why Paradoxes? 
----------------------------------

Paradoxes is a tiny cross-platform with no dependency, which is written in C++11 like most state-of-the-art game AI in board game. Before Paradoxes, a library offers similar tools for game AI development is not available. The aim of Paradoxes is hope to offer an agile development method for game AI by using some common algorithm frameworks, guarantee the efficiency of codes and reserve enough interfaces that allow users to modify the algorithms frameworks quickly for particular game.

Except the algorithms frameworks, Paradoxes also offer a series of tools to enhance the efficiency of game AI development. For the engineers of game AI, they do not need to implement same algorithms for different games again and again, and it would be easier for them to explore which algorithm is more suitable for the problem they faced by running various algorithm frameworks with same definition of game logic. For the researcher of game AI, Paradoxes offers industry-level algorithm implement with flexible interfaces that allow them to modify those standard algorithms quickly to test their new idea about the variant of algorithms.

----------------------------------
Example
----------------------------------
**Monte Carlo simulation**

    MonteCarloSimulation<State, Action, Result, true> mc(...);
	MonteCarloSetting setting(...);
	Action action = mc.DoMonteCarlo(state, setting);

**Minimax search**

	MinimaxSearch<State, Action, true> minimax(...);
	MinimaxSetting setting(...);
	Action action = minimax.DoNegamax(state, setting);

**Monte Carlo tree search**

	MonteCarloTreeSearch<State, Action, Result, true> mcts(...);
	MctsSetting setting(...);
	Action action = mcts.DoMcts(state, setting);

there is a simple example about tic-tac-toe, see *gadt_unittest.h*.

----------------------------------
Debug Tools
----------------------------------
Except the algorithms, Paradoxes also offer some debug tools. If you enable the debug model explicitly, a search report would be output after each search finished. a example to enable search log as following:

	MonteCarloTreeSearch<State,Action,Result> mcts(...);
    mcts.InitLog(...);
    mcts.EnableLog();
    mcts.EnableJsonOutput();
    Action action = mcts.DoMcts(state, MctsSetting());

If the log was enabled, the log woule be output to a ostream like std::cout, if the JSON output was enabled, a JSON file which contain complete information of the search tree would be output.

----------------------------------
Other Tools
----------------------------------
Paradoxes include a tiny unix-style shell, whihc allow users to execute functions by commands. For game AI developer, they can use the shell to create a work environment quickly. More details, please see document.

Other tools in Paradoxes include a template of BitBoard, which is powerful for the development of some board game like chess or checker. The list of all tools included in Paradoes, see the documents.

----------------------------------
Documents
----------------------------------
Paradoxes would offer full documents support in next time.
If there is any bug or question about Paradoxes, please contact me with email or report issue.












