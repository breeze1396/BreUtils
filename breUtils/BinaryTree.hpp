#ifndef _BINARY_TREE_
#define _BINARY_TREE_

#include <string>
#include <memory>

using std::string;

template <typename T>
class Node {
public:
	Node() : data_{}, left_(nullptr), right_(nullptr) {}
	Node(T data) : data_(data), left_(nullptr), right_(nullptr) {}
	Node(T data, Node* left, Node* right)
		: data_(data), left_(left), right_(right) {}
	T Data;
	Node* left;
	Node* right;
};


template <typename T>
class BinaryTree
{
public:
    BinaryTree() : root_(nullptr) {}
    BinaryTree(std::string treeStr);

    std::shared_ptr<Node> getRoot() { return root_; }
    void printTree() const {
        printTreePreOrder();
        printTreeInOrder();
        printTreePostOrder();
        printTreeLevelOrder();
    }
    void printTreePreOrder() const {
    }
    void printTreeInOrder() const;
    void printTreePostOrder() const;
    void printTreeLevelOrder() const;

private:
    Node* createTreeFromStr(const std::string& str, size_t& index);
    void printTreePreOrder(Node* node) const;
    void printTreeInOrder(Node* node) const;
    void printTreePostOrder(Node* node) const;

private:
    std::shared_ptr<Node> root_;
};


// 实现字符串解析构造函数
 BinaryTree::BinaryTree(std::string treeStr)
{
    size_t index = 0;
    root_ = createTreeFromStr(treeStr, index);
}


 void BinaryTree::setTree(std::string tree_str)
{
}





 void BinaryTree::printTreeInOrder() const
{
}


 void BinaryTree::printTreePreOrder() const
{
     printTreePreOrder(this->root_);
}


 void BinaryTree::printTreePostOrder() const
{
}


 void BinaryTree::printTreeLevelOrder() const
{
}


 Node* BinaryTree::createTreeFromStr(const std::string& str, size_t& index)
{
    if (index >= str.size())
        return nullptr;

    // 分割下一个整数值
    size_t nextValueStart = index;
    while (nextValueStart < str.size() && !isspace(str[nextValueStart]))
        ++nextValueStart;

    int value;
    try {
        value = std::stod(str.substr(index, nextValueStart - index)); // 根据实际类型选择 stoul, stod, stoll 等
    }
    catch (...) {
        throw std::invalid_argument("Invalid string format for creating the binary tree.");
    }

    auto currentNode = new Node(value);

    index = nextValueStart + 1; // 跳过分隔符

    // 递归构建子树
    currentNode->left_ = createTreeFromStr(str, index);
    currentNode->right_ = createTreeFromStr(str, index);

    return currentNode;
}

 void BinaryTree::printTreePreOrder(Node* node) const
{
     if (!node) return;
     std::cout << node->data_ << ": " << std::endl;
     printTreePreOrder(node->left_);
     printTreePreOrder(node->right_);
}
  
 void BinaryTree::printTreeInOrder(Node* node) const
{
}

 void BinaryTree::printTreePostOrder(Node* node) const
{
}

#endif // !_BINARY_TREE_
