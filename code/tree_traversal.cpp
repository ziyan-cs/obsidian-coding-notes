// Template: Tree Traversal — Preorder, Inorder, Level Order
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <cassert>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left, *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// 前序（迭代）
static vector<int> preorder(TreeNode* root) {
    if (!root) return {};
    vector<int> res;
    stack<TreeNode*> st;
    st.push(root);
    while (!st.empty()) {
        auto n = st.top(); st.pop();
        res.push_back(n->val);
        if (n->right) st.push(n->right);
        if (n->left) st.push(n->left);
    }
    return res;
}

// 中序（迭代）
static vector<int> inorder(TreeNode* root) {
    vector<int> res;
    stack<TreeNode*> st;
    auto cur = root;
    while (cur || !st.empty()) {
        while (cur) { st.push(cur); cur = cur->left; }
        cur = st.top(); st.pop();
        res.push_back(cur->val);
        cur = cur->right;
    }
    return res;
}

// 层序（BFS）
static vector<vector<int>> levelOrder(TreeNode* root) {
    if (!root) return {};
    vector<vector<int>> res;
    queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int sz = (int)q.size();
        vector<int> level;
        while (sz--) {
            auto n = q.front(); q.pop();
            level.push_back(n->val);
            if (n->left) q.push(n->left);
            if (n->right) q.push(n->right);
        }
        res.push_back(level);
    }
    return res;
}

// --- Helpers ---
// Builds a binary tree from a preorder traversal with -1 representing null.
// The caller passes vals by pointer so the recursive calls share an index.
static TreeNode* buildTreeHelper(const vector<int>& vals, int& idx) {
    if (idx >= (int)vals.size() || vals[idx] == -1) {
        idx++;
        return nullptr;
    }
    auto root = new TreeNode(vals[idx++]);
    root->left  = buildTreeHelper(vals, idx);
    root->right = buildTreeHelper(vals, idx);
    return root;
}

static TreeNode* buildTree(const vector<int>& vals) {
    int idx = 0;
    return buildTreeHelper(vals, idx);
}

static void freeTree(TreeNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

int main() {
    // Build tree:
    //     1
    //    / \\
    //   2   3
    //  / \\   \\
    // 4   5   6
    vector<int> vals = {1, 2, 4, -1, -1, 5, -1, -1, 3, -1, 6, -1, -1};
    auto root = buildTree(vals);

    // Preorder: 1, 2, 4, 5, 3, 6
    auto pre = preorder(root);
    vector<int> expPre = {1, 2, 4, 5, 3, 6};
    assert(pre == expPre);

    // Inorder: 4, 2, 5, 1, 3, 6
    auto in = inorder(root);
    vector<int> expIn = {4, 2, 5, 1, 3, 6};
    assert(in == expIn);

    // Level order: [[1], [2,3], [4,5,6]]
    auto level = levelOrder(root);
    assert(level.size() == 3);
    assert(level[0] == vector<int>({1}));
    assert(level[1] == vector<int>({2, 3}));
    assert(level[2] == vector<int>({4, 5, 6}));

    freeTree(root);
    cout << "All tests passed!" << endl;
    return 0;
}
