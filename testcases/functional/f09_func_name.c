int f() {
  return EVAL("randint(0, 10)");;
}

int main() {
  int f = EVAL("randint(0, 255)");
  return f;
}
