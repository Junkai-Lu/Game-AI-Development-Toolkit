![Logo of Paradoxes](http://junkai.lu/img/Paradoxes/logo.png)

# Paradoxes : A Game AI Development Toolkit 

---------------------------------
What is Paradoxes?
---------------------------------
Paradoxes is a tiny, cross-platform library for game AI development, which is written by C++11. As the name “Game AI Development Toolkit”, it offers a series tools for Game AI development.

I have ever implemented AI for many games(most are board game). Each time when I try to implement an AI for a new game by common algorithms like minimax, MCTS or something else, I found I have to rewrite them again because there is not an available algorithm template or library of those common game AI algorithm. Base on this reason, I decided to develop Paradoxes. The first and main aim of Paradoxes is to offer templates of some frequently-used algorithms in game AI area, like traditional *minimax search* and *Monte Carlo tree search* which have received great attention in recent years, and some other algorithms. In Paradoxes, we use a State-Action-Result model as the defination method of games, and most game can be defined by this format. Once the defination of game completed, varient game AI algorithms are available.

This may sounds like *General Game Playing* (GGP), but there is great difference between them. GGP focus on the problem that how to play various game by one algorithm, what they mainly concerned about is the average performance in various games but not in one particular game. In Paradoxes, we offers various algorithm templates for users to choose. We do not decide which one is better, the users of Paradoxes can define the game logic then run different algorithms to decide which algorithm is more suitable for their requirement. All the algorithm templates in Paradoxes offer various interfaces which allow users to modify the algorithm by their requirement expediently, or using some heuristic function or trick of the game to make the algorithm running faster and more efficient on particular games.

Except the algorithm templates, Paradoxes also include a series development tool for game AI like *bitboard* template, *visual search tree* and other powerful tools. More details, see documents.

---------------------------------
Where can I use Paradoxes?
---------------------------------

When you develop a game AI by using common game AI algorithms.

Most games can be defined as the form of State-Action-Result. Users are able to directly use common game AI algorithms framework like *Minimax search*, *Monte Carlo simulation* and *Monte Carlo tree search* after they complete the basic definition of game logic. If debug model are enable explicitly, Paradoxes would do some basic check to ensure the definition of the game logic is correct while those algorithms be executed. Another advantage is that Paradoxes would output the search tree as a JSON file after the search finished if the JsonOutput option was enabled explicitly. Some third-party tools can be used to analysis the JSON file of search tree, or use the analysis tool that is contained in Paradoxes.

----------------------------------
Why Paradoxes? 
----------------------------------

Paradoxes is a tiny cross-platform without dependency, which is written in C++11 like most state-of-the-art game AI in board games. Before Paradoxes, there is not an available library offers similar tools for game AI development. The final aim of Paradoxes is hope to offers an agile development method for game AI. While ensuring the speed of development, it also guarantee the efficiency of the implement of algorthms and reserve enough interfaces to allow users to modify the algorithms frameworks quickly according to their requirement.

Except the algorithms frameworks, Paradoxes also offer a series of tools to enhance the efficiency of game AI development. For the game AI engineers, they can use standard game AI algorithms and they no longer need to implement same algorithm on different games again and again. All the algorithms in Paradoxes shared same interfaces, which means the process that engineers explore which algorithm is more suitable for the problem they faced woule be easier by running various algorithm frameworks with same definition of game logic. For the researcher of game AI, Paradoxes offers industry-level algorithm implement with flexible interfaces that allow them to modify those standard algorithms quickly to test their new idea about the variant of algorithms.

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
Except the algorithms, Paradoxes also offer some tools for debug. If you enable the debug mode explicitly, a search report would be output after each search finished. A example code to enable search log and JsonOutput option as following:

	MonteCarloTreeSearch<State,Action,Result> mcts(...);
    mcts.InitLog(...);
    mcts.EnableLog();
    mcts.EnableJsonOutput();
    Action action = mcts.DoMcts(state, MctsSetting());

If the log option was enabled, the log woule be output to a ostream (default is std::cout), if the JSON output option was enabled, a JSON file which contain complete information of the search tree would be output after the search finished. Paradoxes also allows users to add custom infomation into the JSON file. 

----------------------------------
Other Tools
----------------------------------
Paradoxes include a tiny unix-style shell, whihc allow users to execute functions by commands. For game AI developer, they can use the shell to create a work environment quickly. More details, please see document.

Other tools in Paradoxes include a template of BitBoard, which is useful for the development of some board game like chess or checker. The full list of all tools included in Paradoes, see the documents.

----------------------------------
Documents
----------------------------------
The documents of Paradoxes are updating. see ./doc folder to require lattest documents.
If there is any bug or question about Paradoxes, please contact me with email or report issue.

----------------------------------
Copyright and License
----------------------------------
Copyright 2017 JUNKAI LU <junkai-lu@outlook.com>. 

Code released under the MIT license.










