double evaluate(int n, double d) {
  double res = 0.0;
  int t = 3;
  t++;
  int p = n;
  ++p;

  if (n > 0) {
    for (int i = 0; i < n; ++i) {
      res += 1 / i + t;
    }

    for (int i = 0; i * i < n; ++i) {
      res = res - i - p;
      for (int j = 0; j < n; ++j) {
        res *= 2;
      }
    }
  } else {
    for (int i = n; i < 0; ++i) {
      if (d > 10) {
        res *= 2;
      } else {
        d = d - i;
        res += d;
      }
    }
  }

  res = res - d;
  return d - res;
}
