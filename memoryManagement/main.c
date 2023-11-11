/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/main.c to edit this template
 */

/* 
 * File:   main.c
 * Author: student
 *
 * Created on March 15, 2023, 8:32 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "memoryTest.h"
#include <locale.h> /* for comma separated numbers */
/*
 * 
 */
int main(int argc, char** argv) {
    setlocale(LC_NUMERIC,"");
    if (0 != memoryTestOne()) {
        fprintf(stderr,"Failure in memory test one");
        exit (EXIT_FAILURE);
    }
    
    
    if (0 != memoryTestTwo()) {
        fprintf(stderr,"Failure in memory test two");
        exit (EXIT_FAILURE);
    }
    if (0 != memoryTestThree()) {
        fprintf(stderr,"Failure in memory test three");
        exit (EXIT_FAILURE);
    }
    printf("Tests Succeeded");
    return (EXIT_SUCCESS);
}

