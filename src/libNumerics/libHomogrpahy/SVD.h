/* Singular value decomposition.
    Copyright (C) 2014 Victoria Rudakova <vicrucann@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SVD_H
#define SVD_H

#include "matrix.h"

#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#include <cassert>

namespace libNumerics {
	//typedef double flnum;

	/// Functor to sort elements of a vector by decreasing value.
	template <typename T>
	class SVDElement {
	public:
		SVDElement(const vector<T>& D, int i)
			: m_val(D(i)), m_i(i) {}
		bool operator<(const SVDElement& e) const
		{ return (m_val>e.m_val); }

		T m_val;
		int m_i;
	};

	/// Singular Value Decomposition: U diag(D) V, U in O(m), V in O(n), D>=0.
	template <typename T>
	class SVD {
	public:
		matrix<T> U, V;
		vector<T> D;
		typedef matrix<T> Mat;
		typedef vector<T> Vec;
	public:
		SVD(const matrix<T>& A)
			: U(A.nrow(),A.nrow()), V(A.ncol(),A.ncol()), D(std::min(A.nrow(),A.ncol()))
		{
			D.fill(0);
			A.SVD(U, D, V);
			sort();
		}
		/// Recompose from SVD. This should be the initial matrix.
		matrix<T> compose() const
		{
			return U * D.diag(U.ncol(), V.nrow()) * V.t();
		}
		/// Return ith greathest singular value. It is safer than picking directly in D
		/// as it checks against out of bounds.
		T sv(int i) const
		{
			assert(0<=i && (i<U.nrow() || i<V.ncol()));
			if(i<D.nrow())
				return D(i);
			return 0;
		}
		/// Solve the linear system Ax = 0 with ||x|| = 1.0 via SVD.
		/// Check if there is a single singular value such that SV/maxSV<ratioExtremes.
		/// However if the two lowest SV satisfy this, accept if minSV/minSV2<ratio2Min.
		/// This indicates whether the solution is unique.
		static bool Nullspace(const Mat& A, Vec *nullspace,double ratioExtremes=1e-2, double ratio2Min=.5)
		{
			if(A.nrow()+1<A.ncol()) // rank<n-1
				return false;
			SVD svd(A);
			int last=A.lastCol();
			assert(last>=1);
			T minSV1=std::max(svd.sv(last), std::numeric_limits<T>::epsilon()*svd.sv(0));
			T minSV2=std::max(svd.sv(last-1), std::numeric_limits<T>::epsilon()*svd.sv(0));    
			if(minSV1>=ratioExtremes*svd.sv(0) || // No low SV
				(minSV2<ratioExtremes*svd.sv(0) && // 2+ low SV...
				minSV1>=ratio2Min*minSV2))        // ... and rather close
				return false;
			*nullspace = svd.V.col(svd.V.lastCol());
			return true;
		}
		/// Inverse of norm-2 condition value (ratio of extreme singular values)
		T InvCond(const Mat& A) {
			assert(A.nrow()==A.ncol());
			SVD svd(A);
			return svd.D(svd.D.lastRow())/svd.D(0);
		}
		/// Make square matrix of rank<=2.
		void EnforceRank2_3x3(const Mat& A, Mat *ARank)
		{
			assert(A.nrow()==3 && A.ncol()==3);
			SVD svd(A);
			svd.D(svd.D.lastRow()) = 0;
			*ARank = svd.compose();
		}
		/// Save the two last nullspace vectors as 3x3 matrices.
		void Nullspace2_Remap33(const Mat &A, Mat& f1, Mat& f2) {
			assert(A.nrow()==7 && A.ncol()==9);
			assert(f1.nrow()==3 && f1.ncol()==3);
			assert(f2.nrow()==3 && f2.ncol()==3);      
			SVD svd(A);
			f1.read( svd.V.col(svd.V.lastCol()-1) );
			f2.read( svd.V.col(svd.V.lastCol()) );
		}
	private:
		/// Sort SVD by decreasing order of singular value.
		void sort()
		{
			std::vector<SVDElement<T> > vec;
			for(int i=0; i < D.nrow(); i++)
				vec.push_back( SVDElement<T>(D, i) );
			std::sort(vec.begin(), vec.end());
			// Apply permutation
			for(int i=std::min(U.ncol(),V.ncol())-1; i >=0; i--)
				if(vec[i].m_i != i) { // Find cycle of i
					const vector<T> colU = U.col(i);
					const vector<T> colV = V.col(i);
					const T w = D(i);
					int j = i;
					while(vec[j].m_i != i) {
						U.paste(0,j, U.col(vec[j].m_i));
						V.paste(0,j, V.col(vec[j].m_i));
						D(j) = D(vec[j].m_i);
						std::swap(j,vec[j].m_i);
					}
					vec[j].m_i = j;
					U.paste(0,j, colU);
					V.paste(0,j, colV);
					D(j) = w;
				}
		}
	};

} // namespace libNumerics

#endif
