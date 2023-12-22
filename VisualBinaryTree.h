#ifndef VISUAL_BINARY_TREE
#define VISUAL_BINARY_TREE

#include "SFML/Graphics.hpp"
#include <string>
#include <iostream>


namespace vbt
{
	constexpr float circle_size = 22.f;
	constexpr float offset_x_multi = 15.f;
	constexpr float offset_y = 80.f;
	constexpr float MOVE_SPEED = 100.f;

	template <class T>
	class VisualBinaryNode
	{
	public:

		enum class Status
		{
			None, Insert, Search, Delete
		};

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
		}

		void update(const float& deltaTime)
		{
			if (this->repositioning)
			{
				sf::Vector2f dir = newPosition - position;
				float dist = sqrt(dir.x * dir.x + dir.y * dir.y);

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
			circle.setPosition(position);
			circle.setFillColor(sf::Color::White);
			circle.setRadius(circle_size);
			circle.setOrigin(circle_size, circle_size);
			window.draw(circle);

			circle.setPosition(position);
			circle.setFillColor(sf::Color::Black);
			circle.setRadius(circle_size - 2.f);
			circle.setOrigin(circle_size - 2.f, circle_size - 2.f);
			window.draw(circle);

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

		void repositioning(const int& depth, node_ptr node)
		{
			float offsetX = std::pow(static_cast<float>(depth), 2) * offset_x_multi;
			float offsetY = offset_y;

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
			float offsetX = std::pow(static_cast<float>(actualDepth), 2) * offset_x_multi;
			--this->actualDepth;
			if (node == nullptr)
			{
				node = new VisualBinaryNode<T>(std::forward<U>(x), sf::Vector2f(parent->position.x - offsetX, parent->position.y + offset_y), this->font);
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
			float offsetX = std::pow(static_cast<float>(actualDepth), 2) * offset_x_multi;
			--this->actualDepth;
			if (node == nullptr)
			{
				node = new VisualBinaryNode<T>(std::forward<U>(x), sf::Vector2f(parent->position.x + offsetX, parent->position.y + offset_y), this->font);
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

		void update(const float& deltatime)
		{
			this->repositioning(this->depth(), this->root);
			Iterator it = this->begin();
			for (; it != this->end(); ++it)
			{
				node_ptr temp = *it;
				temp->update(deltatime);
			}
		}

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