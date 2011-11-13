#include "lab6.h"
#include <stdio.h>

void m_transpose(matrix r, matrix m){
    matrix temp;
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            temp[i][j]=m[j][i];
    m_copy(r,temp);
}
void setIdentity(float mat[4][4]){
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            if(i==j){
                mat[i][j]=1;
            }
            else
                mat[i][j]=0;
}

void m_copy(matrix mdest, matrix m){
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            mdest[i][j] = m[i][j];
        }
    }
}

void m_print(matrix m){
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            printf("%.2f ", m[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
void mm_mult(matrix result, float trans[4][4], float mat[4][4]){
    matrix temp;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            temp[i][j]=0;
            for(int k=0;k<4;k++){
                temp[i][j] += trans[i][k] * mat[k][j];
            }
        }
    }
    m_copy(result,temp);
}
/*
void vm_mult(vector result, matrix mat, vector v){
    vector temp;
    for(int i=0;i<4;i++){
        temp[i]=0;
        for(int j=0;j<4;j++)
            temp[i] += mat[j][i]*v[j];
    }
    v_copy(result,temp);
}
*/
void mv_mult(vector result, matrix mat, vector v){
    vector temp;
    for(int i=0;i<4;i++){
        temp[i]=0;
        for(int j=0;j<4;j++)
            temp[i] += mat[i][j]*v[j];
    }
    v_copy(result,temp);
}
