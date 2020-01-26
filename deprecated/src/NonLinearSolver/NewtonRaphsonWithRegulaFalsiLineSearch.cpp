#include "NonLinearSolver/NonLinearSolver.h"

bool NonLinearSolver::NewtonRaphsonWithRegulaFalsiLineSearch(Mesh &mesh,DofHandler &dofHandler,
               SolutionSystem &solutionSystem,EquationSystem &equationSystem,
               ElmtSystem &elmtSystem,MaterialSystem &mateSystem,BCSystem &bcSystem,
               FESystem &feSystem,
               FECtrlInfo &feCtrlInfo){

    _CurrentIters=0;
    bool IsConvergent=false;
    bool IsLineSearchFound=false;
    

    
    // feSystem.PresetDirichletBC(mesh,dofHandler,bcSystem,solutionSystem._Unew);
    bcSystem.ApplyPreSetDirichletBC(feCtrlInfo._CurrentT,mesh,dofHandler,solutionSystem._U);
    solutionSystem._Unew=solutionSystem._U;
    solutionSystem._V.setZero();
    solutionSystem._Utemp.setZero();
    
    _Rnorm=0.0;
    
    while(_CurrentIters<_MaxIters && !IsConvergent){
        if(feCtrlInfo._TimeSteppingMethod==TimeSteppingType::BACKWARDEULER){
            solutionSystem._Utemp=solutionSystem._Unew;
        }
        else if(feCtrlInfo._TimeSteppingMethod==TimeSteppingType::CRANKNICOLSON){
            solutionSystem._Utemp=(solutionSystem._U+solutionSystem._Unew)*feCtrlInfo._ctan[0];
        }
        else{
            solutionSystem._Utemp=solutionSystem._Unew;
        }
        
        feSystem.FormFE(feCtrlInfo._ISW,
                        feCtrlInfo._CurrentT,feCtrlInfo._CurrentDt,feCtrlInfo._ctan,mesh,dofHandler,elmtSystem,mateSystem,
                        solutionSystem._Utemp,solutionSystem._V,solutionSystem._Hist,solutionSystem._HistOld,
                        solutionSystem._ProjValue,
                        equationSystem._AMATRIX,equationSystem._RHS);

        bcSystem.SetMaxKMatrixValue(feSystem.GetMaxAMatrixValue()*1.0e5);

        bcSystem.ApplyBC(mesh,dofHandler,feSystem.GetFEPtr(),
                         feCtrlInfo._CurrentT,feCtrlInfo._ctan,
                         equationSystem._AMATRIX,equationSystem._RHS,
                         solutionSystem._Unew);
        // cout<<equationSystem._AMATRIX<<endl;
        _Rnorm=equationSystem._RHS.norm()/sqrt(feSystem.GetVolume());
        if(_linearSolver.Solve(equationSystem._AMATRIX,equationSystem._RHS,equationSystem._dU)){
            _Enorm=(equationSystem._RHS*equationSystem._dU).norm()/sqrt(feSystem.GetVolume());
            _s0=AtDotB(equationSystem._RHS,equationSystem._dU)/sqrt(feSystem.GetVolume());
            _eta0=1.0;
            _eta=_eta0;
            _s=_s0;

            if(_CurrentIters==0){
                _Rnorm0=_Rnorm;
                _Enorm0=_Enorm;
            }
            // start to do line search to find the suitable update
            IsLineSearchFound=false;_nLineSearch=0;
            _etaL=0.1;_etaU=1.9;
            _eta=2.0;
            double etaold=_eta;
            
            while(/*_s/_s0>_Stol &&*/ _nLineSearch<_MaxLineSearch){
                //*************************************************
                // this algorithm is taken from:
                // https://en.wikipedia.org/wiki/Regula_falsi
                //*************************************************
                //****************************
                // For lower side
                //****************************
                if(feCtrlInfo._TimeSteppingMethod==TimeSteppingType::BACKWARDEULER){
                    solutionSystem._Utemp=solutionSystem._Unew+equationSystem._dU*_etaL;
                }
                else if(feCtrlInfo._TimeSteppingMethod==TimeSteppingType::CRANKNICOLSON){
                    solutionSystem._Utemp=(solutionSystem._Unew+equationSystem._dU*_etaL+solutionSystem._U)*feCtrlInfo._ctan[0];
                }
                else{
                    solutionSystem._Utemp=solutionSystem._Unew+equationSystem._dU*_etaL;
                }
                solutionSystem._V=(solutionSystem._Utemp-solutionSystem._U)*feCtrlInfo._ctan[1];
                //*********
                feSystem.FormFE(3,
                        feCtrlInfo._CurrentT,feCtrlInfo._CurrentDt,feCtrlInfo._ctan,mesh,dofHandler,elmtSystem,mateSystem,
                        solutionSystem._Utemp,solutionSystem._V,solutionSystem._Hist,solutionSystem._HistOld,
                        solutionSystem._ProjValue,
                        equationSystem._AMATRIX,equationSystem._RHS);
                bcSystem.ApplyBC(mesh,dofHandler,feSystem.GetFEPtr(),
                         feCtrlInfo._CurrentT,feCtrlInfo._ctan,
                         equationSystem._AMATRIX,equationSystem._RHS,
                         solutionSystem._Utemp);
                _sL=AtDotB(equationSystem._RHS,equationSystem._dU)/sqrt(feSystem.GetVolume());
                //****************************
                // For upper side
                //****************************
                if(feCtrlInfo._TimeSteppingMethod==TimeSteppingType::BACKWARDEULER){
                    solutionSystem._Utemp=solutionSystem._Unew+equationSystem._dU*_etaU;
                }
                else if(feCtrlInfo._TimeSteppingMethod==TimeSteppingType::CRANKNICOLSON){
                    solutionSystem._Utemp=(solutionSystem._Unew+equationSystem._dU*_etaU+solutionSystem._U)*feCtrlInfo._ctan[0];
                }
                else{
                    solutionSystem._Utemp=solutionSystem._Unew+equationSystem._dU*_etaU;
                }
                solutionSystem._V=(solutionSystem._Utemp-solutionSystem._U)*feCtrlInfo._ctan[1];
                //******
                feSystem.FormFE(3,
                        feCtrlInfo._CurrentT,feCtrlInfo._CurrentDt,feCtrlInfo._ctan,mesh,dofHandler,elmtSystem,mateSystem,
                        solutionSystem._Utemp,solutionSystem._V,solutionSystem._Hist,solutionSystem._HistOld,
                        solutionSystem._ProjValue,
                        equationSystem._AMATRIX,equationSystem._RHS);
                bcSystem.ApplyBC(mesh,dofHandler,feSystem.GetFEPtr(),
                         feCtrlInfo._CurrentT,feCtrlInfo._ctan,
                         equationSystem._AMATRIX,equationSystem._RHS,
                         solutionSystem._Utemp);
                _sU=AtDotB(equationSystem._RHS,equationSystem._dU)/sqrt(feSystem.GetVolume());

                // now we calculate the new eta
                etaold=_eta;
                _eta=_etaU-_sU*(_etaU-_etaL)/(_sU-_sL);
                if(feCtrlInfo._TimeSteppingMethod==TimeSteppingType::BACKWARDEULER){
                    solutionSystem._Utemp=solutionSystem._Unew+equationSystem._dU*_eta;
                }
                else if(feCtrlInfo._TimeSteppingMethod==TimeSteppingType::CRANKNICOLSON){
                    solutionSystem._Utemp=(solutionSystem._Unew+equationSystem._dU*_eta+solutionSystem._U)*feCtrlInfo._ctan[0];
                }
                else{
                    solutionSystem._Utemp=solutionSystem._Unew+equationSystem._dU*_eta;
                }
                solutionSystem._V=(solutionSystem._Utemp-solutionSystem._U)*feCtrlInfo._ctan[1];
                // only forms the residual
                feSystem.FormFE(3,
                        feCtrlInfo._CurrentT,feCtrlInfo._CurrentDt,feCtrlInfo._ctan,mesh,dofHandler,elmtSystem,mateSystem,
                        solutionSystem._Utemp,solutionSystem._V,solutionSystem._Hist,solutionSystem._HistOld,
                        solutionSystem._ProjValue,
                        equationSystem._AMATRIX,equationSystem._RHS);
                bcSystem.ApplyBC(mesh,dofHandler,feSystem.GetFEPtr(),
                         feCtrlInfo._CurrentT,feCtrlInfo._ctan,
                         equationSystem._AMATRIX,equationSystem._RHS,
                         solutionSystem._Utemp);
                // calculate new s
                _s=AtDotB(equationSystem._RHS,equationSystem._dU)/sqrt(feSystem.GetVolume());
                // then we can calculate the new eta
                if(_s*_sL<0.0){
                    _etaU=_eta;_sU=_s;
                }

                if(_s*_sU<0.0){
                    _etaL=_eta;_sL=_s;
                }
                
               

                _nLineSearch+=1;

                if(_IsDepDebug){
                    printf("***    Iters=%3d, %2d search: s=%11.3e,eta =%11.3e***\n",_CurrentIters+1,_nLineSearch,_s,_eta);
                    printf("***                         sL=%11.3e,etaL=%11.3e***\n",_sL,_etaL);
                    printf("***                         sU=%11.3e,etaU=%11.3e***\n",_sU,_etaU);
                }

                if(abs(etaold-_eta)<_Stol||abs(_s)<_EAbsTol){
                    IsLineSearchFound=true;
                    break;
                }
            }
            if(IsLineSearchFound){
                solutionSystem._Unew+=equationSystem._dU*_eta;
                //bcSystem.ApplyPreSetDirichletBC(feCtrlInfo._CurrentT,mesh,dofHandler,solutionSystem._Unew);
                if(feCtrlInfo._TimeSteppingMethod==TimeSteppingType::BACKWARDEULER||
                   feCtrlInfo._TimeSteppingMethod==TimeSteppingType::CRANKNICOLSON){
                    solutionSystem._V=(solutionSystem._Unew-solutionSystem._U)*feCtrlInfo._ctan[1];
                }
                else{
                    solutionSystem._V=(solutionSystem._Unew-solutionSystem._U)*feCtrlInfo._ctan[1];
                }
                feSystem.ResetMaxAMatrixValue();
                _CurrentIters+=1;
            }
            else{
                cout<<"*** Error: line search failed in NR iteration !!!          ***"<<endl;
                PrintDepIterationInfo();
                return false;
            }

            if(_IsDepDebug){
                PrintDepIterationInfo();
            }

            if(ConvergenceCheck()){
                IsConvergent=true;
                break;
            }
        }
        else{
            cout<<"*** Error: solve Ax=F failed(NewtonRaphson/nonlinearSolver)***"<<endl;
            return false;
        }
    }
    if(_CurrentIters>=_MaxIters && !IsConvergent){
        cout<<"*** Error: newton-raphson solve failed!!!                  ***"<<endl;
        PrintDepIterationInfo();
        return false;
    }
    return IsConvergent;
}