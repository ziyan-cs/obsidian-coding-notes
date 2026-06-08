// Template: Linked List — Reverse, middle, merge
#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// 反转链表（迭代）
static ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    while (head) {
        ListNode* next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
}

// 找中点（快慢指针）
static ListNode* middleNode(ListNode* head) {
    auto slow = head, fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// 合并有序链表
static ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode dummy(0), *p = &dummy;
    while (l1 && l2) {
        if (l1->val < l2->val) { p->next = l1; l1 = l1->next; }
        else { p->next = l2; l2 = l2->next; }
        p = p->next;
    }
    p->next = l1 ? l1 : l2;
    return dummy.next;
}

// --- Helpers ---
static ListNode* buildList(const vector<int>& vals) {
    if (vals.empty()) return nullptr;
    ListNode dummy(0), *p = &dummy;
    for (int v : vals) {
        p->next = new ListNode(v);
        p = p->next;
    }
    return dummy.next;
}

static void freeList(ListNode* head) {
    while (head) {
        auto next = head->next;
        delete head;
        head = next;
    }
}

static vector<int> listToVec(ListNode* head) {
    vector<int> res;
    while (head) {
        res.push_back(head->val);
        head = head->next;
    }
    return res;
}

int main() {
    // Test reverse
    auto list1 = buildList({1, 2, 3, 4, 5});
    auto rev = reverseList(list1);
    auto revVec = listToVec(rev);
    assert(revVec.size() == 5);
    for (int i = 0; i < 5; i++) assert(revVec[i] == 5 - i);
    freeList(rev);

    // Test middle (odd length)
    auto list2 = buildList({1, 2, 3, 4, 5});
    assert(middleNode(list2)->val == 3);
    freeList(list2);

    // Test middle (even length — second middle)
    auto list2b = buildList({1, 2, 3, 4});
    assert(middleNode(list2b)->val == 3);
    freeList(list2b);

    // Test merge
    auto l1 = buildList({1, 3, 5});
    auto l2 = buildList({2, 4, 6});
    auto merged = mergeTwoLists(l1, l2);
    auto mergedVec = listToVec(merged);
    assert(mergedVec.size() == 6);
    for (int i = 0; i < 6; i++) assert(mergedVec[i] == i + 1);
    // merged takes ownership of l1/l2 nodes; free only once
    freeList(merged);

    cout << "All tests passed!" << endl;
    return 0;
}
