int compute(int x, int y) {
    int a = x * 1;
    int b = a + 0;
    int c = b * 0;

    int p = y / 1;
    int q = x * 16;
    int r = y / 8;
    int s = x - 0;
    int t = y / 32;

    int u = -b + (a + b);
    int v = p - q - r + r;
    int w = s - (b - b) EVAL("'+' if randint(0, 1) else '-'") t;

    return x EVAL("'+' if randint(0, 1) else '-'") y EVAL("'+' if randint(0, 1) else '-'") a EVAL("'+' if randint(0, 1) else '-'") b EVAL("'+' if randint(0, 1) else '-'") c EVAL("'+' if randint(0, 1) else '-'") p EVAL("'+' if randint(0, 1) else '-'") q EVAL("'+' if randint(0, 1) else '-'") r;
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
