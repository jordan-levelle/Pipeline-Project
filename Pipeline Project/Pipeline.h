// Pipeline.h : This file contains the 'specification' function. Program execution begins and ends there.

// Jordan C. Levelle
// Copyright 2021
// Computer Architecture 472 Spring 2021

#pragma once
#include <iostream>
#include <iomanip>
using namespace std;

class Pipeline_Reg {
public:
    // Main Pipeline Methods


    Pipeline_Reg();
    ~Pipeline_Reg();
    void IF_stage(int instructionSet); // Instruction Fetch
    void ID_stage(); // Instruction Decode
    void EX_stage(); // Execution
    void MEM_stage(); // Memory
    void WB_stage(); // Write Back
    void Print_out_everything();
    void Copy_write_to_read();

    static const int MM = 1024;
    int Main_Mem[MM];
    int Regs[32];

private:
 
};




// Instruction Fetch || Instruciton Decode || IF/ID
class IF_ID {
public:
    IF_ID();
    ~IF_ID();
    void ifid_Set(int instruction);
    int ifid_Get();

private:
    int instruct = 0;
};


// Instruction Decode || Execute Operation || ID/EX
class ID_EX {
public:
    ID_EX();
    ~ID_EX();

    void idex_set_Reg1(int);
    void idex_set_Reg2(int); 
    void idex_setOffset(short);

    int idex_get_Reg1();
    int idex_get_Reg2();
    int idex_getOffset();
    
    void idex_set_writeRegister_2016(int);
    void idex_set_writeRegister_1511(int);
    
    int idex_get_writeRegister_2016();
    int idex_get_writeRegister_1511();
   
    void idex_setFunction(int);

    int idex_getFunction();

    void idex_set_ALUsrc(int);
    void idex_set_regDst(int);
    void idex_set_ALUop(int);

    int idex_get_ALUsrc();
    int idex_get_regDst();
    int idex_get_ALUop();

    void idex_set_memoryRead(int);
    void idex_set_memoryWrite(int);
    
    int idex_get_memoryRead();
    int idex_get_memoryWrite();

    void idex_set_memorytoRegister(int);
    void idex_set_registerWrite(int);

    int idex_get_memorytoRegister();
    int idex_get_registerWrite();

private:
 
    //Control Unit
    // EX
    int ALUsrc = 0;
    int registerDst = 0;
    int ALUop = 0;
    //  M
    // int branchUnit; No branches
    int memoryRead = 0;
    int memoryWrite = 0;
    // WB
    int memorytoRegister = 0;
    int registerWrite = 0;

    // Register, Sign Extender
    int readReg1 = 0;
    int readReg2 = 0;
    int writeRegister_2016 = 0;
    int writeRegister_1511 = 0;
    short signExtended = 0;
    int function = 0;
    

};



// Execute Operation || Access Memory || EX/MEM
class EX_MEM {
public:
    EX_MEM();
    ~EX_MEM();

    void exmem_set_ALUresult(int);
    void exmem_set_SWvalue(int);
    void exmem_set_writeRegisterNum(int);

    int exmem_get_ALUresult();
    int exmem_get_SWvalue();
    int exmem_get_writeRegisterNum();

    void exmem_set_memoryRead(int);
    void exmem_set_memoryWrite(int);
    void exmem_set_memorytoRegister(int);
    void exmem_set_registerWrite(int);

    int exmem_get_memoryRead();
    int exmem_get_memoryWrite();
    int exmem_get_memorytoRegister();
    int exmem_get_registerWrite();


private:

    int memoryRead = 0;
    int memoryWrite = 0;
    int memorytoRegister = 0;
    int registerWrite = 0;

    int ALUres = 0;
    int SWval = 0;
    int writeRegisterNum = 0;
};
  


// Access Memory || Write Back || MEM/WB
class MEM_WB {
public:
    MEM_WB();
    ~MEM_WB();

    void memwb_set_lwdata(int);
    void memwb_set_ALUresult(int);
    void memwb_set_writeRegisterNum(int);
   
    int memwb_get_lwdata();
    int memwb_get_ALUresult();
    int memwb_get_writeRegisterNum();

    void memwb_set_memorytoRegister(int);
    void memwb_set_registerWrite(int);

    int memwb_get_memorytoRegister();
    int memwb_get_registerWrite();

private:
   
    int memorytoRegister = 0;
    int registerWrite = 0;

    int ALUres = 0;
    int writeRegisterNum = 0;
    int LWval = 0;
};




