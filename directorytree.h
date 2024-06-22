#ifndef DIRECTORY_TREE_H
#define DIRECTORY_TREE_H

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

//class TreeNode {
//public:
//    std::string value;
//    std::vector<std::shared_ptr<TreeNode>> children;
//
//    TreeNode(const std::string& val);
//};

struct TreeNode {
    std::string value;
    std::vector<TreeNode* > children;
};



class DirectoryTree {
public:
    DirectoryTree();
    ~DirectoryTree();

    TreeNode* getRoot() const;

    void addChild(const std::string& parentValue, const std::string& childValue);
    TreeNode*  findNode(TreeNode* node, const std::string& value);
    bool remveNode(TreeNode* node, const std::string& value);
    void printTraversal(TreeNode* node ,int path = 0);
    void fromJson(const json& j, TreeNode* node);
    json toJson(TreeNode* node);

private:
    TreeNode* root;
};


//class DirectoryTree {
//private:
//    std::shared_ptr<TreeNode> root;
//    std::map<std::string, std::shared_ptr<TreeNode>> nodes;
//
//    void deleteSubTree(const std::shared_ptr<TreeNode>& node);
//    void fromJson(const json& j, const std::shared_ptr<TreeNode>& node);
//
//public:
//    DirectoryTree();
//
//    void addChild(const std::string& parentValue, const std::string& childValue);
//    void deleteNode(const std::string& nodeValue);
//    void clearTree();
//    void dfsTraversal(const std::shared_ptr<TreeNode>& node) const;
//    void printDFSTraversal() const;
//    json toJson(const std::shared_ptr<TreeNode>& node) const;
//    json getTreeJson() const;
//    void buildTreeFromJson(const json& j);
//};

#endif // DIRECTORY_TREE_H


