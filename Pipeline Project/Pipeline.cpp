// Pipeline.cpp : This file contains the 'implementation' function. Program execution begins and ends there.

// Jordan C. Levelle
// Copyright 2021
// Computer Architecture 472 Spring 2021

#include <iostream>
#include <iomanip>
#include "Pipeline.h"
using namespace std;

#define MSK_OPCO 0xFC000000 // Masks to use for bitwise and operations which extracts necessary bits,
#define MSK_REG1 0x03E00000 
#define MSK_REG2 0x001F0000
#define MSK_REG3 0x0000F800
#define MSK_FUNC 0x0000003F
#define MSK_IONS 0x0000FFFF

//#define SHFT_OP  >>26 // Shift operations
//#define SHFT_R1  >>21
//#define SHFT_R2  >>16
//#define SHFT_R3  >>11

// Buffer Stages objects 
IF_ID ifid_write;
IF_ID ifid_read;
ID_EX idex_write;
ID_EX idex_read;
EX_MEM exmem_write;
EX_MEM exmem_read;
MEM_WB memwb_write;
MEM_WB memwb_read;

//-------------------------------------------------
// Class Pipeline_Reg
Pipeline_Reg::Pipeline_Reg() // Constructor
{
	int cnt = 0;

	while (cnt < MM)
	{
		for (short i = 0; i < 0xFF + 1; i++)
		{
			Main_Mem[cnt] = i;
			cnt++;
		}
	}

	for (int i = 1; i < 32; i++)
	{
		Regs[0] = 0;
		Regs[i] = 0x100 + i;
	}
}

Pipeline_Reg::~Pipeline_Reg(){}


void Pipeline_Reg::IF_stage(int instruction) // Fetch instruction
{
    ifid_write.ifid_Set(instruction);
}

void Pipeline_Reg::ID_stage() // Decoding and register fetching
{
    int instruction = 0;
    instruction = ifid_read.ifid_Get();

    if (instruction == 0)
    {
        idex_write.idex_set_regDst(0);
        idex_write.idex_set_ALUsrc(0);
        idex_write.idex_set_ALUop(0);
        idex_write.idex_set_memoryRead(0);
        idex_write.idex_set_memoryWrite(0);
        idex_write.idex_set_memorytoRegister(0);
        idex_write.idex_set_registerWrite(0);

        idex_write.idex_setOffset(0);
        idex_write.idex_set_Reg1(0);
        idex_write.idex_set_Reg2(0);
        idex_write.idex_set_writeRegister_1511(0);
        idex_write.idex_set_writeRegister_2016(0);
        idex_write.idex_setFunction(0);
    }
    else
    {
        int opCode = 0;
        opCode = (ifid_read.ifid_Get() & MSK_OPCO) >> 26;
        
        int signExtended = 0;
        signExtended = (ifid_read.ifid_Get() & MSK_IONS);
        idex_write.idex_setOffset(signExtended);

        int reg1 = 0;
        reg1 = (ifid_read.ifid_Get() & MSK_REG1) >> 21;
        idex_write.idex_set_Reg1(Regs[reg1]);

        int reg2 = 0;
        reg2 = (ifid_read.ifid_Get() & MSK_REG2) >> 16;
        idex_write.idex_set_Reg2(Regs[reg2]);

        int reg3 = 0;
        reg3 = (ifid_read.ifid_Get() & MSK_REG3) >> 11;

        idex_write.idex_set_writeRegister_2016(reg2);
        idex_write.idex_set_writeRegister_1511(reg3);

        int inputFunc = 0;
        inputFunc = (ifid_read.ifid_Get() & MSK_FUNC);
        idex_write.idex_setFunction(inputFunc);

        if (opCode == 0)
        {
            idex_write.idex_set_regDst(1);
            idex_write.idex_set_ALUsrc(0);
            idex_write.idex_set_ALUop(10);
            
            idex_write.idex_set_memoryRead(0);
            idex_write.idex_set_memoryWrite(0);
            idex_write.idex_set_memorytoRegister(0);
            idex_write.idex_set_registerWrite(1);
        }
        else if (opCode == 0x20)
        {   // lb
            idex_write.idex_set_regDst(0);
            idex_write.idex_set_ALUsrc(1);
            idex_write.idex_set_ALUop(00);
           
            idex_write.idex_set_memoryRead(1);
            idex_write.idex_set_memoryWrite(0);
            idex_write.idex_set_memorytoRegister(1);
            idex_write.idex_set_registerWrite(1);
        }
        else if (opCode == 0x28)
        {   // sb
            idex_write.idex_set_ALUsrc(1);
            idex_write.idex_set_ALUop(00);
            idex_write.idex_set_memoryRead(0);
            idex_write.idex_set_memoryWrite(1);
            idex_write.idex_set_registerWrite(0);
        }
    }
}
   
