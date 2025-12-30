int main() {
    int x = 3;
    if (x > EVAL("randint(2, 4)")) {
        x = x + 1;
    } else {
        x = x - 1;
    }
    return x;
}
