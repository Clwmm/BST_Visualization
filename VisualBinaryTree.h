#ifndef VISUAL_BINARY_TREE
#define VISUAL_BINARY_TREE

#include "SFML/Graphics.hpp"
#include <string>
#include <iostream>
#include <cmath>


namespace vbt
{
	constexpr float CIRCLE_SIZE = 23.f;
	constexpr float OFFSET_X_MULTI = 15.f;
	constexpr float OFFSET_Y = 80.f;
	constexpr float MOVE_SPEED = 100.f;
	constexpr float ANIMATION_SPEED = 0.8f;

	enum class Status
	{
		None, Insert, Search, Delete
	};

	template <class T>
	class VisualBinaryNode
	{
	public:
		T key;
		VisualBinaryNode* right = nullptr;
		VisualBinaryNode* left = nullptr;
		VisualBinaryNode* parent = nullptr;

		sf::CircleShape circle;
		sf::Vector2f position;
		sf::Vector2f newPosition;
		bool repositioning = false;
		sf::Text text;

		Status status = Status::None;

		template <class U>
		VisualBinaryNode(U&& key, sf::Vector2f position, sf::Font& font)
		{
			this->key = std::forward<U>(key);
			this->position = position;

			std::string strText = std::to_string(this->key);
			if (strText.size() < 2)
				strText = "0" + strText;
			text.setFont(font);
			text.setCharacterSize(24);
			text.setString(strText);
			text.setPosition(sf::Vector2f(position.x - 13, position.y - 15));

			circle.setPointCount(60);
		}

		void update(const float& deltaTime)
		{
			if (this->repositioning)
			{
				sf::Vector2f dir = newPosition - position;
				float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

				if (dist > 1.0f)
				{
					dir /= dist;
					position.x += dir.x * MOVE_SPEED * deltaTime;
					position.y += dir.y * MOVE_SPEED * deltaTime;
				}
				else
					this->repositioning = false;
			}
		}

		void draw(sf::RenderWindow& window)
		{
			if (this->status == Status::Search)
			{
				circle.setPosition(position);
				circle.setFillColor(sf::Color::Yellow);
				circle.setRadius(CIRCLE_SIZE);
				circle.setOrigin(CIRCLE_SIZE, CIRCLE_SIZE);
				window.draw(circle);
			}
			else
			{
				circle.setPosition(position);
				circle.setFillColor(sf::Color::White);
				circle.setRadius(CIRCLE_SIZE);
				circle.setOrigin(CIRCLE_SIZE, CIRCLE_SIZE);
				window.draw(circle);
			}
			

			circle.setPosition(position);
			circle.setFillColor(sf::Color::Black);
			circle.setRadius(CIRCLE_SIZE - 3.f);
			circle.setOrigin(CIRCLE_SIZE - 3.f, CIRCLE_SIZE - 3.f);
			window.draw(circle);

			std::string strText = std::to_string(this->key);
			if (strText.size() < 2)
				strText = "0" + strText;
			text.setString(strText);
			text.setPosition(sf::Vector2f(position.x - 13, position.y - 15));
			window.draw(text);
		}
	};

	template <class T>
	class VisualBinaryTree
	{
	private:
		using node_ptr = VisualBinaryNode<T>*;
		node_ptr root = nullptr;

		int actualDepth = 0;

		sf::Vertex line[2];
		sf::Font font;

		int size = 0;

		struct UpdateStatus
		{
			Status status = Status::None;
			node_ptr actual_node = nullptr;
			node_ptr previous_node = nullptr;
			T x;
			float time = 0.f;

			void clear()
			{
				if (this->actual_node)
					this->actual_node->status = Status::None;
				if (this->previous_node)
					this->previous_node->status = Status::None;
				this->status = Status::None;
				this->actual_node = nullptr;
				this->previous_node = nullptr;
				this->time = 0.f;
				this->x = NULL;
			}
		};

		UpdateStatus status;

		void repositioning(const int& depth, node_ptr node)
		{
			float offsetX = std::pow(static_cast<float>(depth), 2) * OFFSET_X_MULTI;
			float offsetY = OFFSET_Y;

			if (node == nullptr)
				return;

			if (node->left)
			{
				sf::Vector2f newPosition(node->position.x - offsetX, node->position.y + offsetY);
				node->left->newPosition = newPosition;
				node->left->repositioning = true;
				this->repositioning(depth - 1, node->left);
			}
			if (node->right)
			{
				sf::Vector2f newPosition(node->position.x + offsetX, node->position.y + offsetY);
				node->right->newPosition = newPosition;
				node->right->repositioning = true;
				this->repositioning(depth - 1, node->right);
			}
		}

