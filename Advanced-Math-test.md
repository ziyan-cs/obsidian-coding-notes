## 第四章 多元函数微积分 · 单元检测

  T1 (§4.1) 判断极限是否存在：
  $$\lim_{(x,y)\to(0,0)} \frac{xy}{x^2 + y^2}$$

① R: 令 y=kx，这里的 lim(x,kx)->(0,0) 结果为 k/(1+$k^2$) 不是定值，所以不存在。

  T2 (§4.2) 求 $z = x^3y + e^{xy}$ 的所有二阶偏导数 $z_{xx}$, $z_{xy}$, $z_{yy}$。

② R: d$z_{xx}$ = 6xy+e^(xy)·$y^2$+, d$z_{xy}$ = 3x^2+e^(xy)·(xy+1), d$z_{yy}$ = e^(xy)·x^2

  T3 (§4.3) 求 $z = \ln(x^2 + y^2)$ 在点 $(1,1)$ 处的全微分 $dz$。

③ R: d$z$ = 2x/(x^2+y^2)d$x$ + 2y/(x^2+y^2)d$y$ = dx + dy

  T4 (§4.4) 设 $z = f(u, v)$，其中 $u = x^2 - y^2$，$v = e^{xy}$，$f$ 可微。用 $f_1', f_2'$ 表示 $\dfrac{\partial z}{\partial x}$。

④ R: $\dfrac{\partial z}{\partial x}$ =  $f_1'$·(2x) + $f_2'$·(e^(xy)·y)

  T5 (§4.5) 由方程 $x^2 + y^2 + z^2 - 3xyz = 0$ 确定 $z = z(x,y)$，求$\dfrac{\partial z}{\partial x}$ 和 $\dfrac{\partial z}{\partial y}$。

⑤ R:  $\dfrac{\partial z}{\partial x}$ = (3xz-2x)/(2z-3xy) ； $\dfrac{\partial z}{\partial y}$ = (3xz -2y)/(2z-3xy)

  T6 (§4.6 无条件极值) 求 $f(x,y) = x^3 + y^3 - 3x - 3y$ 的极值点与极值。

⑥ R: 极值点是(1, 1)，是极小值 -4，极值点（-1，-1），是极大值 4

  T7 (§4.6 条件极值) 用拉格朗日乘数法求：平面上 $x+y+z=1$ 到原点最近的点坐标。

⑦ R: 点坐标 (1/3, 1/3, 1/3)

  T8 (§4.7) 设 $f(x,y) = x^2 - y^2$，求：
  - (1) 在点 $(1,1)$ 处沿从 $(1,1)$ 到 $(3,2)$ 方向的方向导数；
  - (2) 在点 $(1,1)$ 处的梯度。

⑧ R: (1): 2/$\sqrt(5)$  (2): (2, -2)

---

## 第五章 重积分 · 单元检测

  T1 计算：$\iint_D xy dxdy$，其中 $D={(x,y)\mid0\le x \le 2,\ 0 \le y\le 1}$。

① R: 1

  T2 交换积分次序：$\int_0^1 dy \int_y^{\sqrt{y}} f(x,y) dx$

② R: $\int_0^1 dx \int_{x^2}^{x} f(x,y) dy$

  T3 计算：$\iint_D e^{-x^2-y^2} dxdy$，$D$ 为圆域 $x^2 + y^2 \le 1$。

③ R: (1-1/e)·${\pi}$

  T4 计算：$\iiint_{\Omega} z dxdydz$，$\Omega$ 为 $x^2 + y^2 + z^2 \le 1,z \ge 0$。

④ R: (1/4)·$\pi$

  T5 用柱坐标计算：$\iiint_{\Omega} (x^2+y^2) dv$，$\Omega$ 由 $z = 0$，$z= 4$，$x^2 + y^2 = 4$ 围成。

⑤ R: 32$\pi$

  T6 求抛物面 $z = x^2 + y^2$ 在平面 $z = 4$ 下方的表面积。
  
⑥ R:  $\pi$/6(17·$\sqrt(17)$-1)

---

##  第六章 曲线积分与曲面积分 · 单元检测

  T1 计算第一类曲线积分：$\int_L (x+y) ds$，$L$ 为连接 $(0,0)$ 到 $(1,1)$的直线段。

