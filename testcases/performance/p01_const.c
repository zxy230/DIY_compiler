int compute(int x, int y) {
    int a = 100;
    int b = 200;
    int c = -300;
    int p = a + 150;
    int q = b * 250;
    int r = c - 350;
    int result = p * q / (r - 1);
    return result;
}

int main() {
    int N = 1000;
    int MOD = 998244353;

    int i = 0;
    int result = 0;
    while (i < N) {
        result = ((result + compute((i + 100) % 32768, (i + 200) % 32768) % MOD) % MOD - i % MOD) % MOD;
        i = i + 1;
    }
    return result;
}
