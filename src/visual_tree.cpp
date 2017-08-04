#include "visual_tree.h"

namespace gadt
{
	namespace visual_tree
	{
		const char* g_VISUAL_TREE_CHILD_KEY = "childs";
		const char* g_VISUAL_TREE_COUNT_KEY = "count";
		const char* g_VISUAL_TREE_DEPTH_KEY = "depth";

		//copy constructor.
		VisualNode::VisualNode(const VisualNode* node, pointer new_parent, VisualTree* new_owner) :
			_dict(node->_dict),
			_parent(new_parent),
			_depth(node->_depth),
			_count(node->_count),
			_owner(new_owner)
		{
			for (pointer p : node->_childs)
			{
				pointer ptr = new VisualNode(p, this, new_owner);
				_childs.push_back(ptr);
			}
		}

		//increase count.
		void VisualNode::incr_count()
		{
			_count++;
			if (_parent != nullptr)
			{
				_parent->incr_count();
			}
		}

		//refresh count of all nodes.
		size_t VisualNode::refresh_count()
		{
			if (_childs.size() == 0)
			{
				_count = 1;
				return 1;
			}
			size_t total = 0;
			for (pointer p : _childs)
			{
				total += p->refresh_count();
			}
			_count = total;
			return total;
		}

		//traverse all nodes.
		void VisualNode::traverse_subtree(std::function<void(reference)> callback)
		{
			callback(*this);
			for (pointer p : _childs)
			{
				p->traverse_subtree(callback);
			}
		}

		//to json string
		std::string VisualNode::to_json() const
		{
			std::stringstream ss;
			ss << "{ " << std::endl;
			bool is_first = true;
			for (auto pair : _dict)
			{
				if (!is_first)
				{
					ss << ","<< std::endl;
				}
				else
				{
					is_first = false;
				}
				ss << "\"" << pair.first << "\":";
				if (pair.second.is_integer())
				{
					ss << pair.second.integer_value();
				}
				else if (pair.second.is_float())
				{
					ss << pair.second.float_value();
				}
				else if (pair.second.is_string())
				{
					ss << "\"" << pair.second.string_value() << "\"";
				}
				else if (pair.second.is_boolean())
				{
					ss << std::boolalpha << pair.second.boolean_value();
				}
				else
				{
					ss << "null";
				}
			}
			if (_childs.size() > 0)
			{
				if (!is_first)
				{
					ss << ",";
				}
				ss << std::endl << "\"" << g_VISUAL_TREE_CHILD_KEY << "\":" << std::endl << "[" << std::endl;
				for (size_t i = 0; i < _childs.size(); i++)
				{
					ss << _childs[i]->to_json();
					if (i != _childs.size() - 1)//is not the last one
					{
						ss << ",";
					}
					ss << std::endl;
				}
				ss << "]" << std::endl;
			}
			ss << "}" << std::endl;
			return ss.str();
		}

		//default constructor.
		VisualTree::VisualTree() :
			_root_node{ new VisualNode(nullptr,(size_t)0,this) }
		{
		}

		//copy constructor.
		VisualTree::VisualTree(const VisualTree & tree) :
			_root_node{ new VisualNode(tree._root_node,nullptr,this) }
		{
		}

		void VisualTree::operator=(const VisualTree & tree)
		{
			delete _root_node;
			_root_node = new VisualNode(tree.root_node(), nullptr, this);
		}
	}
}


