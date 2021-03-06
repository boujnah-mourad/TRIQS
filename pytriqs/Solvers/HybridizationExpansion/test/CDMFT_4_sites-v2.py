
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

from pytriqs.Base.Archive import *
from pytriqs.Base.GF_Local import GF_Initializers,GF,GFBloc_ImFreq,inverse
from pytriqs.Base.Lattice.SuperLattice import TBSuperLattice as SuperLattice
from pytriqs.Base.Lattice.TightBinding import TBLattice as Lattice
from pytriqs.Base.SumK.SumK_Discrete_From_Lattice import *
from pytriqs.Base.DMFT.Loop_Generic import *
import pytriqs.Base.Utility.MPI as MPI

#
# A plaquette calculation with CDMFT, and a basic Riemann sum over BZ
#

# A few variables
Beta = 10
Density_Required = None #0.85 #None
Chemical_potential  =  2.0
Field_AF, Field_F = 0.0, 0.0

#
#  Solver
#
from pytriqs.Solvers.HybridizationExpansion.Models import Solver_2x2_Para_Hubbard
S = Solver_2x2_Para_Hubbard(Beta = Beta, U_interact = 4.0)
# I can change some parameter in the S. namespace, overruling the defaults
# I could even change them from iteration to iteration in the loop below.
S.N_Cycles = 3000
S.N_Warmup_Cycles = 0
S.Length_Cycle = 10
S.N_Legendre_Coeffs = 30

# Lattice and Super-Lattice 
t   = -1.00             # First neighbour Hopping
tp  =  0.0*t            # Second neighbour Hopping
hop= {  (1,0)  :  [[ t]], # Hopping[ Displacement on the lattice] = [[t11,t12,t13....],[t21,t22,t23....],...,[....,tnn]] 
        (-1,0) :  [[ t]], # where n=Number_Orbitals
        (0,1)  :  [[ t]],
        (0,-1) :  [[ t]],
        (1,1)  :  [[ tp]],
        (-1,-1):  [[ tp]],
        (1,-1) :  [[ tp]],
        (-1,1) :  [[ tp]]}

L = Lattice ( Units = [(1,0,0) , (0,1,0) ], Hopping = hop)
SL = SuperLattice(BaseLattice=L, SuperLatticeUnits=[ (2,0), (0,2) ])

# SumK function that will perform the sum over the BZ
SK = SumK_Discrete_From_Lattice (TheLattice = SL, Number_Points_in_BZ = 8, Method = "Riemann")

# Defines G and Sigma with a block structure compatible with the SumK function 
G= GF( Name_Block_Generator = [ (s,GFBloc_ImFreq(Indices = SK.GFBlocIndices, Mesh = S.G.mesh)) for s in ['up','down'] ], Copy = False)
Sigma = G.copy()

# Init Sigma
for n,B in S.Sigma : B <<= GF_Initializers.Const(2.0)

# Now I write my DMFT loop...
class myloop (DMFT_Loop_Generic) : 
   def Self_Consistency(self) :
      S.Transform_SymmetryBasis_toRealSpace (IN= S.Sigma, OUT = Sigma) # Embedding     
      
      # Computes sum over BZ and returns density
      F = lambda mu : SK(mu = mu,Sigma = Sigma, Field = None ,Res = G).total_density()/4 
      
      if Density_Required :
         self.Chemical_potential = Dichotomy.Dichotomy(Function = F,
                                                       xinit = self.Chemical_potential, yvalue =Density_Required,
                                                       Precision_on_y = 0.01, Delta_x=0.5,  MaxNbreLoop = 100, 
                                                       xname="Chemical_Potential", yname= "Total Density",
                                                       verbosity = 3)[0]
      else:
         MPI.report("Total density  = %.3f"%F(self.Chemical_potential))

      S.Transform_RealSpace_to_SymmetryBasis (IN = G, OUT = S.G)       # Extraction 
      S.G0 = inverse(S.Sigma + inverse(S.G))                           # Finally get S.G0 

# Construct an instance and run.
myloop(Solver_List = S,Chemical_potential  = 2.0).run(N_Loops = 1)
                                                                     
# Opens the results shelve
if MPI.IS_MASTER_NODE():
  Results = HDF_Archive("CDMFT_4_sites-v2.output.h5",'w')
  Results["G"] = S.G
