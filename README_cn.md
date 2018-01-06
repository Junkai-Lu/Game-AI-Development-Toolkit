![Logo of Paradoxes](http://junkai.lu/img/Paradoxes/logo.png)

#Paradoxes：一个游戏AI开发工具包

Paradoxes是一个小型的，跨平台游戏AI开发库，其使用C++11编写。正如他的名字“游戏AI开发工具包”那样，它为游戏AI开发提供了一系列的工具。

关于Paradoxes
----------------------------------

在我们为一个特定的游戏实现AI时，第一步通常是定义游戏的表示以及逻辑过程。对于不同的游戏来说，这一步的实现上会有着巨大的差异。第二步是在游戏中实现某种算法， 例如MCTS或者Minimax，而最后一步是测试这个算法是否有效。某些时候优化也会被作为一个额外的步骤，用来降低算法的开销。

Paradoxes提供了许多的算法模板与调试工具，这使得它的用户可以高效地执行第二以及第三个步骤。在Paradoxes之前，还不存在这样一个包含了这些算法模板与调试工具的库。举例来说，用户可以在游戏的逻辑定义结束后，直接地运行一个标准的蒙特卡罗树搜索算法（像UCT）或者极大极小算法而不需要任何额外的代码。同时，算法模板包含了许多的接口来保证它的拓展性。

在算法模板之外，许多开发与调试工具也被包含在Paradoxes之中来使得开发过程可以变得更有效率。例如，用户可以将它们的算法遍历的搜索树输出为一个JSON文件，来检查他们的算法的细节。更多的工具，请参阅Paradoxes的文档。


我在哪里可以使用Paradoxes？
----------------------------------

当你通过使用流行的AI算法来开发一个游戏AI时。

Paradoxes包含了许多流行的算法的模板。如果你希望使用它们来做为你的AI的算法框架，Paradoxes将会是一个好的选择。

如果你还无法确定要应用的算法，Paradoxes提供了一个友好的环境来测试为同一种游戏测试多个算法。所有被包括在Paradoxes之中的算法都共享相同的接口，这意味着可以直接地使用相同的代码去运行不同的AI方法，然后比较它们。


为什么要使用Paradoxes？
----------------------------------

Paradoxes为游戏AI开发提供了一些通用的工具包。每一次当我们需要通过使用一些流行的算法为一个新游戏实现一个新的AI时候，我们不得不重新编写或者修改那些算法。为什么我们不能使用一个通用的算法模板呢？然而，这些算法模板在Paradoxes之前是从未有过的。如果你是一名游戏AI开发者或研究者，这个理由对你来说已经足够了。
在算法之外，Paradoxes还提供了许多额外的功能来加快开发进程。它们与算法模板们享有共同的接口，可以在工程中直接应用。Paradoxes致力于为游戏AI的开发提供完整的工具链，使得这个过程更为简单并更为有效。

特性：

- 轻量级。无依赖。
- 跨平台，支持Windows/Linux
- 涵盖了大部分流行的算法
- 强通用性。支持大部分棋盘/卡牌游戏。

Example
----------------------------------
**Monte Carlo simulation**

    MonteCarloSimulation<State, Action> mc(...);
    MonteCarloSetting setting(...);
    Action action = mc.DoMonteCarlo(state, setting);

**Minimax search**

    MinimaxSearch<State, Action> minimax(...);
    MinimaxSetting setting(...);
    Action action = minimax.DoNegamax(state, setting);

**Monte Carlo tree search**

    MonteCarloTreeSearch<State, Action, Result> mcts(...);
    MctsSetting setting(...);
    Action action = mcts.DoMcts(state, setting);

一个简单的关于井字棋的案例，请参照*./example/gadt_unittest.h*文件


文档
----------------------------------
Paradoxes的文档与教程正在更新。请参见*./doc* 文件夹来获取最新的文档。

如果有任何关于Paradoxes的BUG或者建议，请通过[邮件](Junkai-lu@outlook.com)联系我或者在ISSUE中提出。


版权与协议
----------------------------------
Copyright 2017 JUNKAI LU <junkai-lu@outlook.com>. 

代码基于MIT协议发布。
