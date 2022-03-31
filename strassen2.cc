#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_set>
#include <ctime>
#include <iostream>

struct matrix {
    int* data;
    int dim;
    bool odd;

    matrix(int* d, int n) {
        data = d;
        dim = n;
        odd = dim % 2 != 0;
    }
};

void print(matrix* A) {
    for (int i = 0; i < A->dim; i++) {
        for (int j = 0; j < A->dim; j++) {
            std::cout << A->data[i * A->dim + j] << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

matrix* add(matrix* A, matrix* B) {
    int* o_data = (int*) malloc(sizeof(int) * A->dim * A->dim);
    for (int i = 0; i < A->dim * A->dim; i++) {
        o_data[i] = A->data[i] + B->data[i];
    }
    return new matrix(o_data, A->dim);
}

matrix* sub(matrix* A, matrix* B) {
    int* o_data = (int*) malloc(sizeof(int) * A->dim * A->dim);
    for (int i = 0; i < A->dim * A->dim; i++) {
        o_data[i] = A->data[i] - B->data[i];
    }
    return new matrix(o_data, A->dim);
}

matrix* mult(matrix* A, matrix* B) {
    int* c_data;
    int dim = A->dim;
    c_data = (int*) malloc(sizeof(int) * dim * dim);
    for (int i = 0; i < dim; i++) {
        for (int k = 0; k < dim; k++) {
            for (int j = 0; j <dim; j++) {
                c_data[i * dim + j] += A->data[i * dim + k] * B->data[k * dim + j];
            }
        }
    }
    return new matrix(c_data, dim);
}

void pad(matrix* A) {
    int dim = A->dim;
    int* c_data = (int*) malloc(sizeof(int) * (dim + 1) * (dim + 1));
    int count = 0;
    for (int i = 0; i < dim * dim; i++){
        c_data[count] = A->data[i];
        count++;
        if ((i + 1) % dim == 0) {
            c_data[count] = 0;
            count++;
        }
    }
    for (int i = 0; i < dim; i++) {
        c_data[count + i] = 0;
    }
    free(A->data);
    A->data = c_data;
    A->dim = dim + 1;
}

void depad(matrix* A) {
    int dim = A->dim;
    int* c_data = (int*) malloc(sizeof(int) * dim * dim);

    int count = 0;
    for (int i = 0; i < dim * dim; i++) {
        if ((i + 1) % dim == 0) {
            continue;
        }
        if (count >= (dim-1) * (dim-1)) {
            break;
        }

        c_data[count] = A->data[i];
        count++;
    }

    free(A->data);
    A->data = c_data;
    A->dim = dim - 1;
}

// matrix* strassen(matrix* A, matrix* B) {
//     int dim = A->dim;
//     // pad here if needed for A and B
//     int* data = (int*) malloc(sizeof(int) * dim * dim);
//     //matrix* AEBG = new matrix(data, dim/2);
// }


int main() {
    int data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int dataa[9] = {1, 0, 0, 0, 1, 0, 0, 0 ,1};

    int* data1 = (int*)malloc(sizeof(int) * 3 * 3);
    int* data2 = (int*)malloc(sizeof(int) * 3 * 3);
    
    for (int i = 0; i < 9; i++){
        data1[i] = data[i];
        data2[i] = dataa[i];
    }

    matrix* A = new matrix(data1, 3);
    matrix* B = new matrix(data2, 3);
    pad(A);
    print(A);
    pad(B);
    print(B);
    matrix* C = mult(A, A);
    print(C);
    depad(C);
    print(C);

    
    
}




