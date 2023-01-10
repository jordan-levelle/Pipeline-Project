// Project 3 Pipeline.cpp : This file contains the 'main' function. Program execution begins and ends there.

// Jordan C. Levelle
// Copyright 2021
// Computer Architecture 472 Spring 2021


#include <iostream>
#include <iomanip>
#include "Pipeline.h"
using namespace std;

int main() // Start of Main Program
{
    
    Pipeline_Reg Test;
    int instruct;

    int instructionSet[8] // Test instructions
    {
    instructionSet[0] = 0x00A63820,
    instructionSet[1] = 0x8D0F0004,
    instructionSet[2] = 0xAD09FFFC,
    instructionSet[3] = 0x00625022,
    instructionSet[4] = 0x00000000,
    instructionSet[5] = 0x00000000,
    instructionSet[6] = 0x00000000,
    instructionSet[7] = 0x00000000,
    };

    for (int i = 0; i < 8; i++)
    {
        cout << "\nCLOCK CYCLE: " << dec << i + 1 << endl;
        instruct = instructionSet[i];
        Test.IF_stage(instruct);
        Test.ID_stage();
        Test.EX_stage();
        Test.MEM_stage();
        Test.WB_stage();
        Test.Print_out_everything();
        Test.Copy_write_to_read();
    } 
    /*
   
    Pipeline_Reg Run;
    int instruct;
    
    int instructionSet[12]
    {
    instructionSet[0] = 0xA1020000,
    instructionSet[1] = 0x810AFFFC,
    instructionSet[2] = 0x00831820,
    instructionSet[3] = 0x01263820,
    instructionSet[4] = 0x01224820,
    instructionSet[5] = 0x81180000,
    instructionSet[6] = 0x81510010,
    instructionSet[7] = 0x00624022,
    instructionSet[8] = 0x00000000,  // # This is a nop, used just to allow the "real" instructions finish in the pipeline
    instructionSet[9] = 0x00000000,  // # " " 
    instructionSet[10] = 0x00000000, // # " "
    instructionSet[11] = 0x00000000  // # " "
    
    };
    
    
    for (int i = 0; i < 12; i++) 
    {
        cout << "\nCLOCK CYCLE: " << dec << i + 1 << endl;
        instruct = instructionSet[i];
        Run.IF_stage(instruct);
        Run.ID_stage();
        Run.EX_stage();
        Run.MEM_stage();
        Run.WB_stage();
        Run.Print_out_everything();
        Run.Copy_write_to_read();
    }
    */

    return 0;
}// End of Main Program


