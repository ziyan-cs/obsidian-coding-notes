// Template: KMP — Pattern matching (Knuth-Morris-Pratt)
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
using namespace std;

// 构建 next 数组（最长相等前后缀）
static vector<int> buildNext(const string& p) {
    int m = (int)p.size();
    vector<int> next(m, 0);
    for (int i = 1, j = 0; i < m; i++) {
        while (j > 0 && p[i] != p[j]) j = next[j - 1];
        if (p[i] == p[j]) j++;
        next[i] = j;
    }
    return next;
}

// KMP 搜索，返回第一个匹配位置，未找到返回 -1
static int kmpSearch(const string& text, const string& pattern) {
    if (pattern.empty()) return 0;
    int n = (int)text.size(), m = (int)pattern.size();
    auto next = buildNext(pattern);
    for (int i = 0, j = 0; i < n; i++) {
        while (j > 0 && text[i] != pattern[j]) j = next[j - 1];
        if (text[i] == pattern[j]) j++;
        if (j == m) return i - m + 1;
    }
    return -1;
}

int main() {
    // Basic match
    string text = "ababcabcabababd";
    string pattern = "ababd";
    assert(kmpSearch(text, pattern) == 10);

    // Pattern not found
    assert(kmpSearch(text, "xyz") == -1);

    // Empty pattern
    assert(kmpSearch(text, "") == 0);

    // At beginning
    assert(kmpSearch("hello world", "hello") == 0);

    // At end
    assert(kmpSearch("hello world", "world") == 6);

    // Single character
    assert(kmpSearch("aaaaa", "a") == 0);
    assert(kmpSearch("bbba", "a") == 3);

    // Repeated pattern
    assert(kmpSearch("aaaaa", "aa") == 0);

    // Full match
    assert(kmpSearch("abc", "abc") == 0);

    cout << "All tests passed!" << endl;
    return 0;
}
