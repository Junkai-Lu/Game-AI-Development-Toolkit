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

#include "mcts.hpp"

#pragma once

namespace gadt
{
	namespace mcts
	{
		/*
		* class LockFreeMCTS is a template of lock-free MCTS, which is a lock free and multi-thread(tree parallelization).
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [Result] is the game-result class, which stand for a terminal state of the game.
		* [_is_debug] decides whether debug info would be ignored or not. which may cause slight degradation in performance if it is enabled.
		*/
		template<typename State, typename Action, typename Result = AgentIndex, bool _is_debug = false>
		class LockFreeMCTS final : MctsBase<State, Action, Result, _is_debug>
		{
		public:
			using GameAlgorithmBase<State, Action, Result, _is_debug>::name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::set_name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::InitLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableJsonOutput;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableJsonOutput;
			using MctsBase<State, Action, Result, _is_debug>::_func_package;
			using MctsBase<State, Action, Result, _is_debug>::_setting;
			using MctsBase<State, Action, Result, _is_debug>::Set;
			using MctsBase<State, Action, Result, _is_debug>::MctsIteration;
			using MctsBase<State, Action, Result, _is_debug>::ExcuteIteration;
			using MctsBase<State, Action, Result, _is_debug>::ExcuteMCTS;

		private:

			using Node = MctsNode<State, Action, Result, _is_debug>;				//searcg node.	
			using Allocator = typename Node::Allocator;								//allocator of nodes
			using FuncPackage = MctsFuncPackage<State, Action, Result, _is_debug>;	//function package.

		private:

			//excute mcts iterations.
			void ExcuteIteration(Node& root_node, stl::LinearAllocator<Allocator, _is_debug>& allocators) override
			{
				//thread content
				std::vector<std::thread> threads;

				//enable multithread.
				for (size_t thread_id = 0; thread_id < _setting.max_thread; thread_id++)
				{
					Allocator* thread_allocator = allocators.construct(_setting.max_node_per_thread);
					threads.push_back(std::thread([&](Allocator* allocator)->void {
						MctsIteration(&root_node, allocator);
					}, thread_allocator));
				}

				//join all threads.
				for (size_t i = 0; i < threads.size(); i++)
				{
					threads[i].join();
				}

				//delete the original visit of root node.
				root_node.set_visit_count(root_node.visit_count() - 1);
			}

		public:

			//create monte carlo tree search with necessary functions. 
			LockFreeMCTS(
				typename FuncPackage::UpdateStateFunc		_UpdateState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetermineWinnerFunc	_DetermineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue
			) :
				MctsBase<State, Action, Result, _is_debug>(_UpdateState, _MakeAction, _DetermineWinner, _StateToResult, _AllowUpdateValue)
			{
				set_name("Lock-Free Monte Carlo Tree Search");
			}

			//create monte carlo tree search with function package. 
			LockFreeMCTS(FuncPackage function_package, MctsSetting setting = MctsSetting()) :
				MctsBase<State, Action, Result, _is_debug>(function_package, setting)
			{
				set_name("Lock-Free Monte Carlo Tree Search");
			}

			//run lock-free mcts.
			Action Run(const State& root_state) override
			{
				return ExcuteMCTS(root_state);
			}

			//run lock-free MCTS with custom setting.
			Action Run(const State& root_state, MctsSetting setting) override
			{
				_setting = setting;
				auto result = ExcuteMCTS(root_state);
				return result;
			}
		};

