//int foo(int j) {
//    for (int i = 1, a = 0, c = 1; i < 2; ++i) {
//        if (i > 1) {
//            i += 1;
//        } else if (i > 0) {
//            i -= 100;
//        }
//        else i--;
//    }
//
//    for (j = 0; j < 2; ++j);
//
//    while (j < 2) {
//        if (j > 3) break;
//        else break;
//    }
//    j++;
//    return 0;
//}
//
//void bar() {
//    int i = 2;
//    switch(i) {
//    case 0:
//        break;
//    case 1:
//        i++; break;
//    default:
//        break;
//    }
//
//    foo(1);
//}

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
  }
}

int main(int argc, char* argv[]) {
    f();
    return 0;
}