void Pipeline_Reg::EX_stage() // Perform requested instruction on the specific operands read from idex_read 
{
    // filler nop
    if (idex_write.idex_get_regDst() == 0 && idex_write.idex_get_ALUsrc() && idex_write.idex_get_ALUop() && idex_write.idex_get_memoryRead() && idex_write.idex_get_memoryWrite() && idex_write.idex_get_memorytoRegister() && idex_write.idex_get_registerWrite())
    {
        exmem_write.exmem_set_memoryRead(0);
        exmem_write.exmem_set_memoryWrite(0);
        exmem_write.exmem_set_memorytoRegister(0);
        exmem_write.exmem_set_registerWrite(0);
    }
    else
    {
        exmem_write.exmem_set_memoryRead(idex_read.idex_get_memoryRead());
        exmem_write.exmem_set_memoryWrite(idex_read.idex_get_memoryWrite());
        exmem_write.exmem_set_memorytoRegister(idex_read.idex_get_memorytoRegister());
        exmem_write.exmem_set_registerWrite(idex_read.idex_get_registerWrite());

        int ALUresult = 0;

        if (idex_read.idex_get_ALUop() == 10 && idex_read.idex_get_ALUsrc() == 0 && idex_read.idex_get_regDst() == 1)
        {
            if (idex_read.idex_getFunction() == 0x20) // add
            {
                ALUresult = idex_read.idex_get_Reg1() + idex_read.idex_get_Reg2();
                exmem_write.exmem_set_ALUresult(ALUresult);

            }
            else if (idex_read.idex_getFunction() == 0x22) // sub
            {
                ALUresult = idex_read.idex_get_Reg1() - idex_read.idex_get_Reg2();
                exmem_write.exmem_set_ALUresult(ALUresult);
            }

            int SWvalue = 0;
            SWvalue = idex_read.idex_get_Reg2();
            exmem_write.exmem_set_SWvalue(SWvalue);

            int writeRegNum = 0;
            writeRegNum = idex_read.idex_get_writeRegister_1511();
            exmem_write.exmem_set_writeRegisterNum(writeRegNum);
        }
        else if (idex_read.idex_get_ALUsrc() == 00 && idex_read.idex_get_regDst() == 1 && idex_read.idex_get_ALUop() == 0) // lb
        {
            ALUresult = idex_read.idex_get_Reg1() + idex_read.idex_getOffset();
            exmem_write.exmem_set_writeRegisterNum(ALUresult);
            
            int SWvalue = 0;
            SWvalue = idex_read.idex_get_Reg2();
            exmem_write.exmem_set_SWvalue(SWvalue);

            int writeRegNum = idex_read.idex_get_writeRegister_2016();
            exmem_write.exmem_set_writeRegisterNum(writeRegNum);
        }
        else if (idex_read.idex_get_ALUsrc() == 0 && idex_read.idex_get_ALUop() == 1) // sb
        {
            ALUresult = idex_read.idex_get_Reg1() + idex_read.idex_getOffset();
            exmem_write.exmem_set_ALUresult(ALUresult);

            int SWvalue = 0;
            SWvalue = idex_read.idex_get_Reg2();
            exmem_write.exmem_set_SWvalue(SWvalue);
        }
        else // buffer nop
        {
            ALUresult = 0;
            exmem_write.exmem_set_ALUresult(0);
            exmem_write.exmem_set_writeRegisterNum(0);
            exmem_write.exmem_set_SWvalue(0);
        }

    }
}

