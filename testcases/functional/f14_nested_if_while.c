int main() {
    int x = 0;
    while (x < EVAL("randint(5, 15)")) {
        if (x % 2 == 0) {
            x = x + EVAL("randint(2, 4)");
        } else {
            x = x EVAL("'+' if randint(0, 1) else '-'") 1;
        }
    }
    return x;
}
