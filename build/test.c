double evaluate(int n) {
    double e = 1;
    double t = 1;
    int i=1;
    while (i <= n) {
        t=t/i;
        e=e+t;
        i+=1;
    }
    return e;
}

double evaluate1(int n, double initval) {
    for (int i=1; i<n; ++i) {
        initval += (1/((double)i*i));
    }
    return initval;
}

double evaluate2(double x, int count) {
    double c = 3.75;
    for (int i = 0; i < count; ++i) {
        x = c*x*(1-x);
    }
    return x;
}
