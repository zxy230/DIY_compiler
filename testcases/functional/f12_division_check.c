int main() {
    int a = EVAL("(a := 10)");
    int b = EVAL("(b := randint(1, 10))");
    return a / b; // EVAL("a / b")
}
