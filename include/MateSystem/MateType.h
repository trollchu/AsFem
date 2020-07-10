//****************************************************************
//* This file is part of the AsFem framework
//* A Simple Finite Element Method program (AsFem)
//* All rights reserved, Yang Bai @ CopyRight 2020
//* https://github.com/yangbai90/AsFem.git
//* Licensed under GNU GPLv3, please see LICENSE for details
//* https://www.gnu.org/licenses/gpl-3.0.en.html
//****************************************************************
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ Author : Yang Bai
//+++ Date   : 2020.07.09
//+++ Purpose: Define the material type in AsFem
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma once

enum class MateType{
    NULLMATE,
    LINEARELASTICMATE,
    SAINTVENANTMATE,
    NEOHOOKEANMATE,
    MOONEYRIVLINMATE,
    POISSONMATE,
    DIFFUSIONMATE,
    THERMALMATE,
    WAVEMATE,
    CAHNHILLIARDMATE,
    LINEARELASTICPFFRACTUREMATE,
    HYPERELASTICPFFRACTUREMATE,
    DENDRITEMATE,
    USER1MATE,
    USER2MATE,
    USER3MATE,
    USER4MATE,
    USER5MATE,
    USER6MATE,
    USER7MATE,
    USER8MATE,
    USER9MATE,
    USER10MATE
};