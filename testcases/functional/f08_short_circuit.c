int main() {
    int a = 0;
    int b = 1;
    if (a && (b / a)) {
        return 0;
    } else if (b || (b / a)) {
        return EVAL("randint(2, 255)");
    } else {
        return 1;
    }
}
