#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_set>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

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
        delete[] data;
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
    for (int i = 0; i < dim * dim; i++) {
        c_data[i] = 0;
    }
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
        // printf("no depad\n");
        return;
    }

    int dim = A->dim;
    int* c_data = (int*) malloc(sizeof(int) * (dim - 1) * (dim - 1));

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

matrix* strassen(matrix* M1, matrix* M2, matrix* data_store, int crossover) {

    // pad here if needed for A and B
    if (M1->dim == 1) {
        data_store->data[0] = M1->data[0] * M2->data[0];
        // std::cout << "one num, " << M1->data[0] * M2->data[0] << "\n";
        return data_store;
    }
    // printf("here1\n");
    if (M1->dim <= crossover) {
        // printf("here\n");
        // print(M1);
        // print(M2);
        matrix* res = mult(M1, M2);
        // print(res);
        data_store->data = res->data;
        // print(data_store);
        free(res);
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

    working_data1 = strassen(A, FH, working_data1, crossover);
    add_to(AFBH, working_data1);
    add_to(CFDH, working_data1);
    //free(P1);

    working_data1 = strassen(AB, H, working_data1, crossover);
    sub_from(AEBG, working_data1);
    add_to(AFBH, working_data1);
    //free(P2);

    working_data1 = strassen(CD, E, working_data1, crossover);
    add_to(CEDG, working_data1);
    sub_from(CFDH, working_data1);
    //free(P3);
    
    working_data1 = strassen(D, GE, working_data1, crossover);
    add_to(AEBG, working_data1);
    add_to(CEDG, working_data1);
    // free(P4);

    working_data1 = strassen(AD, EH, working_data1, crossover);
    add_to(AEBG, working_data1);
    add_to(CFDH, working_data1);
    // free(P5);

    working_data1 = strassen(BD, GH, working_data1, crossover);
    add_to(AEBG, working_data1);
    // free(P6);

    working_data1 = strassen(CA, EF, working_data1, crossover);
    add_to(CFDH, working_data1);
    // free(P7);

    free(working_data1);
    // std::cout << "here are the quads: \n";
    // print(AEBG);
    // print(AFBH);
    // print(CEDG);
    // print(CFDH);
    depad(AEBG);
    depad(AFBH);
    depad(CEDG);
    depad(CFDH);
    // printf("h\n");
    matrix* res = combine_quads(AEBG, AFBH, CEDG, CFDH, data_store);
    // printf("h2\n");
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

    // printf("h3\n");

    depad(res);
    // depad(A);
    // depad(B);
    // printf("h4\n");
    return res;
}

matrix* build_matrix(float probability, int dim) {
    std::random_device rd;    
    std::mt19937 generator (rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    int* data = (int*)malloc(sizeof(int) * dim * dim);

    for (int i = 0; i < dim; i++) {
        for (int j = 0; j <= i; j++) {
            data[i * dim + j] = dis(generator) < probability ? 1 : 0; 
            data[j * dim + i] = data[i * dim + j];
        }
    }
    return new matrix(data, dim);
}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: ./strassen 0 dimension inputfile\n");
        return 1;
    }

    // int debug = std::stoi(argv[1], nullptr, 0);
    int dim = std::stoi(argv[2], nullptr, 0);
    char* filename = argv[3];

    std::ifstream myfile;
    myfile.open(filename);
    int count = 0;

    int* data_A = (int*)malloc(sizeof(int) * dim * dim);
    int* data_B = (int*)malloc(sizeof(int) * dim * dim);

    std::string num;
    bool flag = false;
    while(myfile >> num){
        //std::cout << num << '\n';
        if (!flag && count >= dim * dim) {
            flag = true;
            count = 0;
        }
        // printf("%i, %s\n", count, num.c_str());
        if (flag) {
            // printf("here\n");
            data_B[count] = std::stoi(num, nullptr, 0);
        } else {
            data_A[count] = std::stoi(num, nullptr, 0);
        }
        count++;
    }
    myfile.close();

    matrix* A = new matrix(data_A, dim);
    matrix* B = new matrix(data_B, dim);

    // print(A);
    // print(B);

    // int* data_store = (int*)malloc(sizeof(int) * dim * dim);

    // matrix* data_store_C = new matrix(data_store, dim);

    // print(A);
    // print(B);

    // using namespace std::chrono;
    // auto start_naive = high_resolution_clock::now();
    // matrix* D = mult(A, B);
    // auto stop_naive = high_resolution_clock::now();
    // auto duration_naive = duration_cast<microseconds>(stop_naive - start_naive);
    // duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    // float time_naive = duration_naive.count() / 1000000.0;
    // printf("Multiplied naively in %f seconds\n", time_naive);

    // printf("conventional:\n");
    // print(D);

    // matrix* D = mult(A, B);
    // printf("naive\n");
    // print(D);

    // print(A);
    // print(B);
    // matrix* C1 = strassen(A, B, data_store_C, 1);
    // printf("no base case\n");
    // print(C1);
    // depad(A);
    // depad(B);

    // print(A);
    // print(B);
    std::ofstream datafile;
    datafile.open("data3.csv");
    datafile << "p,triangle\n";
    // for (int i = 10; i < 200; i++) {
    //     printf("n = %i\n", i);
    //     auto start_strassen = high_resolution_clock::now();
    //     matrix* C = strassen(A, B, data_store_C, i); 
    //     auto stop_strassen = high_resolution_clock::now();
    //     auto duration_strassen = duration_cast<microseconds>(stop_strassen - start_strassen);
    //     duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    //     float time_strassen = duration_strassen.count() / 1000000.0;
    //     printf("Multiplied strassen in %f seconds\n", time_strassen);
    //     datafile << i << "," << time_strassen << "\n";
    // }
    // datafile.close();
    for (int i = 1; i < 6; i++){
        datafile << i << ',';
        float bigsum = 0.f;
        for (int j = 0; j < 10; j++) {
            matrix* E = build_matrix(0.01 * i, 1024);
            int* data_store = (int*)malloc(sizeof(int) * dim * dim);
            matrix* data_store_C = new matrix(data_store, dim);
            matrix* intermediate = strassen(E, E, data_store_C, 50);

            int* data_store2 = (int*)malloc(sizeof(int) * dim * dim);
            matrix* data_store_C2 = new matrix(data_store2, dim);
            matrix* done = strassen(intermediate, E, data_store_C2, 50);

            int sum = 0;
            
            for (int i = 0; i < dim; i++) {
                for (int j = 0; j < dim; j++) {
                    if (i == j) {
                        // printf("%i\n", D->data[i*dim+j]);
                        // printf("%i\n", done->data[i*dim+j]);
                        sum += done->data[i*dim+j];
                        // printf("\n");
                    }
                }
            }
            bigsum += sum / 6.f;
            printf("%f\n", sum / 6.f);
        }
        datafile << (bigsum / 10.f) << "\n";
        
    }
    datafile.close();
    
    // bool yay = true;
    // for (int i = 0; i < D->dim * D->dim; i++) {
    //     if (D->data[i] != C->data[i]){
    //         yay = false;
    //         break;
    //     }
    // }
    // std::cout << yay << '\n';
}




