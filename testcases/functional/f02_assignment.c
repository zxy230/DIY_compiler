int main() {
    int x = EVAL("randint(0, 127)");
    x = x + EVAL("randint(0, 127)");
    return x;
}
