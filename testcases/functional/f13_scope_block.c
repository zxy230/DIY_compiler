int main() {
    int x = EVAL("randint(0, 1023)");
    int y = EVAL("randint(0, 10)");
    {
        int y = EVAL("randint(11, 1023)");
        x = x + y;
    }
    return y;
}
