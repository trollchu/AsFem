//****************************************************************
//* This file is part of the AsFem framework
//* A Simple Finite Element Method program (AsFem)
//* All rights reserved, Yang Bai @ CopyRight 2020
//* https://github.com/yangbai90/AsFem.git
//* Licensed under GNU GPLv3, please see LICENSE for details
//* https://www.gnu.org/licenses/gpl-3.0.en.html
//****************************************************************

#include "MessagePrinter/MessagePrinter.h"

void Msg_InputFileName_Invalid(string filename){
    PetscPrintf(PETSC_COMM_WORLD,"*** Error: input file name(=%20s) is invalid !!!      ***\n",filename.c_str());
    
    // PetscPrintf(PETSC_COMM_WORLD,"*************************************************************************\n");
}