void Pipeline_Reg::MEM_stage() // If instruct is lb index into Main mem or just pass from exmem_read to memwb_write
{
    memwb_write.memwb_set_memorytoRegister(exmem_read.exmem_get_memorytoRegister()); // control bits
    memwb_write.memwb_set_registerWrite(exmem_read.exmem_get_registerWrite());

    memwb_write.memwb_set_ALUresult(exmem_read.exmem_get_ALUresult()); // ALU result

    // filler nop
    if (exmem_read.exmem_get_memoryRead() == 0 && exmem_read.exmem_get_memoryWrite() == 0 && exmem_read.exmem_get_memorytoRegister() == 0 && exmem_read.exmem_get_registerWrite() == 0)
    {
        memwb_write.memwb_set_writeRegisterNum(0);
        memwb_write.memwb_set_lwdata(0);
    }
    else if(exmem_read.exmem_get_memoryRead() == 0 && exmem_read.exmem_get_memoryWrite() == 0) // r type instruction
    {
        memwb_write.memwb_set_writeRegisterNum(exmem_read.exmem_get_writeRegisterNum());
    }
    else if(exmem_read.exmem_get_memoryRead() == 1 && exmem_read.exmem_get_memoryWrite() == 0) // lb instruction
    {
        memwb_write.memwb_set_lwdata(Main_Mem[(exmem_read.exmem_get_ALUresult())]);
        memwb_write.memwb_set_writeRegisterNum(exmem_read.exmem_get_writeRegisterNum());
    }
    else if(exmem_read.exmem_get_memoryRead() == 0 && exmem_read.exmem_get_memoryWrite() == 1) // sb instruction
    {
        Main_Mem[exmem_read.exmem_get_ALUresult()] = exmem_read.exmem_get_SWvalue();
    }
}

void Pipeline_Reg::WB_stage() // Write to registers based on read from memwb_read
{
    if (memwb_read.memwb_get_memorytoRegister() == 0 && memwb_read.memwb_get_registerWrite() == 1) // r type instruction
    {
        Regs[memwb_read.memwb_get_writeRegisterNum()] = memwb_read.memwb_get_ALUresult();
    }
    else if (memwb_read.memwb_get_memorytoRegister() == 1 && memwb_read.memwb_get_registerWrite() == 1) // lb instruction
    {
        Regs[memwb_read.memwb_get_writeRegisterNum()] = Main_Mem[memwb_read.memwb_get_ALUresult()];
    }
}

