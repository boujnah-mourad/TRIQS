
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "./python_stuff.hpp"

#include "./src/array.hpp"
#include "./src/vector.hpp"
#include "./src/matrix.hpp"
#include "./src/expressions/matrix_algebra.hpp"
#include "./src/linalg/matmul.hpp"
#include "./src/linalg/mat_vec_mul.hpp"
#include "./src/linalg/inverse.hpp"
#include "./src/linalg/determinant.hpp"
#include <iostream>

using namespace std;
using namespace triqs::arrays;
namespace blas = boost::numeric::bindings::blas;
namespace lapack = boost::numeric::bindings::lapack;
namespace bindings= boost::numeric::bindings;

int main(int argc, char **argv) {

 init_python_stuff(argc,argv);

 triqs::arrays::matrix<double,Option::Fortran > A(5,5);
 triqs::arrays::matrix<double > Ac(5,5);
 typedef triqs::arrays::vector<double> vector_type;
 vector_type  MC(5), MB(5);

 for (int i =0; i<5; ++i)
  for (int j=0; j<5; ++j)
   A(i,j) = i+2*j+1; 
 
 Ac = A;

 MC() = 1;MB()=0; 
 range R(1,3); 
 cout<< "A = "<< A<< endl<<endl;

 //MB(R) = 2* mat_vec_mul(A(R,R),MC(R));
 MB(R) =  A(R,R) * MC(R) ;
 cout<< "A(R,R) = "<< A(R,R)<< endl<<endl;
 cout<<" MC(R) = "<<MC(R)<< endl<<endl;
 cout<<" MB = "<< A(R,R) * MC(R) <<" = "<<MB<< endl<<endl;

 cout<<"testing infix"<<endl<<endl;

 //MB(R) += mat_vec_mul(A(R,R),MC(R));
 MB(R) += A(R,R) * MC(R);
 cout<< "A = "<< A(R,R)<< endl<<endl;
 cout<<" MC = "<<MC<< endl<<endl;
 cout<<" MB(R) += A(R,R) * MC(R) = "<<MB<< endl<<endl;

 cout<<" explicit blas call"<<endl<<endl;

 triqs::arrays::matrix_view<double > Acw =  A.transpose();

 vector_type::view_type MB_w(MB(R));

 cout<<" MC = "<<MC<< endl<<endl;

 blas::gemv(1,A(R,R), MC(R), 0, MB_w);
 cout<< "A = "<< A(R,R)<< endl<<endl;
 cout<<" MB = "<<MB<< endl<<endl;

 blas::gemv(1,Ac(R,R), MC(R), 0, MB_w);
 cout<< "A = "<< Ac(R,R)<< endl<<endl;
 cout<<" MB = "<<MB<< endl<<endl;

 blas::gemv(1,Acw(R,R), MC(R), 0, MB_w);
 cout<< "A = "<< Acw(R,R)<< endl<<endl;
 cout<<" MB = "<<MB<< endl<<endl;

 //cout<< A(R,R) * A(R,R) << " = " << triqs::arrays::matrix<double,Option::Fortran > (A(R,R) * A(R,R)) <<endl<<endl; 

}



