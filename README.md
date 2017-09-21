# Paradoxes, A Game AI Development Toolkit 
---------------------------------
What is Paradoxes?
---------------------------------
Paradoxes is a tiny, cross-platform library for game AI development, which is written by C++11. As the name “Game AI Development Toolkit”, it offers a series tools for Game AI development.

I have implement AI for many games, most of them are board game. However, when I try to implement an AI for a new game by common algorithms like minimax or MCTS, I found I have to rewrite them again because there is not an available algorithm template or library those common game AI algorithm. Base on this reason, the first and main aim of Paradoxes is to offer templates of some frequently-used algorithms in game AI area, like traditional *minimax search* and *Monte Carlo tree search*, the latter one have received great attention in recent years. We use a State-Action-Result model in Paradoxes as the defination method of games, and most game can be defined as this format.

This may sounds like *General Game Playing* (GGP) but there is great difference between them. GGP focus on the problem that how to play various game by one algorithm, what they mainly concerned about is the average performance in various games but not in one particular game. In Paradoxes, we offers various algorithm templates for users to choose. We do not decide which one is better, the user of Paradoxes can define the game logic then run different algorithm on it to decide which algorithm is more suitable for their requirement. All the algorithms template in Paradoxes also offers interfaces which allow users to modify the algorithm quickly, to make the algorithm running more fast or more efficient in particular game by some heuristic function or trick of the game.

Except the algorithm templates, Paradoxes also include a series development tool for game AI like *bitboard* template, *visual search tree* and other powerful tools. More details, see documents.

---------------------------------
Where can I use Paradoxes?
---------------------------------

When you develop a game AI by using common game AI algorithms.

Most games can be defined as the form of State-Action-Result. Users are able to directly use common game AI algorithms framework like *Minimax search*, *Monte Carlo simulation* and *Monte Carlo tree search* after they complete the basic definition of game logic. If debug model are enable explicitly, Paradoxes would do some basic check to ensure the definition of the game logic is correct while those algorithms be executed. Another advantage is that Paradoxes would output the search tree as a JSON file after the search finished if the JsonOutput option was enabled explicitly. Some third-party tools can be used to analysis the JSON file of search tree, or use the analysis tool that is contained in Paradoxes.

----------------------------------
Why Paradoxes? 
----------------------------------

Paradoxes is a tiny cross-platform with no dependency, which is written in C++11 like most state-of-the-art game AI in board games. Before Paradoxes, a library offers similar tools for game AI development is not available. The final aim of Paradoxes is hope to offer an agile development method for game AI by using some common algorithm frameworks, in the same time, guarantee the efficiency of algorthm implements and reserve enough interfaces that allow users to modify the algorithms frameworks quickly for particular game.

Except the algorithms frameworks, Paradoxes also offer a series of tools to enhance the efficiency of game AI development. For the game AI engineers, they do not need to implement same algorithms on different games again and again, and the process they explore which algorithm is more suitable for the problem they faced woule be easier by running various algorithm frameworks with same definition of game logic. For the researcher of game AI, Paradoxes offers industry-level algorithm implement with flexible interfaces that allow them to modify those standard algorithms quickly to test their new idea about the variant of algorithms.

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
Except the algorithms, Paradoxes also offer some debug tools. If you enable the debug mode explicitly, a search report would be output after the search finished each time. a example code to enable search log and JsonOutput option as following:

	MonteCarloTreeSearch<State,Action,Result> mcts(...);
    mcts.InitLog(...);
    mcts.EnableLog();
    mcts.EnableJsonOutput();
    Action action = mcts.DoMcts(state, MctsSetting());

If the log option was enabled, the log woule be output to a ostream (default is std::cout), if the JSON output option was enabled, a JSON file which contain complete information of the search tree would be output after the search finished. It also support users to add custom infomation into the JSON file. 

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












