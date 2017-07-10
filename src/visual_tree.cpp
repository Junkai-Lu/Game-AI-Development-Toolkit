#include "visual_tree.h"

namespace gadt
{
	namespace visual_tree
	{
		const char* g_VISUAL_TREE_CHILD_KEY = "childs";

		//copy constructor.
		TreeNode::TreeNode(const TreeNode& node, pointer new_parent, VisualTree* new_owner) :
			_dict(node._dict),
			_parent(new_parent),
			_depth(node._depth),
			_count(node._count),
			_owner(new_owner)
		{
			for (pointer p : node._childs)
			{
				pointer ptr = new TreeNode(*p, this, new_owner);
				_childs.push_back(ptr);
			}
		}

		//increase count.
		void TreeNode::incr_count()
		{
			_count++;
			if (_parent != nullptr)
			{
				_parent->incr_count();
			}
		}

		//refresh count of all nodes.
		size_t TreeNode::refresh_count()
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
		void TreeNode::traverse_subtree(std::function<void(reference)> callback)
		{
			callback(*this);
			for (pointer p : _childs)
			{
				p->traverse_subtree(callback);
			}
		}

		//to json string
		std::string TreeNode::to_json() const
		{
			std::stringstream ss;
			ss << "{ ";
			bool is_first = true;
			for (auto pair : _dict)
			{
				if (!is_first)
				{
					ss << ",";
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
				ss << ",\"" << g_VISUAL_TREE_CHILD_KEY << "\":[";
				for (size_t i = 0; i < _childs.size(); i++)
				{
					ss << _childs[i]->to_json();
					if (i != _childs.size() - 1)//is not the last one
					{
						ss << ",";
					}
				}
				ss << "]";
			}
			ss << "}";
			return ss.str();
		}

		//default constructor.
		VisualTree::VisualTree() :
			_root_node{ nullptr,0,this }
		{
		}

		//copy constructor.
		VisualTree::VisualTree(const VisualTree & tree) :
			_root_node{ tree._root_node,nullptr,this }
		{
		}
	}
}