		/*
		* class MultiTreeMCTS is a template of root parallelization MCTS, which is lock-free and multi-thread(root parallelization).
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [Result] is the game-result class, which stand for a terminal state of the game.
		* [_is_debug] decides whether debug info would be ignored or not. which may cause slight degradation in performance if it is enabled.
		*/
		template<typename State, typename Action, typename Result = AgentIndex, bool _is_debug = false>
		class MultiTreeMCTS final : MctsBase<State, Action, Result, _is_debug>
		{
		public:
			using GameAlgorithmBase<State, Action, Result, _is_debug>::name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::set_name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::InitLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableJsonOutput;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableJsonOutput;
			using MctsBase<State, Action, Result, _is_debug>::_func_package;
			using MctsBase<State, Action, Result, _is_debug>::_setting;
			using MctsBase<State, Action, Result, _is_debug>::Set;
			using MctsBase<State, Action, Result, _is_debug>::MctsIteration;
			using MctsBase<State, Action, Result, _is_debug>::ExcuteIteration;
			using MctsBase<State, Action, Result, _is_debug>::ExcuteMCTS;

		private:

			using Node = MctsNode<State, Action, Result, _is_debug>;				//searcg node.	
			using Allocator = typename Node::Allocator;								//allocator of nodes
			using FuncPackage = MctsFuncPackage<State, Action, Result, _is_debug>;	//function package.

		private:

			//excute mcts iterations.
			void ExcuteIteration(Node& root_node, stl::LinearAllocator<Allocator, _is_debug>& allocators) override
			{
				//thread content
				std::vector<std::thread> threads;
				stl::LinearAllocator<Node> root_node_copys(_setting.max_thread - 1);
				for (size_t i = 1; i < _setting.max_thread; i++)
					root_node_copys.construct(root_node.state(), nullptr, _func_package, _setting);//copy

				//enable multithread.
				for (size_t thread_id = 0; thread_id < _setting.max_thread; thread_id++)
				{
					Node* thread_node = &root_node;
					if (thread_id > 0)
						thread_node = root_node_copys[thread_id - 1];

					Allocator* thread_allocator = allocators.construct(_setting.max_node_per_thread);
					threads.push_back(std::thread([&](Node* node, Allocator* allocator)->void {
						MctsIteration(node, allocator);
					},thread_node, thread_allocator));
				}

				//join all threads.
				for (size_t i = 0; i < threads.size(); i++)
				{
					threads[i].join();
				}

				//accmulate values in all copys of root node.
				for (size_t copy_index = 0; copy_index < root_node_copys.size(); copy_index++)
				{
					Node* copy = root_node_copys[copy_index];
					root_node.set_visit_count(root_node.visit_count() + copy->visit_count());
					std::vector<Node*> root_childs = root_node.child_ptr_set();
					std::vector<Node*> copy_childs = copy->child_ptr_set();
					for (size_t i = 0; i < root_childs.size() && i < copy_childs.size(); i++)
					{
						root_childs[i]->set_visit_count(root_childs[i]->visit_count() + copy_childs[i]->visit_count());
						root_childs[i]->set_win_count(root_childs[i]->win_count() + copy_childs[i]->win_count());
					}
				}

				//delete the original visit of root node and its copys.
				root_node.set_visit_count(root_node.visit_count() - static_cast<uint32_t>(_setting.max_thread));
			}

		public:

			//create monte carlo tree search with necessary functions. 
			MultiTreeMCTS(
				typename FuncPackage::UpdateStateFunc		_UpdateState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetermineWinnerFunc	_DetermineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue
			) :
				MctsBase<State, Action, Result, _is_debug>(_UpdateState, _MakeAction, _DetermineWinner, _StateToResult, _AllowUpdateValue)
			{
				set_name("Multi-Tree Monte Carlo Tree Search");
			}

			//create monte carlo tree search with function package. 
			MultiTreeMCTS(FuncPackage function_package, MctsSetting setting = MctsSetting()) :
				MctsBase<State, Action, Result, _is_debug>(function_package, setting)
			{
				set_name("Multi-Tree Monte Carlo Tree Search");
			}

			//run lock-free mcts.
			Action Run(const State& root_state) override
			{
				return ExcuteMCTS(root_state);
			}

			//run lock-free MCTS with custom setting.
			Action Run(const State& root_state, MctsSetting setting) override
			{
				_setting = setting;
				auto result = ExcuteMCTS(root_state);
				return result;
			}
		};
	}
}
