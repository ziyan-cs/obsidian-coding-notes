// Template: Sorting — Quick sort and Merge sort
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
using namespace std;

// 快速排序
static int partition(vector<int>& nums, int l, int r) {
    int p = nums[r], i = l;
    for (int j = l; j < r; j++)
        if (nums[j] <= p) swap(nums[i++], nums[j]);
    swap(nums[i], nums[r]);
    return i;
}

static void quickSort(vector<int>& nums, int l, int r) {
    if (l >= r) return;
    int pivot = partition(nums, l, r);
    quickSort(nums, l, pivot - 1);
    quickSort(nums, pivot + 1, r);
}

// 归并排序
static void mergeSort(vector<int>& nums, int l, int r, vector<int>& tmp) {
    if (l >= r) return;
    int mid = l + (r - l) / 2;
    mergeSort(nums, l, mid, tmp);
    mergeSort(nums, mid + 1, r, tmp);
    int i = l, j = mid + 1, k = l;
    while (i <= mid && j <= r)
        tmp[k++] = nums[i] <= nums[j] ? nums[i++] : nums[j++];
    while (i <= mid) tmp[k++] = nums[i++];
    while (j <= r) tmp[k++] = nums[j++];
    copy(tmp.begin() + l, tmp.begin() + r + 1, nums.begin() + l);
}

int main() {
    // Test quickSort
    vector<int> nums1 = {5, 3, 1, 4, 2};
    quickSort(nums1, 0, (int)nums1.size() - 1);
    assert(is_sorted(nums1.begin(), nums1.end()));

    // Test mergeSort
    vector<int> nums2 = {9, 7, 5, 3, 1, 8, 6, 4, 2};
    vector<int> tmp(nums2.size());
    mergeSort(nums2, 0, (int)nums2.size() - 1, tmp);
    assert(is_sorted(nums2.begin(), nums2.end()));

    // Edge cases
    vector<int> single = {1};
    quickSort(single, 0, 0);
    assert(is_sorted(single.begin(), single.end()));

    // Already sorted
    vector<int> sorted = {1, 2, 3, 4, 5};
    quickSort(sorted, 0, (int)sorted.size() - 1);
    assert(is_sorted(sorted.begin(), sorted.end()));

    // Reverse sorted
    vector<int> rev = {5, 4, 3, 2, 1};
    quickSort(rev, 0, (int)rev.size() - 1);
    assert(is_sorted(rev.begin(), rev.end()));

    cout << "All tests passed!" << endl;
    return 0;
}
