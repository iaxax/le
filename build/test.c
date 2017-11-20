
int f() {
  int s = 0;
  int q = 0;
  int n;
  for (int i = 0; i < n; ++i) {
    if (i % 2 == 0) {
      s += i;
    } else {
      q += 2 * i;
    }
  }

  for (int i, j = 1, k = 0; i < 2; ++i) {
    if (i + 1 < 2) {
      i += 2;
      ++i;
    } else {
      j -= 3;
    }
    if (j > 3) {
      break;
    } else {
      i *= 5;
    }
  }

  int* ptr;
  int arr[2];
  for(;;) {
    q += 1;
    ptr = &q;
    s = *ptr;
    *ptr = 2;
    arr[q + s] = 2;
    q = arr[2];
    int p = 0;
  }
}

int main(int argc, char* argv[]) {
    f();
    return 0;
}
