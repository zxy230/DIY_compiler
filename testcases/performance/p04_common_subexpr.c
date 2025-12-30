int compute(int x, int y) {
    int a = x - y;
    int b = x - y;
    int z = a + b + (x - y) * (x - y);
    return x - y - z;
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