void Pipeline_Reg::Print_out_everything()
{
    cout << "Registers: " << endl;
    cout << "\n";
    for (size_t i = 0; i < 32; i++)
    {
        cout << "Regs" << dec << i << ":" << "0x" << hex << Regs[i] << endl;
    }


    // cout if/id
    cout << "\nIF/ID WRITE" << " Instruction 0x" << setfill('0') << setw(8) << hex << ifid_write.ifid_Get() << endl;
    cout << "IF/ID READ" << " Instruction 0x" << setfill('0') << setw(8) << hex << ifid_read.ifid_Get() << endl;
    cout << endl;

    // cout id/ex write
    cout << "ID/EX WRITE" << endl;
    cout << "RegisterDest: " << idex_write.idex_get_regDst() << endl;
    cout << "ALUsrc: " << idex_write.idex_get_ALUsrc() << endl;
    cout << "ALUop: " << dec << idex_write.idex_get_ALUop() << endl;
    cout << "Memread: " << idex_write.idex_get_memoryRead() << endl;
    cout << "Memwrite: " << idex_write.idex_get_memoryWrite() << endl;
    cout << "Memory to Register: " << idex_write.idex_get_memorytoRegister() << endl;
    cout << "Register Write: " << idex_write.idex_get_registerWrite() << endl;
    cout << "Read Reg 1 Value: " << "0x" << hex << idex_write.idex_get_Reg1() << endl;
    cout << "Read Reg 2 Value: " << "0x" << hex << idex_write.idex_get_Reg2() << endl;
    cout << "Sign Extend offset: " << setw(8) << hex << idex_write.idex_getOffset() << endl;
    cout << "Write Register 16-20: " << dec << idex_write.idex_get_writeRegister_2016() << endl;
    cout << "Write Register 11-15: " << dec << idex_write.idex_get_writeRegister_1511() << endl;
    cout << "Function: " << "0x" << hex << idex_write.idex_getFunction() << endl;
    cout << endl;

    // cout id/ex read
    cout << "ID/EX READ" << endl;
    cout << "RegisterDest: " << idex_read.idex_get_regDst() << endl;
    cout << "ALUsrc: " << idex_read.idex_get_ALUsrc() << endl;
    cout << "ALUop: " << dec << idex_read.idex_get_ALUop() << endl;
    cout << "Memread: " << idex_read.idex_get_memoryRead() << endl;
    cout << "Memwrite: " << idex_read.idex_get_memoryWrite() << endl;
    cout << "Memory to Register: " << idex_read.idex_get_memorytoRegister() << endl;
    cout << "Register Write: " << idex_read.idex_get_registerWrite() << endl;
    cout << "Read Reg 1 Value: " << "0x" << hex << idex_read.idex_get_Reg1() << endl;
    cout << "Read Reg 2 Value: " << "0x" << hex << idex_read.idex_get_Reg2() << endl;
    cout << "Sign Extend offset: " << setw(8) << hex << idex_read.idex_getOffset() << endl;
    cout << "Write Register 16-20: " << dec << idex_read.idex_get_writeRegister_2016() << endl;
    cout << "Write Register 11-15: " << dec << idex_read.idex_get_writeRegister_1511() << endl;
    cout << "Function: " << "0x" << hex << idex_read.idex_getFunction() << endl;
    cout << endl;

    // cout ex/mem write
    cout << "EX/MEM WRITE" << endl;
    cout << "Memread: " << exmem_write.exmem_get_memoryWrite() << endl;
    cout << "Memwrite: " << exmem_write.exmem_get_memoryWrite() << endl;
    cout << "Memory to register: " << exmem_write.exmem_get_memorytoRegister() << endl;
    cout << "Register Write: " << exmem_write.exmem_get_registerWrite() << endl;
    cout << "ALU result: " << "0x" << hex << exmem_write.exmem_get_ALUresult() << endl;
    cout << "SW Value: " << "0x" << hex << exmem_write.exmem_get_SWvalue() << endl;
    cout << "Write Register Number: " << dec << exmem_write.exmem_get_writeRegisterNum() << endl;
    cout << endl;


    // cout ex/mem read
    cout << "EX/MEM READ" << endl;
    cout << "Memread: " << exmem_read.exmem_get_memoryWrite() << endl;
    cout << "Memwrite: " << exmem_read.exmem_get_memoryWrite() << endl;
    cout << "Memory to register: " << exmem_read.exmem_get_memorytoRegister() << endl;
    cout << "Register Write: " << exmem_read.exmem_get_registerWrite() << endl;
    cout << "ALU result: " << "0x" << hex << exmem_read.exmem_get_ALUresult() << endl;
    cout << "SW Value: " << "0x" << hex << exmem_read.exmem_get_SWvalue() << endl;
    cout << "Write Register Number: " << dec << exmem_read.exmem_get_writeRegisterNum() << endl;
    cout << endl;

    // cout mem/wb write
    cout << "MEM/WB WRITE" << endl;
    cout << "Memory to register: " << memwb_write.memwb_get_memorytoRegister() << endl;
    cout << "Register Write: " << memwb_write.memwb_get_registerWrite() << endl;
    cout << "ALU result: " << "0x" << hex << memwb_write.memwb_get_ALUresult() << endl;
    cout << "LW Value: " << "0x" << hex << memwb_write.memwb_get_lwdata() << endl;
    cout << "Write Register Number: " << dec << memwb_write.memwb_get_lwdata() << endl;
    cout << endl;

    // cout mem/wb read
    cout << "MEM/WB READ" << endl;
    cout << "Memory to register: " << memwb_read.memwb_get_memorytoRegister() << endl;
    cout << "Register Write: " << memwb_read.memwb_get_registerWrite() << endl;
    cout << "ALU result: " << "0x" << hex << memwb_read.memwb_get_ALUresult() << endl;
    cout << "LW Value: " << "0x" << hex << memwb_read.memwb_get_lwdata() << endl;
    cout << "Write Register Number: " << dec << memwb_read.memwb_get_lwdata() << endl;
    cout << endl;

    
}

