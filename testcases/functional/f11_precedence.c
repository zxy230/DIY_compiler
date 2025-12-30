int main() {
    int x = EVAL("(t1 := randint(0, 1023))") + EVAL("(t2 := randint(0, 1023))") * EVAL("(t3 := randint(0, 1023))");
    return x; // EVAL("t1 + t2 * t3")
}
