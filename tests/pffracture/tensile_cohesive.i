*** This is an input file for 2d cohesive phase field fracture model

[mesh]
  type=gmsh
  file=sample.msh
[end]

[dofs]
name=ux uy d
[end]

[projection]
  name=vonMises Hydro sigxx sigyy sigxy
[end]


[elmts]
  [solids]
    type=cohesivepffrac
    dofs=ux uy d
    mate=cohesive
    domain=alldomain
  [end]
[end]

[mates]
  [cohesive]
    type=cohesivepffrac
    params=2.1e2 0.2 2.5    2.7e-3 2.0e-2 1.0e-7     1
    //     E      nu SigmaC Gc     L      viscosity  UseHist
    // UseHist=1-->use the stagger solution
    // UseHist=0-->use the fully coupled solution
  [end]
[end]

[ics]
  [const]
    type=const
    dof=d
    params=0.0
  [end]
[end]


[bcs]
  [fixUx]
    type=dirichlet
    dof=ux
    boundary=bottom
    value=0.0
  [end]
  [fixUy]
    type=dirichlet
    dof=uy
    boundary=bottom
    value=0.0
  [end]
  [loadUx]
    type=dirichlet
    dof=uy
    value=1.0*t
    boundary=top
  [end]
[end]

[timestepping]
  type=be
  dt=1.0e-4
  dtmax=1.0e-3
  dtmin=5.0e-7
  endtime=1.0e2
  adaptive=true
[end]
[nonlinearsolver]
  type=newtonls
  r_abs_tol=5.0e-7
  r_rel_tol=1.5e-8
  maxiters=15
[end]


[job]
  type=transient
  debug=dep
  projection=true
[end]
