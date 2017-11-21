
int f() {
  // test for normal loop
  int s, q, n;
  for (int i = 0; i < n; ++i) {
    if (i % 2 == 0) {
      s += i;
    } else {
      q += 2 * i;
    }
  }

  // test for nested if-else
  for (int i, j = 1, k = 0; i < 2; ++i) {
    if (i + 1 < 2) {
      i += 2;
    } else if (i > 3){
      j -= 3;
    } else {
      i += i;
    }
  }

  // test for nested loop
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      s++;
      for (int k = 0; k < 3; ++k);
    }
    for (int j = 0; j < i; ++j);
  }

}

int main(int argc, char* argv[]) {
    f();
    return 0;
}
