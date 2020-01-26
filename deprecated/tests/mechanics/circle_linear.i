*** 2d linear elastic problem

[mesh]
  type=gmsh
  file=circle.msh
[end]

[dofs]
name=ux uy
[end]

[projection]
name=von hystress sig_xx sig_yy sig_xy str_xx str_yy str_xy
[end]

[elmts]
  [solids]
    type=mechanics
	  dofs=ux uy
    mate=elastic
  [end]
[end]

[mates]
  [elastic]
    type=linearelastic
    params=1.0e5 0.25
    // params= E nu(Youngs modulus and poisson ratio)
  [end]
[end]


[bcs]
  [fix_ux]
    type=dirichlet
    dof=ux
    value=0.0
    boundary=bottom
  [end]
  [fix_uy]
    type=dirichlet
    dof=uy
    value=0.0
    boundary=bottom
  [end]
  [loadux]
    type=dirichlet
    dof=uy
    value=0.1
    boundary=left
  [end]
  [shearux]
    type=dirichlet
    dof=ux
    value=0.15
    boundary=left
  [end]
[end]

[job]
  type=static
  debug=dep
  projection=true
[end]