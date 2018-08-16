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
* LIABILITY, WHETHER IN AN T OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "gadt_config.h"

#pragma once

namespace gadt
{
	namespace args
	{
		/*
		* Assign a element in tuple from a string.
		*
		* <Tuple> is the type of tuple that contains all the elements.
		* <N> is the index of element that would be assign here.
		*
		* [tp] is the tuple of parameters.
		* [arg] is the inputed argument, which would be assign to the element.
		* [err] is used to save error if it occured when parsing args, or it should be empty.
		*
		* No return.
		*/
		template<class Tuple, std::size_t N>
		void AssignTupleElement(Tuple& tp, const std::string& arg, std::string& err)
		{
			using ElemType = typename std::tuple_element<N, Tuple>::type;
			std::istringstream convertor(arg);
			if (!(convertor >> std::get<N>(tp)))
			{
				std::get<N>(tp) = ElemType(0);
				std::stringstream ss;
				ss << "unexpected type of arg[" << N << "]: " << arg;
				err += ss.str();
			}
		}

		/*
		* TupleElementGetter is used to gradually get the element of a tuple.
		*
		* <Tuple> is the type of tuple that contains all the elements.
		* <N> is the index of parameter that would be get here.
		*/
		template<class Tuple, std::size_t N>
		struct TupleElementGetter
		{
			/*
			* Fetch a element and assign it.

			* [tuple] is the tuple of parameters.
			* [args] is the inputed arguments.
			* [err] is the details of errors that occured during parsing args, or it should be empty if no error occured.
			*
			* No return.
			*/
			static void Fetch(Tuple& tuple, const std::vector<std::string>& args, std::string& err)
			{
				TupleElementGetter<Tuple, N - 1>::Fetch(tuple, args, err);
				if (err.empty())
				{
					AssignTupleElement<Tuple, N - 1>(tuple, args[N - 1], err);
				}
			}
		};

		/*
		* TupleElementGetter is used to gradually get the element of a tuple.
		* this class is the terminal template specialization of TupleElementGetter.
		*
		* <Tuple> is the type of tuple that contains all the elements.
		*/
		template<typename Tuple>
		struct TupleElementGetter<Tuple, 1>
		{
			/*
			* Fetch a element and assign it.

			* [tuple] is the tuple of parameters.
			* [args] is the inputed arguments.
			* [err] is the details of errors that occured during parsing args, or it should be empty if no error occured.
			*
			* No return.
			*/
			static void Fetch(Tuple& tuple, const std::vector<std::string>& args, std::string& err)
			{
				AssignTupleElement<Tuple, 0>(tuple, args[0], err);
			}
		};

		/*
		* FuncExecutorWithArgs is used to gradually unpack tuple into multi args that target function need.
		*
		* <TupleType> is the type of tuple that contains all the parameters.
		* <FuncType> is the type of target function.
		* <N> is the index of parameter that would be unpacked here.
		* <ExtraArgs...> are extra parameters before unpacked parameters
		*/
		template<typename TupleType, typename FuncType, std::size_t N, class... ExtraArgs>
		struct FuncExecutorWithArgs
		{
			/*
			* Execute a function by parameter that were contained in the inputed tuple.
			*
			* <ReturnType> is the type of return value of function.
			* <ArgTypes...> are types of parameters that had been unpacked from tuple.
			*
			* [tuple] is the tuple of parameters.
			* [func] is the function that would be executed.
			* [args...] are parameters that had been unpacked from tuple.
			*
			* Return: depend on the typename #ReturnType and parameter @func.
			*/
			template<typename ReturnType, class... ArgTypes>
			static ReturnType Execute(const TupleType& tuple, FuncType func, const ExtraArgs&... extra_args, const ArgTypes&... args)//previous arguments.
			{
				constexpr const size_t TUPLE_SIZE = std::tuple_size<TupleType>::value;
				using NextElemType = typename std::tuple_element<N - 1, TupleType>::type;
				using Executor = FuncExecutorWithArgs<TupleType, FuncType, N - 1, ExtraArgs...>;
				return Executor::template Execute<ReturnType, NextElemType, ArgTypes...>(tuple, func, extra_args..., std::get<N - 1>(tuple), args...);
			}
		};

		/*
		* FuncExecutorWithArgs is used to gradually unpack tuple into multi args that target function need.
		* this class is the terminal template specialization of FuncExecutorWithArgs.
		*
		* <TupleType> is the type of tuple that contains all the parameters.
		* <FuncType> is the type of target function.
		* <ExtraArgs...> are extra parameters before unpacked parameters
		*/
		template<typename TupleType, typename FuncType, class... ExtraArgs>
		struct FuncExecutorWithArgs<TupleType, FuncType, 1, ExtraArgs...>
		{
			/*
			* Execute a function by parameter that were contained in the inputed tuple.
			*
			* <ReturnType> is the type of return value of function.
			* <ArgTypes...> are types of parameters that had been unpacked from tuple.
			*
			* [tuple] is the tuple of parameters.
			* [func] is the function that would be executed.
			* [args...] are parameters that had been unpacked from tuple.
			*
			* Return: depend on the typename #ReturnType and parameter @func.
			*/
			template<typename ReturnType, class... ArgTypes>
			static ReturnType Execute(const TupleType& tuple, FuncType func, const ExtraArgs&... extra_args, const ArgTypes&... args)
			{
				return func(extra_args..., std::get<0>(tuple), args...);
			}
		};