void Pipeline_Reg::Copy_write_to_read()
{
    ifid_read.ifid_Set(ifid_write.ifid_Get());

    idex_read.idex_set_ALUsrc(idex_write.idex_get_ALUsrc());
    idex_read.idex_set_regDst(idex_write.idex_get_regDst());
    idex_read.idex_set_ALUop(idex_write.idex_get_ALUop());
    idex_read.idex_set_memoryRead(idex_write.idex_get_memoryRead());
    idex_read.idex_set_memoryWrite(idex_write.idex_get_memoryWrite());
    idex_read.idex_set_memorytoRegister(idex_write.idex_get_memorytoRegister());
    idex_read.idex_set_registerWrite(idex_write.idex_get_registerWrite());
    idex_read.idex_set_Reg1(idex_write.idex_get_Reg1());
    idex_read.idex_set_Reg2(idex_write.idex_get_Reg2());
    idex_read.idex_setOffset(idex_write.idex_getOffset());
    idex_read.idex_set_writeRegister_2016(idex_write.idex_get_writeRegister_2016());
    idex_read.idex_set_writeRegister_1511(idex_write.idex_get_writeRegister_1511());
    idex_read.idex_setFunction(idex_write.idex_getFunction());


    exmem_read.exmem_set_ALUresult(exmem_write.exmem_get_ALUresult());
    exmem_read.exmem_set_SWvalue(exmem_write.exmem_get_SWvalue());
    exmem_read.exmem_set_writeRegisterNum(exmem_write.exmem_get_writeRegisterNum());
    exmem_read.exmem_set_memoryRead(exmem_write.exmem_get_memoryRead());
    exmem_read.exmem_set_memoryWrite(exmem_write.exmem_get_memoryWrite());
    exmem_read.exmem_set_memorytoRegister(exmem_write.exmem_get_memorytoRegister());
    exmem_read.exmem_set_registerWrite(exmem_write.exmem_get_registerWrite());

    memwb_read.memwb_set_lwdata(memwb_write.memwb_get_lwdata());
    memwb_read.memwb_set_ALUresult(memwb_write.memwb_get_ALUresult());
    memwb_read.memwb_set_writeRegisterNum(memwb_write.memwb_get_writeRegisterNum());
    memwb_read.memwb_set_memorytoRegister(memwb_write.memwb_get_memorytoRegister());
    memwb_read.memwb_set_registerWrite(memwb_write.memwb_get_registerWrite());

}




//-------------------------------------------------
// Instruction Fetch || Instruciton Decode || IF/ID
IF_ID::IF_ID() {}
IF_ID::~IF_ID() {}

void IF_ID::ifid_Set(int instructionSet)
{
    instruct = instructionSet;
}

int IF_ID::ifid_Get()
{
    return instruct;
}




//-------------------------------------------------
// Instruction Decode || Execute Operation || ID/EX
ID_EX::ID_EX() {}
ID_EX::~ID_EX() {}

// Setters
void ID_EX::idex_set_Reg1(int sr1)
{
    readReg1 = sr1;
}
void ID_EX::idex_set_Reg2(int sr2)
{
    readReg2 = sr2;
}
void ID_EX::idex_setOffset(short offset)
{
    signExtended = offset;
}


// Getters
int ID_EX::idex_get_Reg1()
{
    return readReg1;
}
int ID_EX::idex_get_Reg2()
{
    return readReg2;
}
int ID_EX::idex_getOffset()
{
    return signExtended;
}


// Setters
void ID_EX::idex_set_writeRegister_2016(int reg2)
{
    writeRegister_2016 = reg2;
}
void ID_EX::idex_set_writeRegister_1511(int reg3)
{
    writeRegister_1511 = reg3;
}


// Getters
int ID_EX::idex_get_writeRegister_2016()
{
    return writeRegister_2016;
}
int ID_EX::idex_get_writeRegister_1511()
{
    return writeRegister_1511;
}


// Setters
//void ID_EX::setOpcode(int Opcode){}

void ID_EX::idex_setFunction(int inputFunc)
{
    function = inputFunc;
}


// Getters

int ID_EX::idex_getFunction()
{
    return function;
}