		template <class U>
		void insert_left(U&& x, node_ptr node, node_ptr parent)
		{
			float offsetX = std::pow(static_cast<float>(actualDepth), 2) * OFFSET_X_MULTI;
			--this->actualDepth;
			if (node == nullptr)
			{
				node = new VisualBinaryNode<T>(std::forward<U>(x), sf::Vector2f(parent->position.x - offsetX, parent->position.y + OFFSET_Y), this->font);
				parent->left = node;
				node->parent = parent;
				return;
			}
			if (x >= node->key)
				this->insert_right(std::forward<U>(x), node->right, node);
			else
				this->insert_left(std::forward<U>(x), node->left, node);
		}

		template <class U>
		void insert_right(U&& x, node_ptr node, node_ptr parent)
		{
			float offsetX = std::pow(static_cast<float>(actualDepth), 2) * OFFSET_X_MULTI;
			--this->actualDepth;
			if (node == nullptr)
			{
				node = new VisualBinaryNode<T>(std::forward<U>(x), sf::Vector2f(parent->position.x + offsetX, parent->position.y + OFFSET_Y), this->font);
				parent->right = node;
				node->parent = parent;
				return;
			}
			if (x >= node->key)
				this->insert_right(std::forward<U>(x), node->right, node);
			else
				this->insert_left(std::forward<U>(x), node->left, node);
		}

		void inorder_(node_ptr node, std::string& text)
		{
			if (node != nullptr)
			{
				this->inorder_(node->left, text);
				text += std::to_string(node->key) + " ";
				this->inorder_(node->right, text);
			}
		}

		void preorder_(node_ptr node, std::string& text)
		{
			if (node != nullptr)
			{
				text += std::to_string(node->key) + " ";
				this->inorder_(node->left, text);
				this->inorder_(node->right, text);
			}
		}

		void postorder_(node_ptr node, std::string& text)
		{
			if (node != nullptr)
			{
				this->inorder_(node->left, text);
				this->inorder_(node->right, text);
				text += std::to_string(node->key) + " ";
			}
		}

		void searchRecursive(node_ptr node, const T& x, std::string& alert)
		{
			if (status.previous_node)
				status.previous_node->status = Status::None;

			if (node != nullptr)
				node->status = Status::Search;
			else
			{
				status.status = Status::None;
				alert = "Not found: " + std::to_string(x);
				return;
			}
			if (node->key == x)
			{
				status.status = Status::None;
				alert = "Found: " + std::to_string(x);
				return;
			}

			status.previous_node = node;

			if (x < node->key)
				status.actual_node = node->left;
			else
				status.actual_node = node->right;
		}

