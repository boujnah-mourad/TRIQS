
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

#include "./src/matrix.hpp"
#include "./src/expressions/matrix_algebra.hpp"
using namespace std;
using namespace triqs::arrays;
typedef triqs::arrays::matrix<double,Option::Fortran > MM; 

struct bare_loop_with_access { 
 void operator()() { 
  
  MM A (20,30);
  for (int i =0; i<20; ++i)
   for (int j=0; j<30; ++j) 
    A(i,j) = 10*i+ j;

  MM B(A), C(A);

  for (int u =0; u<1000; ++u)
   for (int i =0; i<20; ++i)
    for (int j=0; j<30; ++j) 
     A(i,j) = B(i,j) + 3* C(i,j);
 }
};

struct pointer_and_expression_template { 
 MM A,B,C;

 pointer_and_expression_template():A(20,30) {}
 double f(size_t i, size_t j) const { return B(i,j) + 3*C(i,j);}
 
inline void F(double * restrict p, size_t sh,double const & rhs) { p[sh]=rhs;}
inline void F2(double & r,double const & rhs) { r=rhs;}

 void operator()() { 
  
  for (int i =0; i<20; ++i)
   for (int j=0; j<30; ++j) 
    A(i,j) = 10*i+ j;

 B = A; C=A;

 double * p = &A(0,0);
 double * restrict pB = &B(0,0);

 MM const & BB(B);
 size_t s1=1, s2 = 20; 
  boost::tuple<int,int> t;
  for (int u =0; u<1000; ++u)
    for (int j=0; j<30; ++j) {
   for (int i =0; i<20; ++i ) { 
     p[i*s1 + j*s2] = (B + 3* C)(i,j);
     //F(p,i*s1 + j*s2, (B + 3* C)(i,j));
     //F2(p[i*s1 + j*s2], (B + 3* C)(i,j));
    
    }
 }
 }
};

struct expression_template { 
 void operator()() { 
  MM A (20,30);
  for (int i =0; i<20; ++i)
   for (int j=0; j<30; ++j) 
    A(i,j) = 10*i+ j;

  MM B(A), C(A);

  for (int u =0; u<1000; ++u)
    A = B + 3*C;
 }
};


#include "./speed_tester.hpp"
int main() {
 speed_tester<bare_loop_with_access> (5000);
 speed_tester<pointer_and_expression_template> (5000);
 speed_tester<expression_template> (5000);
 return 0;
}

