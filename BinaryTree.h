// Mateusz Ka³wa

#ifndef BINARY_TREE
#define BINARY_TREE

#include <iostream>
#include <stack>

template <class T>
struct BinaryNode
{
	T value;
	BinaryNode* right;
	BinaryNode* left;
	BinaryNode* parent;
	template <class U>
	BinaryNode(U&& x)
		: value(std::forward<U>(x)), right(nullptr), left(nullptr), parent(nullptr) {}
};

template <class T>
class BinaryTree
{
private:
	using node_ptr = BinaryNode<T>*;
	node_ptr root = nullptr;
	int size_ = 0;

	template <class U>
	void insert_left(node_ptr node, node_ptr parent, U&& x)
	{
		if (node == nullptr)
		{
			node = new BinaryNode<T>(std::forward<U>(x));
			parent->left = node;
			node->parent = parent;
			return;
		}
		if (x >= node->value)
			this->insert_right(node->right, node, std::forward<U>(x));
		else if (x < node->value)
			this->insert_left(node->left, node, std::forward<U>(x));
	}
	template <class U>
	void insert_right(node_ptr node, node_ptr parent, U&& x)
	{
		if (node == nullptr)
		{
			node = new BinaryNode<T>(std::forward<U>(x));
			parent->right = node;
			node->parent = parent;
			return;
		}
		if (x >= node->value)
			this->insert_right(node->right, node, std::forward<U>(x));
		else if (x < node->value)
			this->insert_left(node->left, node, std::forward<U>(x));
	}

	node_ptr searchRecursive_(node_ptr node, const T& x)
	{
		if (node == nullptr || node->value == x)
			return node;

		if (x < node->value)
			return this->searchRecursive_(node->left, x);
		else
			return this->searchRecursive_(node->right, x);
	}

	T minimum_(node_ptr node)
	{
		if (node->left == nullptr)
			return node->value;
		else
			return minimum_(node->left);
	}

	T maximum_(node_ptr node)
	{
		if (node->right == nullptr)
			return node->value;
		else
			return maximum_(node->right);
	}

	int depth_(node_ptr node)
	{
		if (node == nullptr)
			return 0;
		else
		{
			int leftDepth = this->depth_(node->left);
			int rightDepth = this->depth_(node->right);

			return std::max(leftDepth, rightDepth) + 1;
		}
	}

	int minimumNodeDepth_(node_ptr node)
	{
		if (node == nullptr)
			return 0;
		else
			return minimumNodeDepth_(node->left) + 1;
	}

	int maximumNodeDepth_(node_ptr node)
	{
		if (node == nullptr)
			return 0;
		else
			return maximumNodeDepth_(node->right) + 1;
	}

	void inorder_(node_ptr node)
	{
		if (node != nullptr)
		{
			this->inorder_(node->left);
			std::cout << node->value << " ";
			this->inorder_(node->right);
		}
	}

	void preorder_(node_ptr node)
	{
		if (node != nullptr)
		{
			std::cout << node->value << " ";
			this->preorder_(node->left);
			this->preorder_(node->right);
		}
	}

	void postorder_(node_ptr node)
	{
		if (node != nullptr)
		{
			this->preorder_(node->left);
			this->preorder_(node->right);
			std::cout << node->value << " ";
		}
	}

	void clear_(node_ptr node)
	{
		if (node != nullptr)
		{
			clear_(node->left);
			clear_(node->right);
			delete node;
		}
	}

public:

	~BinaryTree()
	{
		this->clear_(root);
	}

	struct Iterator
	{
		node_ptr current;
		Iterator(node_ptr node)
			: current(node)
		{
			if (current)
				while (current->left)
					current = current->left;
		}

		T& operator*() { return current->value; }
		Iterator& operator++()
		{
			if (current->right)
			{
				current = current->right;
				if (current->left)
					while (current->left)
						current = current->left;
			}
			else
			{
				node_ptr parent = current->parent;
				while (parent != nullptr)
				{
					if (current != parent->right)
						break;
					current = parent;
					parent = parent->parent;
				}
				current = parent;
			}
			return *this;
		}

		bool operator==(const Iterator& other) const
		{
			return this->current == other.current;
		}

		bool operator!=(const Iterator& other) const
		{
			return this->current != other.current;
		}

	};

	template <class U>
	void insert(U&& x)
	{
		++size_;
		if (root == nullptr)
		{
			root = new BinaryNode<T>(std::forward<U>(x));
			return;
		}
		if (x >= root->value)
			this->insert_right(root->right, root, std::forward<U>(x));
		else if (x < root->value)
			this->insert_left(root->left, root, std::forward<U>(x));
	}

	node_ptr search(const T& x)
	{
		node_ptr current = root;
		while (current != nullptr && current->value != x)
		{
			if (x < current->value)
				current = current->left;
			else
				current = current->right;
		}
		return current;
	}

	node_ptr searchRecursive(const T& x)
	{
		return this->searchRecursive_(root, x);
	}

	int size()
	{
		return this->size_;
	}

	T minimum()
	{
		return minimum_(root);
	}

	T maximum()
	{
		return maximum_(root);
	}

	int depth()
	{
		return this->depth_(root);
	}

	void inorder()
	{
		this->inorder_(root);
		std::cout << std::endl;
	}

	void preorder()
	{
		this->preorder_(root);
		std::cout << std::endl;
	}

	void postorder()
	{
		this->postorder_(root);
		std::cout << std::endl;
	}

	void clear()
	{
		delete root;
		root = nullptr;
	}

	int minimumNodeDepth()
	{
		return minimumNodeDepth_(root);
	}

	int maximumNodeDepth()
	{
		return maximumNodeDepth_(root);
	}

	node_ptr getRoot()
	{
		return root;
	}

	Iterator begin() { return Iterator(this->root); }
	Iterator end() { return Iterator(nullptr); }
};

#endif // !BINARY_TREE