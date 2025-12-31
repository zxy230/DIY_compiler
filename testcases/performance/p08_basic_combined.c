int loopSideEffect(int n, int x) {
    int i = 0;
    int acc = 0;
    while (i < n) {
        acc = acc + i;
        i = i + 1;
    }
    return x;
}

int loop(int n, int x, int y) {
    int MOD = 998244353;

    int i = 0;
    int result = 0;
    while (i < n) {
        int t1 = x * 2 + 1;
        int t2 = y * 3 + 1;
        int t3 = t1 + t2;

        int t4 = 0;
        int j = 0;
        while (j < 10) {
            t4 = loopSideEffect(5, t4) + t3;
            j = j + 1;
        }
        int t5 = t1 + t2 + t3 + t4;

        int t6 = t4 + t5;

        result = ((result + t6 % MOD) % MOD - i % MOD) % MOD;
        i = i + 1;
    }
    return result;
}

int main() {
    int N = 1000;
    int MOD = 998244353;

    int i = 0;
    int result = 0;
    while (i < N) {
        result = (result + loop(10, i, i + 1)) % MOD;
        i = i + 1;
    }
    return result;
}
