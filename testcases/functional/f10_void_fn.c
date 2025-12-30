void print() {
    int x = 1;
    x = x + 1;
    ;
}

int main() {
    print();
    return EVAL("randint(0, 1023)");
}
