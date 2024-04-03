int collatz_conjecture(int input){
    if (input%2==0){
        input=input/2;
    }
    else{
        input=input*3+1;
    }
    return input;
}

int test_collatz_convergence(int input, int max_iter){
    int num_iter = 0;
    int current = input;
    while(num_iter<max_iter && current>1){
        current = collatz_conjecture(current);
        num_iter++;
    }
    if (num_iter == max_iter){
        return -1;
    }
    else return num_iter;
}