
```cpp
double myPow(double x, int n) {
	long long N = n;
	if (n < 0) N = -N;
	double ans = 1;
	while (N > 0) {
		if (N & 1) {
			ans *= x;
		}
		N >>= 1;
		x *= x;
	}
	if (n < 0) ans = 1 / ans;
	return ans;
}
```
**（LeetCode 50）**