		/*
		* ArgConvertor is used to convert several args(string type) into args with target types.
		* for example, ArgConvertor<double, int, std::string> would convert args into those three types by order.
		* the result of convertion can be acquired by function 'parser_success', which would return true if convert success.
		* function 'get_error' would offer the reason of why parser failed if it happened.
		*
		* <ArgTypes...> are the excepted argument types that you want. e.g. <double, int, std::string>, the inputed args would be cast to those types.
		*/
		template<class... ArgTypes>
		class ArgConvertor
		{
		public:

			using ArgList = std::vector<std::string>;
			using ArgTuple = std::tuple<ArgTypes...>;

		private:

			ArgTuple _arg_tuple;
			std::string _err;

		public:

			/*
			* Get the result of parsing args.
			*
			* Return: true if parse success.
			*/
			bool parse_success() const
			{
				return _err.empty();
			}

			/*
			* Get details of occured errors when parsing the args.
			*
			* Return: a string.
			*/
			std::string get_error() const
			{
				return _err;
			}

		public:

			/*
			* Constructor function of ArgConvertor.
			*
			* [args] is a vector of string, such as {"abc", "234"}.
			*/
			ArgConvertor(const ArgList& args) :
				_arg_tuple(),
				_err()
			{
				_arg_tuple = GetTuple(args, _err);
			}

			/*
			* Get a tuple of arguments that may have different type.
			*
			* [args] is a vector of string, such as {"abc", "234"}
			* [err] is used to save error if it occured when parsing args, or it should be empty.
			*
			* Return: a tuple of args.
			*/
			static ArgTuple GetTuple(const ArgList& args, std::string& err)
			{
				ArgTuple tuple;
				err = std::string();
				constexpr const size_t tuple_size = std::tuple_size<ArgTuple>::value;
				if (tuple_size != args.size())
				{
					std::stringstream ss;
					ss << "unexpected size of args. excepted = " << tuple_size << ", size = " << args.size();
					err += ss.str();
					return tuple;
				}
				TupleElementGetter<ArgTuple, tuple_size>::Fetch(tuple, args, err);
				return tuple;
			}

			/*
			* Execute a function with return value and extra args, the types of function arguments equal to the argument type of this class.
			* e.g. the parameters of ArgConvertor<B, C>::ExecuteFunctionWithReturn<A> should be (std::function<void(A, B, C)>, A());
			*
			* <ReturnType> is the type of return value.
			* <ExtraArg> is the type of an addtional passed parameters before unpacked parameters.
			*
			* [func] is target function which must accept excepted args.
			* [extra_args] is the additional passed parameters.
			*
			* Return: depend on the typename #ReturnType and parameter @func.
			*/
			template<class ReturnType, class ExtraArg>
			ReturnType ExecuteFunctionWithReturn(std::function<ReturnType(ExtraArg, ArgTypes...)> func, const ExtraArg& extra_arg)
			{
				using FuncType = std::function<ReturnType(ExtraArg, ArgTypes...)>;
				if (parse_success())
				{
					//if not error occured when convert parameters into tuple.
					constexpr const size_t TUPLE_SIZE = std::tuple_size<ArgTuple>::value;
					return FuncExecutorWithArgs<ArgTuple, FuncType, TUPLE_SIZE, ExtraArg>::template Execute<ReturnType>(_arg_tuple, func, extra_arg);
				}
				return ReturnType();
			}

			/*
			* Execute a function with return value, the types of function arguments equal to the argument type of this class.
			* e.g. the parameter of ArgConvertor<A, B>::ExecuteFunctionWithReturn should be (std::function<void(A, B)>);
			*
			* <ReturnType> is the type of return value.
			*
			* [func] is target function which must accept excepted args.
			*
			* Return: depend on the typename #ReturnType and parameter @func.
			*/
			template<class ReturnType>
			ReturnType ExecuteFunctionWithReturn(std::function<ReturnType(ArgTypes...)> func)
			{
				using FuncType = std::function<ReturnType(ArgTypes...)>;
				if (parse_success())
				{
					//if not error occured when convert parameters into tuple.
					constexpr const size_t TUPLE_SIZE = std::tuple_size<ArgTuple>::value;
					return FuncExecutorWithArgs<ArgTuple, FuncType, TUPLE_SIZE>::template Execute<ReturnType>(_arg_tuple, func);
				}
				return ReturnType();
			}

			/*
			* Execute a function with extra args and no return value, the types of function arguments equal to the argument type of this class.
			* e.g. the parameters of ArgConvertor<B, C>::ExecuteFunction<A> should be (std::function<void(A, B, C )>, A());
			*
			* <ExtraArg> is the type of an addtional passed parameters before unpacked parameters.
			*
			* [func] is a function which must accept excepted args.
			* [extra_args] is the additional passed parameters.
			*
			* No return value.
			*/
			template<class ExtraArg>
			void ExecuteFunction(std::function<void(ExtraArg, ArgTypes...)> func, const ExtraArg& extra_arg)
			{
				ExecuteFunctionWithReturn<void, ExtraArg>(func, extra_arg);
			}

			/*
			* Execute a function without return value, the types of function arguments equal to the argument type of this class.
			* e.g. the parameter of ArgConvertor<A, B>::ExecuteFunction should be (std::function<void(A, B)>);
			*
			* [func] is a function which must accept excepted args.
			*
			* No return value.
			*/
			void ExecuteFunction(std::function<void(ArgTypes...)> func)
			{
				ExecuteFunctionWithReturn<void>(func);
			}
		};
	}
}
