#define f (1<<14)

/*prototype*/
int int2fp (int n);
int fp2int (int x);
int fp2int_round (int x);
int add_fp (int x, int y);
int sub_fp (int x, int y);
int add_fpint (int x, int n);
int sub_fpint (int x, int n);
int multi_fp (int x, int y);
int multi_fpint (int x, int n);
int div_fp (int x, int y);
int div_fpint (int x, int n);

int int2fp (int n) {
    return n*f;
}

int fp2int (int x) {
    return x/f;
}

int fp2int_round (int x){
    if (x>=0) return (x+f/2)/f;
    else return (x-f/2)/f;
}

int add_fp (int x, int y){
    return x+y;
}

int sub_fp (int x, int y){
    return x-y;
}

int add_fpint (int x, int n){
    return x+n*f;
}

int sub_fpint (int x, int n){
    return x-n*f;
}

int multi_fp (int x, int y){
    return ((int64_t)x)*y/f;
}

int multi_fpint (int x, int n){
    return x*n;
}

int div_fp (int x, int y){
    return ((int64_t)x)*f/y;
}

int div_fpint (int x, int n){
    return x/n;
}