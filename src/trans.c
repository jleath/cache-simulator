/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "../include/cachelab.h"


int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void transpose_32(int M, int N, int A[N][M], int B[M][N]);
void transpose_64(int M, int N, int A[N][M], int B[M][N]);
void transpose_61(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    /* Select the right function based on the input. */
    if (M == 32) {
        transpose_32(M, N, A, B);
    } else if (M == 64) {
        transpose_64(M, N, A, B);
    } else if (M == 61) {
        transpose_61(M, N, A, B);
    }
}

/* For 32x32 use 8 byte block */
void transpose_32(int M, int N, int A[N][M], int B[M][N])
{
#define BLOCK_SIZE 8
    int col, row, i, j, a00, a01, a02, a03, a04, a05, a06, a07;
    for (row = 0; row <= N-BLOCK_SIZE; row += BLOCK_SIZE) {
        for (col = 0; col <= M-BLOCK_SIZE; col += BLOCK_SIZE) {
            for (i = row; i < row + BLOCK_SIZE; i++) {
                j = col;
                
                a00 = A[i][j];
                a01 = A[i][j+1];
                a02 = A[i][j+2];
                a03 = A[i][j+3];
                a04 = A[i][j+4];
                a05 = A[i][j+5];
                a06 = A[i][j+6];
                a07 = A[i][j+7];
                
                B[j][i] = a00;
                B[j+1][i] = a01;
                B[j+2][i] = a02;
                B[j+3][i] = a03;
                B[j+4][i] = a04;
                B[j+5][i] = a05;
                B[j+6][i] = a06;
                B[j+7][i] = a07;
            }
        }     
    }
#undef BLOCK_SIZE
}

/* For 64x64 use 4 byte block */
void transpose_64(int M, int N, int A[N][M], int B[M][N])
{
#define BLOCK_SIZE 4
    int col, row, i, j, a00, a01, a02, a03;
    for (row = 0; row <= N-BLOCK_SIZE; row += BLOCK_SIZE) {
        for (col = 0; col <= M-BLOCK_SIZE; col += BLOCK_SIZE) {
            for (i = row; i < row + BLOCK_SIZE; i++) {
                j = col;
                
                a00 = A[i][j];
                a01 = A[i][j+1];
                a02 = A[i][j+2];
                a03 = A[i][j+3];
                
                B[j][i] = a00;
                B[j+1][i] = a01;
                B[j+2][i] = a02;
                B[j+3][i] = a03;
            }
        }     
    }
#undef BLOCK_SIZE
}

/* For 61x67 use 8 byte block */
void transpose_61(int M, int N, int A[N][M], int B[M][N])
{
#define BLOCK_SIZE 8
    int col, row, i, j, a00, a01, a02, a03, a04, a05, a06, a07;
    for (row = 0; row < N-BLOCK_SIZE; row += BLOCK_SIZE) {
        for (col = 0; col < M-BLOCK_SIZE; col += BLOCK_SIZE) {
            for (i = row; i < row + BLOCK_SIZE; i++) {
                j = col;
                
                a00 = A[i][j];
                a01 = A[i][j+1];
                a02 = A[i][j+2];
                a03 = A[i][j+3];
                a04 = A[i][j+4];
                a05 = A[i][j+5];
                a06 = A[i][j+6];
                a07 = A[i][j+7];
                
                B[j][i] = a00;
                B[j+1][i] = a01;
                B[j+2][i] = a02;
                B[j+3][i] = a03;
                B[j+4][i] = a04;
                B[j+5][i] = a05;
                B[j+6][i] = a06;
                B[j+7][i] = a07;
            }
        }     
    }
    /* Since 61 and 67 are not multiples of 8 we have to do some clean up. */
    /* Fill in the right side of the array. */
    if (row != N) {
        for (j = 0; j < M-BLOCK_SIZE; j += BLOCK_SIZE) {
            for (i = row; i < N; ++i) {
                a00 = A[i][j];
                a01 = A[i][j+1];
                a02 = A[i][j+2];
                a03 = A[i][j+3];
                a04 = A[i][j+4];
                a05 = A[i][j+5];
                a06 = A[i][j+6];
                a07 = A[i][j+7]; 
                
                B[j][i] = a00;
                B[j+1][i] = a01;
                B[j+2][i] = a02;
                B[j+3][i] = a03;
                B[j+4][i] = a04;
                B[j+5][i] = a05;
                B[j+6][i] = a06;
                B[j+7][i] = a07;
            }
        }
    }
    
    /* Fill in the bottom of the array. */
    if (col != M) {
        for (i = 0; i < N; ++i) {
            for (j = col; j < M; ++j) {
                B[j][i] = A[i][j];
            }
        }
    }
#undef BLOCK_SIZE
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

