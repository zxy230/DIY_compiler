int compute(int x, int y) {
    int a = EVAL("randint(0, 32767)");
    int b = EVAL("randint(0, 32767)");
    int c = -EVAL("randint(0, 32767)");
    int p = a + EVAL("randint(0, 32767)");
    int q = b * EVAL("randint(0, 32767)");
    int r = c - EVAL("randint(0, 32767)");
    int result = p * q / (r - 1);
    return result;
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
