int compute(int x, int y) {
    int a = EVAL("randint(-32768, 32767)");
    int b = EVAL("randint(-32768, 32767)");
    int c = EVAL("randint(-32768, 32767)");

    int unused1 = a + b;
    int unused2 = b * c;
    int unused3 = a - c;

    if (0) {
        a = a + 1;
        b = b + 2;
        c = c + 3;
    }

    int i = 0;
    while (i < 10) {
        int temp = i * i;
        i = i + 1;
    }

    return a + b + c;
}

int main() {
    int N = 20000000;
    int MOD = 998244353;
    EXEC("MOD = 998244353");

    int i = 0;
    int result = 0;
    while (i < N) {
        result = ((result + compute((i + EVAL("randint(0, MOD)")) % 32768, (i + EVAL("randint(0, MOD)")) % 32768) % MOD) % MOD - i % MOD) % MOD;
        i = i + 1;
    }
    return result;
}
