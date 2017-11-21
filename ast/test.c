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

void f() {
    do {
        int i, k;
        int j = 0;
        int *p = &i;
    } while(2);
}

int main(int argc, char* argv[]) {
    f();
    return 0;
}
