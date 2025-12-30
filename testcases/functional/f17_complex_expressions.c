int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int fibonacci(int n) {
    if (n <= 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

int is_prime(int n) {
    if (n <= 1) {
        return 0;
    }
    if (n <= 3) {
        return 1;
    }
    if (n % 2 == 0 || n % 3 == 0) {
        return 0;
    }

    int i = 5;
    while (i * i <= n) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return 0;
        }
        i = i + 6;
    }
    return 1;
}

int main() {
    int a = EVAL("randint(-1024, 1023)");
    int b = EVAL("randint(-1024, 1023)");
    int c = EVAL("randint(-1024, 1023)");
    int d = EVAL("randint(-1024, 1023)");

    int expr1 = ((a EVAL("'+' if randint(0, 1) else '-'") b) * c EVAL("'+ ' if randint(0, 1) else '- '")(d * a)) / ((b EVAL("'+' if randint(0, 1) else '-'") c) % (a + -d + 2048) + 1) EVAL("'+ ' if randint(0, 1) else '- '")(-a * -b * +c * (d - +2 - -c));

    int x = EVAL("randint(-1024, 0)");
    int y = EVAL("randint(1, 1024)");
    int z = EVAL("randint(-1024, 1023)");

    int expr2 = 0;
    if ((x > y) && ((z + 1) == 1)) {
        expr2 = 1;
    }

    int expr3 = 0;
    if ((x < y) || ((z + 2) == 2)) {
        expr3 = 1;
    }

    int expr4 = 0;
    if (!((x > EVAL("randint(-1024, 1023)") && y < EVAL("randint(-1024, 1023)")) || (z > EVAL("randint(-1024, 1023)") && x < EVAL("randint(-1024, 1023)"))) && (y > EVAL("randint(-1024, 1023)") || x < EVAL("randint(-1024, 1023)"))) {
        expr4 = 1;
    }

    int n1 = EVAL("randint(1, 32767)");
    int n2 = EVAL("randint(1, 32767)");
    int n3 = EVAL("randint(1, 32767)");

    int expr5 = factorial(gcd(n2 % 12, n3 % 12)) + fibonacci(n1 % 10 EVAL("'+' if randint(0, 1) else '-'") 2);

    int max_val = 0;
    if (n1 > n2 && n1 > n3) {
        max_val = n1;
    } else if (n2 > n1 && n2 > n3) {
        max_val = n2;
    } else {
        max_val = n3;
    }

    int sum = 0;
    int i = 1;
    while (i <= EVAL("randint(10, 20)")) {
        if (i % EVAL("randint(2, 3)") == 0) {
            sum = sum + i * i;
        } else if (i % EVAL("randint(4, 5)") == 0) {
            sum = sum + i * i * i;
        } else {
            sum = sum + i;
        }
        i = i + 1;
    }

    int expr6 = 0;
    i = 1;
    while (i <= EVAL("randint(5, 10)")) {
        int j = 1;
        int term = 1;
        while (j <= i) {
            term = term * j;
            j = j + 1;
        }
        expr6 = expr6 + term;
        i = i + 1;
    }

    int expr7 = 0;
    if (is_prime(n1)) {
        if (is_prime(n2)) {
            expr7 = n1 * n2;
        } else if (is_prime(n3)) {
            expr7 = n1 * n3;
        } else {
            expr7 = n1;
        }
    } else if (is_prime(n2)) {
        if (is_prime(n3)) {
            expr7 = n2 * n3;
        } else {
            expr7 = n2;
        }
    } else if (is_prime(n3)) {
        expr7 = n3;
    } else {
        expr7 = n1 + n2 + n3;
    }

    int expr8 = 0;
    int num = EVAL("randint(1024, 16777216)");
    int bit_count = 0;

    while (num > 0) {
        if (num % 2 == 1) {
            bit_count = bit_count + 1;
        }
        num = num / 2;
    }

    int final_result = expr1 EVAL("'+' if randint(0, 1) else '-'") expr2 EVAL("'+' if randint(0, 1) else '-'") expr3 EVAL("'+' if randint(0, 1) else '-'") expr4 EVAL("'+' if randint(0, 1) else '-'") expr5 EVAL("'+' if randint(0, 1) else '-'") max_val EVAL("'+' if randint(0, 1) else '-'") sum EVAL("'+' if randint(0, 1) else '-'") expr6 EVAL("'+' if randint(0, 1) else '-'") expr7 EVAL("'+' if randint(0, 1) else '-'") bit_count;

    return final_result;
}
