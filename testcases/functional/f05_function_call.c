int add(int a, int b) {
    return a + b;
}

int main() {
    int x = add(EVAL("randint(0, 127)"), EVAL("randint(0, 127)"));
    return x;
}
