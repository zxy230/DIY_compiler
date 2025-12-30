int main() {
    int x = 0;
    while (x < 10) {
        if (x == EVAL("randint(0, 10)")) {
            break;
        }
        x = x + 1;
    }
    return x;
}
