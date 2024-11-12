#pragma once
#include <vector>
#include <iostream>
#include <functional>
#include <memory>

using std::vector;
using std::string;
using std::unique_ptr;
using std::shared_ptr;


class Node
{
public:
	Node() { }
	Node(char32_t ch) : word(ch) { }

	~Node() { }
	shared_ptr<Node> getIndexAtChar(char32_t ch) {
		for (int i = 0; i < this->nodes.size(); ++i) {
			if (Node::nodes[i]->word == ch) {
				return nodes[i];
			}
		}
		
		return nullptr;
	}

	vector<shared_ptr<Node>>& getNode() {
		return nodes; 
	}

	void insertNode(char32_t ch) {
		nodes.push_back(std::make_shared<Node>(ch));
	}

	// 设置为单词结尾
	void Node::setEnd() {
		this->isWord = true;
	}

	bool Node::isEnd() const {
		return isWord;
	}

	bool contain(char32_t ch){
		bool ans = false;
		std::for_each(nodes.cbegin(), nodes.cend(), [&](const auto& node) {
			if (node->word == ch) {
				ans = true;
				return;
			}
			});
		return ans;
	}		

	void Node::setWord(char32_t ch) {
		this->word = ch;
	}

	char32_t Node::getWord() const {
		return word;
	}

private:
	vector<shared_ptr<Node> > nodes{};
	char32_t word{};
	int times{ 0 };
	bool isWord{ false };

	std::pair<shared_ptr<Node>, int> vecContains{};
};

class PrefixTree
{
public:
	PrefixTree() : root(std::make_unique<Node>()){ };
	~PrefixTree() { root.reset(); }

	void insert(const string& str) {
		if (str == "") return;

		int i = 0;

		shared_ptr<Node> p = root;
		while (p->contain(str[i]))
		{
			p = p->getIndexAtChar(str[i]);
			i++;
		}
		
		for ( ; i < str.length(); i++)
		{
			p->insertNode(str[i]);
			p = p->getNode().back();
		}

		p->setEnd();
	}


	bool PrefixTree::search(const string& str) {
		if (str == "") return false;
		int i = 0;

		shared_ptr<Node> p = root;
		while (p->contain(str[i]))
		{
			p = p->getIndexAtChar(str[i]);
			i++;
		}
		if (i == str.length() && p->isEnd()) return true;
		else return false;
	}

	bool PrefixTree::startWith(const string& str) {
		if (str == "") return false;
		int i = 0;

		shared_ptr<Node> p = root;
		while (p->contain(str[i]))
		{
			p = p->getIndexAtChar(str[i]);
			i++;
		}
		if (i == str.length()) return true;
		else return false;
	}

private:
	shared_ptr<Node> root; //根节点
};