// Setters
void ID_EX::idex_set_ALUsrc(int ALUsource)
{
    ALUsrc = ALUsource;
}
void ID_EX::idex_set_regDst(int regDest)
{
    registerDst = regDest;
}
void ID_EX::idex_set_ALUop(int ALUoperation )
{
    ALUop = ALUoperation;
}


// Getters
int ID_EX::idex_get_ALUsrc()
{
    return ALUsrc;
}
int ID_EX::idex_get_regDst()
{
    return registerDst;
}
int ID_EX::idex_get_ALUop()
{
    return ALUop;
}


// Setters

void ID_EX::idex_set_memoryRead(int memRead)
{
    memoryRead = memRead;
}
void ID_EX::idex_set_memoryWrite(int memWrite)
{
    memoryWrite = memWrite;
}


// Getters

int ID_EX::idex_get_memoryRead()
{
    return memoryRead;
}
int ID_EX::idex_get_memoryWrite()
{
    return memoryWrite;
}


// Setters
void ID_EX::idex_set_memorytoRegister(int memReg)
{
    memorytoRegister = memReg;
}
void ID_EX::idex_set_registerWrite(int regWrite)
{
    registerWrite = regWrite;
}


// Getters
int ID_EX::idex_get_memorytoRegister()
{
    return memorytoRegister;
}
int ID_EX::idex_get_registerWrite()
{
    return registerWrite;
}



//-------------------------------------------------
// Execute Operation || Access Memory || EX/MEM
EX_MEM::EX_MEM() {}
EX_MEM::~EX_MEM() {}

// Setters
void EX_MEM::exmem_set_ALUresult(int ALUresult)
{
    ALUres = ALUresult;
}
void EX_MEM::exmem_set_SWvalue(int SWvalue)
{
    SWval = SWvalue;
}
void EX_MEM::exmem_set_writeRegisterNum(int writeRegNum)
{
    writeRegisterNum = writeRegNum;
}


// Getters
int EX_MEM::exmem_get_ALUresult()
{
    return ALUres;
}
int EX_MEM::exmem_get_SWvalue()
{
    return SWval;
}
int EX_MEM::exmem_get_writeRegisterNum()
{
    return writeRegisterNum;
}


// Setters
void EX_MEM::exmem_set_memoryRead(int memRead)
{
    memoryRead = memRead;
}
void EX_MEM::exmem_set_memoryWrite(int memWrite)
{
    memoryWrite = memWrite;
}
void EX_MEM::exmem_set_memorytoRegister(int memReg)
{
    memorytoRegister = memReg;
}
void EX_MEM::exmem_set_registerWrite(int regWrite)
{
    registerWrite = regWrite;
}


// Getters
int EX_MEM::exmem_get_memoryRead()
{
    return memoryRead;
}
int EX_MEM::exmem_get_memoryWrite()
{
    return memoryWrite;
}
int EX_MEM::exmem_get_memorytoRegister()
{
    return memorytoRegister;
}
int EX_MEM::exmem_get_registerWrite()
{
    return registerWrite;
}






//-------------------------------------------------
// Access Memory || Write Back || MEM/WB
MEM_WB::MEM_WB() {}
MEM_WB::~MEM_WB() {}

// Setters
void MEM_WB::memwb_set_lwdata(int LWvalue)
{
    LWval = LWvalue;
}

void MEM_WB::memwb_set_ALUresult(int ALUresult)
{
    ALUres = ALUresult;
}

void MEM_WB::memwb_set_writeRegisterNum(int writeRegNum)
{
    writeRegisterNum = writeRegNum;
}

// Getters
int MEM_WB::memwb_get_lwdata()
{
    return LWval;
}

int MEM_WB::memwb_get_ALUresult()
{
    return ALUres;
}

int MEM_WB::memwb_get_writeRegisterNum()
{
    return LWval;
}

// Setters
void MEM_WB::memwb_set_memorytoRegister(int memtoReg)
{
    memorytoRegister = memtoReg;
}

void MEM_WB::memwb_set_registerWrite(int regWrite)
{
    registerWrite = regWrite;
}

// Getters
int MEM_WB::memwb_get_memorytoRegister()
{
    return memorytoRegister;
}

int MEM_WB::memwb_get_registerWrite()
{
    return registerWrite;
}
