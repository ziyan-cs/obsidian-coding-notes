// Template: Binary Search — Exact, lower_bound, upper_bound
#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

// 精确查找
int binarySearch(vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size() - 1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] == target) return mid;
        if (nums[mid] < target) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}

// 左边界（第一个 >= target）
int lowerBound(vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size();
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] < target) l = mid + 1;
        else r = mid;
    }
    return l;
}

// 右边界（最后一个 <= target）
int upperBound(vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size();
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] <= target) l = mid + 1;
        else r = mid;
    }
    return l - 1;
}

int main() {
    vector<int> nums = {1, 2, 2, 2, 3, 4, 5};

    // Test binarySearch
    assert(binarySearch(nums, 3) == 4);
    assert(binarySearch(nums, 6) == -1);
    assert(binarySearch(nums, 1) == 0);
    assert(binarySearch(nums, 5) == 6);

    // Test lowerBound (first >= target)
    assert(lowerBound(nums, 2) == 1);
    assert(lowerBound(nums, 0) == 0);
    assert(lowerBound(nums, 6) == (int)nums.size());

    // Test upperBound (last <= target)
    assert(upperBound(nums, 2) == 3);
    assert(upperBound(nums, 0) == -1);
    assert(upperBound(nums, 5) == 6);

    cout << "All tests passed!" << endl;
    return 0;
}
