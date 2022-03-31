#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_set>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>

struct matrix {
    int* data;
    int dim;
    bool odd;

    matrix(int* d, int n) {
        data = d;
        dim = n;
        odd = dim % 2 != 0;
    }

    matrix(int* d, int n, bool o) {
        data = d;
        dim = n;
        odd = o;
    }
    ~matrix() {
        free(data);
    }
};

// struct quad {
//     matrix* mat;
//     int seg;

//     quad(matrix* matrix, int segm) {
//         mat = matrix;
//         seg = segm;
//     }
// };

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

void add_to(matrix* A, matrix* B) {
    for (int i = 0; i < A->dim * A->dim; i++) {
        A->data[i] += B->data[i];
    }
}

matrix* sub(matrix* A, matrix* B) {
    int* o_data = (int*) malloc(sizeof(int) * A->dim * A->dim);
    for (int i = 0; i < A->dim * A->dim; i++) {
        o_data[i] = A->data[i] - B->data[i];
    }
    return new matrix(o_data, A->dim);
}

void sub_from(matrix* A, matrix* B) {
    for (int i = 0; i < A->dim * A->dim; i++) {
        A->data[i] -= B->data[i];
    }
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
    if (!(A->odd && A->dim % 2 == 0)) {
        return;
    }

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

matrix* extract_quad(matrix* A, int quad) {
    int half_dim = A->dim / 2;
    int* c_data = (int*) malloc(sizeof(int) * half_dim * half_dim);

    int c = quad % 2 == 0 ? 0 : half_dim;
    int r = quad < 2 ? 0 : half_dim;
    // printf("c %i, r %i\n", c, r);

    int count = 0;
    for (int i = r; i < r + half_dim; i++) {
        for (int j = c; j < c + half_dim; j++) {
            // printf("i %i, j %i\n", i, j);
            c_data[count] = A->data[i * A->dim + j];
            count++;
        }
    }
    return new matrix(c_data, half_dim);
}

matrix* combine_quads(matrix* A, matrix* B, matrix* C, matrix* D, matrix* working_data) {
    int full_dim = A->dim * 2;
    if (working_data->odd) {
        full_dim--;
    }
    // int* c_data = (int*) malloc(sizeof(int) * full_dim * full_dim);

    for (int i = 0; i < full_dim; i++) {
        for (int j = 0; j < full_dim; j++) {
            int data = 0;
            if (i < A->dim) {
                if (j < A->dim) { // 0
                    data = A->data[i * A->dim + j];
                } else { // 1
                    data = B->data[i * A->dim + j - A->dim];
                }
            } else {
                if (j < A->dim) { // 2
                    data = C->data[(i - A->dim) * A->dim + j];
                } else { // 3
                    data = D->data[(i - A->dim) * A->dim + j - A->dim];
                }
            }
            working_data->data[i * full_dim + j] = data;
        }
    }
    //matrix* ret = new matrix(working_data, full_dim, odd);
    return working_data;
}

matrix* strassen(matrix* M1, matrix* M2, matrix* data_store) {

    // pad here if needed for A and B
    if (M1->dim == 1) {
        data_store->data[0] = M1->data[0] * M2->data[0];
        // std::cout << "one num, " << M1->data[0] * M2->data[0] << "\n";
        return data_store;
    }
    
    if (M1->dim % 2 != 0) {
        pad(M1);
        pad(M2);
    }
    // print(M1);
    // print(M2);
    // std::cout << "pooooop\n";

    int dim = M1->dim;
    int* data = (int*) malloc(sizeof(int) * dim * dim);

    matrix* A = extract_quad(M1, 0);
    matrix* B = extract_quad(M1, 1);
    matrix* C = extract_quad(M1, 2);
    matrix* D = extract_quad(M1, 3);
    matrix* E = extract_quad(M2, 0);
    matrix* F = extract_quad(M2, 1);
    matrix* G = extract_quad(M2, 2);
    matrix* H = extract_quad(M2, 3);

    int* AEBG_data = (int*) malloc(sizeof(int) * (dim / 2) * (dim / 2));
    int* AFBH_data = (int*) malloc(sizeof(int) * (dim / 2) * (dim / 2));
    int* CEDG_data = (int*) malloc(sizeof(int) * (dim / 2) * (dim / 2));
    int* CFDH_data = (int*) malloc(sizeof(int) * (dim / 2) * (dim / 2));
    
    for (int i = 0; i < (dim / 2) * (dim / 2); i++) {
        AEBG_data[i] = 0;
        AFBH_data[i] = 0;
        CEDG_data[i] = 0;
        CFDH_data[i] = 0;
    }

    matrix* AEBG = new matrix(AEBG_data, dim / 2);
    matrix* AFBH = new matrix(AFBH_data, dim / 2);
    matrix* CEDG = new matrix(CEDG_data, dim / 2);
    matrix* CFDH = new matrix(CFDH_data, dim / 2);

    matrix* FH = sub(F, H);
    matrix* AB = add(A, B);
    matrix* CD = add(C, D);
    matrix* GE = sub(G, E);
    matrix* AD = add(A, D);
    matrix* EH = add(E, H);
    matrix* BD = sub(B, D);
    matrix* GH = add(G, H);
    matrix* CA = sub(C, A);
    matrix* EF = add(E, F);
    
    
    int* working_data = (int*) malloc(sizeof(int) * (dim / 2) * (dim / 2));

    matrix* working_data1 = new matrix(working_data, dim / 2);

    working_data1 = strassen(A, FH, working_data1);
    add_to(AFBH, working_data1);
    add_to(CFDH, working_data1);
    //free(P1);

    working_data1 = strassen(AB, H, working_data1);
    sub_from(AEBG, working_data1);
    add_to(AFBH, working_data1);
    //free(P2);

    working_data1 = strassen(CD, E, working_data1);
    add_to(CEDG, working_data1);
    sub_from(CFDH, working_data1);
    //free(P3);
    
    working_data1 = strassen(D, GE, working_data1);
    add_to(AEBG, working_data1);
    add_to(CEDG, working_data1);
    // free(P4);

    working_data1 = strassen(AD, EH, working_data1);
    add_to(AEBG, working_data1);
    add_to(CFDH, working_data1);
    // free(P5);

    working_data1 = strassen(BD, GH, working_data1);
    add_to(AEBG, working_data1);
    // free(P6);

    working_data1 = strassen(CA, EF, working_data1);
    add_to(CFDH, working_data1);
    // free(P7);

    free(working_data1);
    // std::cout << "here are the quads: \n";
    // print(AEBG);
    // print(AFBH);
    // print(CEDG);
    // print(CFDH);
    // depad(AEBG);
    // depad(AFBH);
    // depad(CEDG);
    // depad(CFDH);

    matrix* res = combine_quads(AEBG, AFBH, CEDG, CFDH, data_store);
    
    // print(res);

    free(AEBG);
    free(AFBH);
    free(CEDG);
    free(CFDH);

    free(FH);
    free(AB);
    free(CD);
    free(GE);
    free(AD);
    free(EH);
    free(BD);
    free(GH);
    free(CA);
    free(EF);

    free(A);
    free(B);
    free(C);
    free(D);
    free(E);
    free(F);
    free(G);
    free(H);

    // printf("this is the result:\n");
    // print(res);
    depad(res);
    depad(A);
    depad(B);
    return res;
}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: ./strassen 0 dimension inputfile\n");
        return 1;
    }

    int debug = std::stoi(argv[1], nullptr, 0);
    int dim = std::stoi(argv[2], nullptr, 0);
    char* filename = argv[3];

    std::ifstream myfile;
    myfile.open(filename);
    int count = 0;

    int* data_A = (int*)malloc(sizeof(int) * dim * dim);
    int* data_B = (int*)malloc(sizeof(int) * dim * dim);

    std::string num;
    while(myfile >> num){
        //std::cout << num << '\n';
        if (count < dim * dim) {
            data_A[count] = std::stoi(num, nullptr, 0);
        } else {
            data_B[count % (dim * dim)] = std::stoi(num, nullptr, 0);
        }
        
        count++;
    }
    myfile.close();

    matrix* A = new matrix(data_A, dim);
    matrix* B = new matrix(data_B, dim);

    print(A);
    print(B);

    int* data_store = (int*)malloc(sizeof(int) * dim * dim);

    matrix* data_store_C = new matrix(data_store, dim);

    matrix* D = mult(A, B);
    print(D);

    matrix* C = strassen(A, B, data_store_C);
    print(C);
    
}