① R: $\sqrt(2)$

  T2 计算第二类曲线积分：$\int_L 2xy dx + x^2 dy$，$L$ 沿 $y=x^2$ 从$(0,0)$ 到 $(1,1)$。

② R: 1

  T3 用格林公式计算：$\oint_L (x^2y) dx - (xy^2) dy$，$L$ 为圆 $x^2 + y^2= 1$，取正向。

③ R:  -$\pi$/2

  T4 计算第一类曲面积分：$\iint_{\Sigma} (x+y+z) dS$，$\Sigma$ 为平面 $x+y+z=1$ 在第一卦限的部分。

④ R: $\sqrt(3)$/2

  T5 计算第二类曲面积分：$\iint_{\Sigma} z dxdy$，$\Sigma$ 为上半球面 $z =\sqrt{1-x^2-y^2}$，取上侧。

⑤ R:  2$\pi$/3

  T6 用高斯公式计算：$\iint_{\Sigma} x^3 dydz + y^3 dzdx + z^3dxdy$，$\Sigma$ 为球面 $x^2+y^2+z^2=1$，取外侧。

⑥ R: 12/5$\pi$

  T7 用斯托克斯公式计算：$\oint_L y dx + z dy + x dz$，$L$ 为平面 $x+y+z=0$ 与球面 $x^2+y^2+z^2=1$ 的交线，从 $z$ 轴正向看去取逆时针方向。
  
⑦ R: -$\sqrt(3)$·$\pi$

  T8补-A（路径无关性）验证 $Pdx + Qdy = (2xy+1)dx + (x^2)dy$ 在 $\mathbb{R}^2$ 上与路径无关，并求从 $(0,0)$ 到 $(1,2)$ 的积分值。

⑧-A R: 3

  T8-B（散度与旋度）设 $\vec{F} = (xy^2, , yz^2, , zx^2)$，求在点 $(1,1,1)$ 处的散度 $\text{div}\vec{F}$ 和旋度 $\nabla \times \vec{F}$。

⑧-B R: 3 ; (-2, -2, -2)

---

## 第七章 无穷级数 · 单元检测

  T1 判断收敛性：$\displaystyle\sum_{n=1}^{\infty} \frac{1}{n^2+1}$

① R: 收敛

  T2 判断收敛性：$\displaystyle\sum_{n=1}^{\infty} \frac{2^n}{n!}$

② R: 发散

  T3 判断收敛性：$\displaystyle\sum_{n=2}^{\infty} \frac{(-1)^{n-1}}{\ln n}$

③ R: 收敛

  T4 判断级数 $\displaystyle\sum_{n=1}^{\infty} (-1)^n\frac{n}{n^2+1}$是绝对收敛、条件收敛还是发散？

④ R: 条件收敛

  T5 求幂级数 $\displaystyle\sum_{n=1}^{\infty} \frac{x^n}{n \cdot 3^n}$的收敛域。

⑤ R: 

  T6 将 $f(x) = \dfrac{1}{1+x}$ 展开为 $(x-1)$ 的幂级数。

⑥ R: 

  T7 将 $f(x) = \begin{cases} -1, & -\pi < x < 0 \ 1, & 0 \le x \le \pi\end{cases}$ 展开为傅里叶级数（周期 $2\pi$）。

⑦ R: 

  T8 判断收敛性：$\displaystyle\sum_{n=1}^{\infty}\left(\frac{n}{2n+1}\right)^n$

⑧ R: 

---

## 第八章 常微分方程 · 单元检测

  T1 求解：$y' + 2xy = x$



  T2 求方程 $(x^2+1)y' + 2xy = 0$ 的通解。



  T3 求解：$y'' - 4y' + 4y = 0$



  T4 求 $y'' + y' - 6y = 0$ 满足 $y(0)=1,\ y'(0)=0$ 的特解。



  T5 用待定系数法求 $y'' - 3y' + 2y = e^x$ 的通解。



  T6 求解：$y'' + y = \sin x$



  T7 求 $(xy')' = 0$ 的通解。



  T8 求 $y'' = x + \sin x$ 的通解。