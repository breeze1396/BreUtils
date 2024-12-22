[big package - math/big - Go Packages](https://pkg.go.dev/math/big#example-package-Fibonacci)



### Float

- **func Float(x float64) *Float** (95): 创建一个新的 *Float 并初始化为 x。
- **func ParseFloat(s string, base int, prec uint, mode RoundingMode) (f *Float, b int, err error)** (75): 解析字符串为 *Float 类型。
- **func (z *Float) Abs(x *Float) *Float** (80): 返回 x 的绝对值。
- **func (z *Float) Add(x, y *Float) *Float** (90): 将 x 和 y 相加，并将结果存储在 z 中。
- **func (z *Float) Cmp(y *Float) int** (88): 比较两个 *Float 值。
- **func (z *Float) Copy(x *Float) *Float** (70): 复制 *Float 值。
- **func (z *Float) Mul(x, y *Float) *Float** (92): 乘以两个 *Float 值。
- **func (z *Float) Quo(x, y *Float) *Float** (87): 除以两个 *Float 值。
- **func (z *Float) Set(x *Float) *Float** (85): 设置 *Float 值。
- **func (z *Float) SetFloat64(x float64) *Float** (83): 设置 *Float 为给定的 float64。
- **func (z *Float) SetPrec(prec uint) *Float** (78): 设置 *Float 的精度。
- **func (z *Float) Sqrt(x *Float) *Float** (82): 计算 *Float 的平方根。
- **func (z *Float) Sub(x, y *Float) *Float** (89): 从 x 减去 y。

### Int
- **func Int(x int64) *Int** (95): 创建一个新的 *Int 并初始化为 x。
- **func (z *Int) Add(x, y *Int) *Int** (93): 将 x 和 y 相加，并将结果存储在 z 中。
- **func (z *Int) Cmp(y *Int) (r int)** (90): 比较两个 *Int 值。
- **func (z *Int) Div(x, y *Int) *Int** (88): 除以两个 *Int 值。
- **func (z *Int) Exp(x, y, m *Int) *Int** (85): 计算 x 的 y 次幂模 m。
- **func (z *Int) GCD(x, y, a, b *Int) *Int** (75): 计算最大公约数。
- **func (z *Int) Mod(x, y *Int) *Int** (87): 取模运算。
- **func (z *Int) Mul(x, y *Int) *Int** (92): 乘以两个 *Int 值。
- **func (z *Int) Neg(x *Int) *Int** (80): 返回 x 的负值。
- **func (z *Int) ProbablePrime(n int) bool** (78): 测试 *Int 是否可能是素数。
- **func (z *Int) Set(x *Int) *Int** (85): 设置 *Int 值。
- **func (z *Int) SetInt64(x int64) *Int** (83): 设置 *Int 为给定的 int64。
- **func (z *Int) Sqrt(x *Int) *Int** (82): 计算 *Int 的平方根。
- **func (z *Int) Sub(x, y *Int) *Int** (90): 从 x 减去 y。

### Rat

- **func Rat(a, b int64) *Rat** (90): 创建一个新的 *Rat 并初始化为 a/b。
- **func (z *Rat) Add(x, y *Rat) *Rat** (88): 将 x 和 y 相加，并将结果存储在 z 中。
- **func (z *Rat) Cmp(y *Rat) int** (85): 比较两个 *Rat 值。
- **func (z *Rat) Mul(x, y *Rat) *Rat** (87): 乘以两个 *Rat 值。
- **func (z *Rat) Quo(x, y *Rat) *Rat** (86): 除以两个 *Rat 值。
- **func (z *Rat) Set(x *Rat) *Rat** (85): 设置 *Rat 值。
- **func (z *Rat) SetFrac(a, b *Int) *Rat** (83): 设置 *Rat 为分数 a/b。
- **func (z *Rat) Sub(x, y *Rat) *Rat** (88): 从 x 减去 y。

