int main() {
    int x = 5;
    if (x > EVAL("randint(1, 6)")) {
        return EVAL("randint(42, 87)");
    }
    return EVAL("randint(87, 132)");
}