		bool remove_(node_ptr node, const T& key)
		{
			if (node == nullptr)
				return false;

			if (node->key > key)
				return remove_(node->left, key);
			if (node->key < key)
				return remove_(node->right, key);

			--size;

			// ONLY ONE CHILD EXISTS
			if (node->left == nullptr && node->right != nullptr)
			{
				node_ptr right = node->right;
				if (node == root)
				{
					right->parent = nullptr;
					root = right;
					delete node;
					return true;
				}
				right->parent = node->parent;
				if (node->parent->right == node)
					node->parent->right = right;
				else
					node->parent->left = right;
				delete node;
				return true;
			}
			else if (node->right == nullptr && node->left != nullptr)
			{
				node_ptr left = node->left;
				if (node == root)
				{
					left->parent = nullptr;
					root = left;
					delete node;
					return true;
				}
				left->parent = node->parent;
				if (node->parent->right == node)
					node->parent->right = left;
				else
					node->parent->left = left;
				delete node;
				return true;
			}
			// NODE IS A LEAF
			else if (node->right == nullptr && node->left == nullptr)
			{
				if (node == root)
				{
					delete node;
					root = nullptr;
					return true;
				}
				if (node->parent->right == node)
					node->parent->right = nullptr;
				else
					node->parent->left = nullptr;
				delete node;
				return true;
			}
			// NODE HAVE BOTH CHILDS
			else
			{
				Iterator it(node);
				++it;
				node_ptr successor = *it;
				
				if (successor == node->right)
				{
					node->key = successor->key;
					node->position = successor->position;
					if (successor->right)
					{
						node->right = successor->right;
						node->right->parent = node;
					}
					else
						node->right = nullptr;
					delete successor;
					return true;
				}

				if (successor->right == nullptr)
				{
					node->key = successor->key;
					node->position = successor->position;
					successor->parent->left = nullptr;
					delete successor;
					return true;
				}
				else
				{
					node->key = successor->key;
					node->position = successor->position;
					successor->parent->left = successor->right;
					successor->right->parent = successor->parent;
					delete successor;
					return true;
				}
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

		void draw_(sf::RenderWindow& window, node_ptr node)
		{
			if (node == nullptr)
				return;

			if (node->right != nullptr)
			{
				line[0] = node->position;
				line[1] = node->right->position;
				window.draw(line, 2, sf::Lines);
				this->draw_(window, node->right);
			}
			if (node->left != nullptr)
			{
				line[0] = node->position;
				line[1] = node->left->position;
				window.draw(line, 2, sf::Lines);
				this->draw_(window, node->left);
			}

			node->draw(window);
		}

		int depth_(node_ptr node)
		{
			if (node == nullptr)
				return 0;
			int leftDepth = this->depth_(node->left);
			int rightDepth = this->depth_(node->right);
			return std::max(leftDepth, rightDepth) + 1;
		}

	public:

		VisualBinaryTree(sf::Font& font)
		{
			this->font = font;
		}

		struct Iterator
		{
			node_ptr current;
			Iterator(node_ptr node)
				: current(node) {}

			node_ptr operator*() { return current; }
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
		void insert(U&& x, const sf::Vector2f& position = sf::Vector2f(0.f, 0.f))
		{
			++size;
			if (root == nullptr)
			{
				root = new VisualBinaryNode<T>(std::forward<U>(x), position, this->font);
				return;
			}
			this->actualDepth = this->depth();
			if (x >= root->key)
				this->insert_right(std::forward<U>(x), root->right, root);
			else
				this->insert_left(std::forward<U>(x), root->left, root);

		}

		int depth() { return this->depth_(root); }

		std::string inorder()
		{
			std::string text;
			this->inorder_(root, text);
			return text;
		}

		std::string preorder()
		{
			std::string text;
			this->preorder_(root, text);
			return text;
		}

		std::string postorder()
		{
			std::string text;
			this->postorder_(root, text);
			return text;
		}

		void update(const float& deltatime, std::string& alert)
		{
			if (status.status == Status::Search)
			{
				if (status.time <= 0.f)
				{
					this->searchRecursive(status.actual_node, status.x, alert);
					status.time = ANIMATION_SPEED;
				}
				else
					status.time -= deltatime;
			}
			else
			{
				if (status.time <= 0.f)
					status.clear();
				else
					status.time -= deltatime;
			}


			this->repositioning(this->depth(), this->root);
			Iterator it = this->begin();
			for (; it != this->end(); ++it)
			{
				node_ptr temp = *it;
				temp->update(deltatime);
			}
		}

		void search(const T& x)
		{
			status.clear();
			status.status = Status::Search;
			status.actual_node = root;
			status.x = x;
		}

		T minimum()
		{
			if (root == nullptr)
				return 0;
			node_ptr temp = root;
			while (temp->left)
				temp = temp->left;
			return temp->key;
		}

		T maximum()
		{
			if (root == nullptr)
				return 0;
			node_ptr temp = root;
			while (temp->right)
				temp = temp->right;
			return temp->key;
		}

		bool remove(const T& key)
		{
			return remove_(this->root, key);
		}

		void clear()
		{
			this->clear_(root);
			root = nullptr;
			this->size = 0;
		}

		int getSize()
		{
			return this->size;
		}

		node_ptr getRoot() { return this->root; }

		void draw(sf::RenderWindow& window) { this->draw_(window, root); }

		Iterator begin()
		{
			node_ptr temp = root;
			if (temp)
				while (temp->left)
					temp = temp->left;
			return Iterator(temp);
		}
		Iterator end() { return Iterator(nullptr); }
	};

};


#endif // !VISUAL_BINARY_TREE