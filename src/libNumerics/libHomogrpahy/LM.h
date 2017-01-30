#ifndef NUMERICS_H
#define NUMERICS_H

#include "matrix.h"
#include <vector>
#include <cmath>

namespace libNumerics 
{
	static const double DEFAULT_RELATIVE_TOL = 1E-3;
	static const double DEFAULT_LAMBDA_INIT = 1E-3;
	static const double DEFAULT_LAMBDA_FACT = 10.0;
	static const double EPSILON_KERNEL = 1E-9;

	template <typename T>
	inline T ABS(T x)
	{ 
		return (x >= 0)? x: -x; 
	}

	/// Solve system AX = B.
	/// Resolution by LU decomposition with pivot.
	template <typename T> 
	bool solveLU(const matrix<T>& A, const vector<T>& B, vector<T>& X)
	{
		X = B;
		return solveLU(A, X);
	}

	/// Replace X by A^{-1}X, by LU solver.
	template <typename T>
	bool solveLU(matrix<T> A, vector<T>& X)
	{
		assert(A.nrow() == A.ncol());
		int	n = A.nrow();
		vector<T> rowscale(n); // Implicit scaling of each row
		std::vector<int> permut(n,0); // Permutation of rows

		// Get the implicit scaling information of each row
		for(int i=0; i< n; i++) {
			T max = 0.0;
			for(int j=0; j< n; j++) {
				T tmp = ABS(A(i,j));
				if (tmp> max)
					max = tmp;
			}
			if(max == 0.0)
				return false;
			rowscale(i) = 1.0/max;
		}
		// Perform the decomposition
		for(int k=0; k < n; k++) {
			// Search for largest pivot element
			T max = rowscale(k)*ABS(A(k,k));
			int imax = k;
			for(int i=k+1; i < n; i++) {
				T tmp = rowscale(i)*ABS(A(i,k));
				if(tmp > max) {
					max = tmp;
					imax = i;
				}
			}
			if(max == 0.0)
				return false;

			// Interchange rows if needed
			if(k != imax) {
				A.swapRows(k, imax);
				rowscale(imax) = rowscale(k); // Scale of row k no longer needed
			}
			permut[k] = imax; // permut(k) was not initialized before
			T Akk = 1/A(k,k);
			for(int i=k+1; i < n; i++) {
				T tmp = A(i,k) *= Akk; // Divide by pivot
				for (int j=k+1;j < n; j++) // Reduce the row
					A(i,j) -= tmp*A(k,j);
			}
		}
		// Forward substitution
		for (int k = 0; k < n; k++) {
			T sum = X(permut[k]);
			X(permut[k]) = X(k);
			for(int j = 0; j < k; j++)
				sum -= A(k,j)*X(j);
			X(k) = sum;
		}
		// Backward substitution
		for(int k = n - 1; k >= 0; k--) {
			T sum = X(k);
			for(int j=k+1; j < n; j++)
				sum -= A(k,j)*X(j);
			X(k) = sum/A(k,k);
		}
		return true;
	}

    /// Levenberg-Marquardt minimization.
	template <typename T>
	class MinLM 
	{
        
    public:
        MinLM() : iterations(0), relativeTol(DEFAULT_RELATIVE_TOL), lambdaInit(DEFAULT_LAMBDA_INIT), lambdaFact(DEFAULT_LAMBDA_FACT)
		{
		}

		/// Perform minimization.
		/// \a targetRMSE is the root mean square error aimed at.
		/// Return the reached RMSE. Since the class does not know the dimension, the
		/// real RMSE should be this value multiplied by sqrt(dim). For example, for 2-D
		/// points this would be sqrt(2) times the returned value.
        T minimize(vector<T>& P, const vector<T>& yData, T targetRMSE=0.1, int maxIters=300)
		{
			T errorMax = targetRMSE * targetRMSE * yData.nrow();
			vector<T> yModel( yData.nrow() );
			modelData(P, yModel);
			vector<T> E( yData-yModel );
			T error = std::sqrt(E.qnorm());
			matrix<T> J( yData.nrow(), P.nrow() );
			modelJacobian(P, J);
			matrix<T> Jt = J.t();
			matrix<T> JtJ = Jt*J;
			vector<T> B = Jt*E;
			compress(JtJ, B);

			T lambda = lambdaInit;
			for( iterations = 0; iterations < maxIters && error > errorMax; iterations++ ) {
				matrix<T> H(JtJ);
				for(int i = 0; i < H.nrow(); i++)
					H(i, i) *= 1 + lambda;
				vector<T> dP( P.nrow() );
				solveLU(H, B, dP);
				uncompress(dP);
				vector<T> tryP = P + dP;
				modelData(tryP, yModel);
				E = yData - yModel;
				T tryError = std::sqrt(E.qnorm());
				if(ABS(tryError-error) <= relativeTol*error)
					break;
				if(tryError > error)
					lambda *= lambdaFact;
				else {
					lambda /= lambdaFact;
					error = tryError;
					P = tryP;
					modelJacobian(P, J);
					Jt = J.t();
					JtJ = Jt*J;
					B = Jt*E;
					compress(JtJ, B);
				}
			}
			return sqrt(error/yData.nrow());
		}

        virtual void modelData(const vector<T>& P, vector<T>& ymodel) const = 0;
        virtual void modelJacobian(const vector<T>& P, matrix<T>& J) const = 0;
        int iterations;
        T relativeTol;
        T lambdaInit;
        T lambdaFact;
    private:
        std::vector<int> m_nullCols;
        
		/// In equation JtJ X = B, remove columns of J close to 0, so that JtJ can be
		/// invertible
		void compress(matrix<T>& JtJ, vector<T>& B)
		{
			T max=0;
			for(int i=0; i < JtJ.nrow(); i++)
				if(JtJ(i,i) > max)
					max = JtJ(i,i);
			max *= EPSILON_KERNEL;
			m_nullCols.clear();
			for(int i=0; i < JtJ.nrow(); i++)
				if(JtJ(i,i) <= max)
					m_nullCols.push_back(i);
			if( m_nullCols.empty() )
				return;
			int n = (int)m_nullCols.size();
			matrix<T> JtJ2( JtJ.nrow() - n, JtJ.ncol() - n );
			vector<T> B2( B.nrow() - (int)m_nullCols.size() );
			for(int i = 0, i2 = 0; i < JtJ.nrow(); i++) {
				if(i - i2 < n && m_nullCols[i-i2] == i)
					continue;
				for(int j = 0, j2 = 0; j < JtJ.ncol(); j++) {
					if(j - j2 < n && m_nullCols[j - j2] == j)
						continue;
					JtJ2(i2,j2) = JtJ(i,j);
					j2++;
				}
				B2(i2) = B(i);
				i2++;
			}
			swap(JtJ,JtJ2);
			swap(B,B2);
		}

		/// Insert 0 in rows of B that were removed by \c compress()
		void uncompress(vector<T>& B)
		{
			if(m_nullCols.empty())
				return;
			int n = (int)m_nullCols.size();
			vector<T> B2(B.nrow() + (int)m_nullCols.size());
			for(int i = 0, i2 = 0; i2 < B2.nrow(); i2++)
				if(i2-i < n && m_nullCols[i2-i] == i2)
					B2(i2) = 0;
				else
					B2(i2) = B(i++);
			swap(B,B2);
		}

    };

} // namespace libNumerics

#endif
