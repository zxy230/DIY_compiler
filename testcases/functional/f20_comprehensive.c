// 斐波那契数列计算（递归实现）
int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

// 最大公约数（辗转相除法）
int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

// 判断素数
int isPrime(int num) {
    if (num <= 1) {
        return 0;
    }

    if (num <= 3) {
        return 1;
    }

    if (num % 2 == 0 || num % 3 == 0) {
        return 0;
    }

    int i = 5;
    while (i * i <= num) {
        if (num % i == 0 || num % (i + 2) == 0) {
            return 0;
        }
        i = i + 6;
    }

    return 1;
}

// 阶乘计算
int factorial(int n) {
    int result = 1;
    while (n > 0) {
        result = result * n;
        n = n - 1;
    }
    return result;
}

// 组合数计算 C(n,k) = n! / (k! * (n-k)!)
int combination(int n, int k) {
    if (k < 0 || k > n) {
        return 0;
    }
    if (k == 0 || k == n) {
        return 1;
    }
    if (k > n / 2) {
        k = n - k;
    }
    int result = 1;
    int i = 1;
    while (i <= k) {
        result = result * (n - i + 1) / i;
        i = i + 1;
    }
    return result;
}

// 幂运算（循环实现）
int power(int base, int exp) {
    int result = 1;
    int i = 1;
    while (i <= exp) {
        result = result * base;
        i = i + 1;
    }
    return result;
}

// 综合测试函数
int complexFunction(int a, int b, int c) {
    int result = 0;
    int i = 1;
    while (i <= a) {
        if (i % 2 == 0) {
            result = result + i * b;
        } else {
            result = result + i * c;
        }
        i = i + 1;
    }
    return result;
}

// 短路求值测试
int shortCircuit(int a, int b) {
    if (a < 0 && b > 0) {
        return a + b;
    }
    if (a > 0 || b < 0) {
        return a - b;
    }
    return a * b;
}

// 嵌套循环和条件测试
int nestedLoopsConds(int x) {
    int sum = 0;
    int i = 1;
    while (i <= 10) {
        int j = 1;
        while (j <= 10) {
            if (i + j > x) {
                sum = sum + i + j;
            } else {
                sum = sum + i * j;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    return sum;
}

// 多层嵌套函数调用
int nestedCalls(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    int sum = a + b;
    sum = sum + c;
    sum = sum + d;
    sum = sum + e;
    sum = sum + f;
    sum = sum + g;
    sum = sum + h;
    sum = sum + i;
    sum = sum + j;
    return sum;
}

int main() {
    int result = 0;
    int i = 1;
    while (i <= 100) {
        result = result + i;
        i = i + 1;
    }

    if (result > 1000) {
        result = result + 30;
    } else {
        result = result + 40;
    }

    int sum = 0;
    i = 1;
    while (i <= 100) {
        if (sum > 1000) {
            sum = sum + 1;
        } else {
            sum = sum + 2;
        }
        i = i + 1;
    }

    if (sum > 1000) {
        sum = sum + 1;
    } else {
        sum = sum + 2;
    }

    int i1 = 5;
    int i2 = 10;
    int i3 = 7;
    int i4 = 12;

    int fib = fibonacci(i1);
    int gcd_result = gcd(i2, i3);
    int prime_check = isPrime(i3);
    int fact = factorial(i2);
    int comb = combination(i3, i1);
    int pow_result = power(i1, 4);
    int complex_result = complexFunction(3, 4, 5);
    int short_circuit = shortCircuit(-5, 10);
    int nested_loops_conds_result = nestedLoopsConds(15);
    int nested_calls_result = nestedCalls(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    result = 1 + (fib * 1 % 37633 + gcd_result * 1 % 37633 - prime_check * 1 % 37633 + fact * 1 % 37633 - comb * 1 % 37633 + pow_result * 1 % 37633 - nested_loops_conds_result * 1) % 254;

    return result;
}
