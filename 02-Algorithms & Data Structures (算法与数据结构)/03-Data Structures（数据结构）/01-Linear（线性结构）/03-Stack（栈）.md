

# 0. 核心

- **后进先出（LIFO）的受限线性结构**
- 相关特征：
    - 只能访问栈顶元素
    - 入栈、出栈、取栈顶时间复杂度均为 $O(1)$
    - 用栈维护「顺序 / 优先级 / 上下文」，解决需要逆序、嵌套的问题
- 优势：
    - 天然适配「后进先出」的业务逻辑（如表达式求值、括号匹配）
    - 内存开销可控，无额外空间浪费

- 劣势：
    - 不支持随机访问，无法遍历中间元素
    - 栈满 / 栈空需要额外判断，存在溢出风险

# 1. 基础语法与使用

- STL 核心操作
	- `push(x)`：将元素 `x` 压栈
	- `pop()`：弹出栈顶元素（不返回值）
	- `top()`：获取栈顶元素的引用
	- `empty()`：判断栈为空返回 `true`，非空返回 `false`
	- `size()`：返回栈中元素个数

- 数组手动实现栈

```cpp
const int MAX = 100010;
int st[MAX], idx = -1; // idx 指向栈顶元素下标，-1 表示空栈

void push(int x) { st[++idx] = x; }
void pop() { --idx; }
int top() { return st[idx]; }
bool empty() { return idx == -1; }
int size() { return idx + 1; }
```

# 2. 经典应用场景

### 2.1 括号匹配问题

- 思路：左括号入栈，遇到右括号时匹配栈顶左括号，最终栈空则合法

```cpp
bool isValid(string s) {
    stack<char> st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') st.push(c);
        else {
            if (st.empty()) return false;
            char top = st.top(); st.pop();
            if (c == ')' && top != '(') return false;
            if (c == ']' && top != '[') return false;
            if (c == '}' && top != '{') return false;
        }
    }
    return st.empty();
}
```

### 2.2 中缀表达式 → 后缀表达式（即逆波兰表达式）

- 操作数：直接输出

- 高优先级运算符 ：入栈
- 相同优先级运算符：弹出栈顶，不入栈
- 低优先级运算符：弹出栈顶并输出，入栈

- 运算符优先级优先级
	- 入栈 `(` 和 `)` 最高
	- 入栈 `)` = 栈内 `(`
	- 入栈 `* /` > 栈内 `* /`
	- 入栈 `* /` > 栈内 `+ -`
	- 入栈 `+ -` > 栈内 `+ -`
	- 栈内 `(` 最低
	- 入栈 `#` 最低
	- 入栈 `#` = 栈内 `#`
 
```cpp
string infixToPostfix(string s) {
    stack<char> op;
    string res;
    unordered_map<char, int> prio = {{'+',1},{'-',1},{'*',2},{'/',2}};
    for (char c : s) {
        // 操作数直接加入结果
        if (isdigit(c)) {
            res += c;
        }
        // 左括号直接入栈
        else if (c == '(') {
            op.push(c);
        }
        // 右括号：弹出直到左括号
        else if (c == ')') {
            while (!op.empty() && op.top() != '(') {
                res += op.top();
                op.pop();
            }
            op.pop(); // 弹出左括号
        }
        // 运算符：按优先级弹出
        else {
            while (!op.empty() && op.top() != '(' && prio[op.top()] >= prio[c]) {
                res += op.top();
                op.pop();
            }
            op.push(c);
        }
    }
    // 剩余运算符全部弹出
    while (!op.empty()) {
        res += op.top();
        op.pop();
    }
    return res;
}
```

### 2.3 后缀表达式求值

- 暂存操作数，遇到运算符弹出 **两个** 操作数，计算结果后回栈

```cpp
int evalRPN(vector<string>& tokens) {
    stack<int> st;
    for (string& t : tokens) {
        if (t == "+" || t == "-" || t == "*" || t == "/") {
            int b = st.top(); st.pop();
            int a = st.top(); st.pop();
            if (t == "+") st.push(a + b);
            else if (t == "-") st.push(a - b);
            else if (t == "*") st.push(a * b);
            else st.push(a / b);
        } else {
            st.push(stoi(t));
        }
    }
    return st.top();
}
```

### 2.4 单调栈（进阶高频）

- 核心：存数据 + 强制维护栈内单调性（只访问栈顶最值）
- 典型场景
	- Next Greater/Smaller Element
	- 柱状图最大矩形、接雨水、每日温度