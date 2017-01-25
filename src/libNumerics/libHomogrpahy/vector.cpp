#ifdef MATRIX_H // Do nothing if not included from matrix.h

namespace libNumerics {

/// Constructor
template <typename T>
vector<T>::vector(int m)
: matrix<T>(m, 1)
{}

template <typename T>
vector<T>::vector(void)
: matrix<T>()
{}

/// 1-vector constructor.
//template <typename T>
//vector<T>::vector(T x)
//: matrix<T>(1,1)
//{
//    this->p[0] = x;
//}

/// 2-vector constructor.
template <typename T>
vector<T>::vector(T x, T y)
: matrix<T>(2,1)
{
    this->p[0] = x;
    this->p[1] = y;
}

/// 3-vector constructor.
template <typename T>
vector<T>::vector(T x, T y, T z)
: matrix<T>(3,1)
{
    this->p[0] = x;
    this->p[1] = y;
    this->p[2] = z;
}

/// Copy constructor
template <typename T>
vector<T>::vector(const vector<T>& v)
: matrix<T>(v)
{}

/// Vector made of zeros.
template <typename T>
vector<T> vector<T>::zeros(int m)
{
    vector<T> V(m);
    for(int i = V.size()-1; i >= 0; i--)
        V.p[i] = (T)0;
    return V;
}

/// Vector made of ones.
template <typename T>
vector<T> vector<T>::ones(int m)
{
    vector<T> V(m);
    for(int i = V.size()-1; i >= 0; i--)
        V.p[i] = (T)1;
    return V;
}

/// Assignment operator
template <typename T>
vector<T>& vector<T>::operator=(const vector<T>& v)
{
    matrix<T>::operator=(v);
    return *this;
}

/// Access the \a i-th coefficient.
template <typename T>
inline T vector<T>::operator[] (int i) const
{
    assert(i >= 0 && i < this->nElements());
    return this->p[i];
}

/// Access the \a i-th coefficient.
template <typename T>
inline T& vector<T>::operator[] (int i)
{
    assert(i >= 0 && i < this->nElements());
    return this->p[i];
}

/// Multiply a vector by scalar.
/// \param a a scalar.
template <typename T>
vector<T> vector<T>::operator*(T a) const
{
    vector<T> v(this->m_rows);
    for(int i = this->m_rows-1; i >= 0; i--)
        v.p[i] = a*this->p[i];
    return v;
}

/// Divide a vector by scalar.
/// \param a a scalar.
template <typename T>
inline vector<T> vector<T>::operator/(T a) const
{
    return operator*( (T)1/a );
}

/// Addition of vectors.
template <typename T>
vector<T> vector<T>::operator+(const vector<T>& v) const
{
    assert(this->m_rows == v.m_rows);
    vector<T> sum(this->m_rows);
    for(int i = this->m_rows-1; i >= 0; i--)
        sum.p[i] = this->p[i] + v.p[i];
    return sum;
}

/// Addition of constant value to each element of the vector.
template <typename T>
vector<T> vector<T>::operator+(T a) const
{
    vector<T> sum(this->m_rows);
    for(int i = this->m_rows-1; i >= 0; i--)
        sum.p[i] = this->p[i] + a;
    return sum;
}

/// Subtraction of vectors.
template <typename T>
vector<T> vector<T>::operator-(const vector<T>& v) const
{
    assert(this->m_rows == v.m_rows);
    vector<T> sub(this->m_rows);
    for(int i = this->m_rows-1; i >= 0; i--)
        sub.p[i] = this->p[i] - v.p[i];
    return sub;
}

/// Subtraction a constant value from each element of the vector.
template <typename T>
vector<T> vector<T>::operator-(T a) const
{
    vector<T> sub(this->m_rows);
    for(int i = this->m_rows-1; i >= 0; i--)
        sub.p[i] = this->p[i] - a;
    return sub;
}

/// Opposite of vector.
template <typename T>
vector<T> vector<T>::operator-() const
{
    vector<T> v(this->m_rows);
    for(int i = this->m_rows-1; i >= 0; i--)
        v.p[i] = -this->p[i];
    return v;
}

/// Vector times matrix.
template <typename T>
matrix<T> vector<T>::operator*(const matrix<T>& m) const
{
    return matrix<T>::operator*(m);
}

/// Dot product operator.
template <typename T>
T vector<T>::operator*(const vector<T>& v) const
{
    assert(v.size() == this->size());
#if (_WIN32 && (!_WIN64))
	return ddot_asm(this->p, v.p, this->size()); 
#else
	T res = (T)0;
    for(int i = v.nrow()-1; i >= 0; i--)
      res += this->p[i] * v[i];
    return res;
#endif
}

/// Diagonal matrix defined by its diagonal vector.
template <typename T>
matrix<T> vector<T>::diag() const
{
    matrix<T> d(this->m_rows, this->m_rows);
    d = (T)0;
    for(int i = this->m_rows-1; i >= 0; i--)
        d(i,i) = this->p[i];
    return d;
}

/// Diagonal matrix defined by its diagonal vector.
template <typename T>
matrix<T> vector<T>::diag(int m, int n) const
{
    matrix<T> d = matrix<T>::zeros(m,n);
    for(int i = ((m<n)?m:n)-1; i >= 0; i--)
        d(i,i) = this->p[i];
    return d;
}

/// Square L^2 norm of vector.
template <typename T>
T vector<T>::qnorm() const
{
    T q = (T)0;
    for(int i = this->m_rows-1; i >= 0; i--)
        q += this->p[i]*this->p[i];
	return q;
}

/// Subvector from \a i0 to \a i1.
template <typename T>
vector<T> vector<T>::copy(int i0, int i1) const
{
    assert(0 <= i0 && i0 <= i1 && i1 <= this->m_rows);
    vector<T> v(i1-i0+1);
    for(int i=i0; i <= i1; i++)
        v.p[i-i0] = this->p[i];
    return v;
}

/// SubvectorRef from \a i0 to \a i1.
template <typename T>
vectorRef<T> vector<T>::copyRef(int i0, int i1) const
{
	assert(0 <= i0 && i0 <= i1 && i1 <= this->m_rows);
	return vectorRef<T>(i1-i0+1, this->p+i0);
}

/// Paste vector \a v from row i0.
template <typename T>
void vector<T>::paste(int i0, const vector<T>& v)
{
    matrix<T>::paste(i0, 0, v); 
}

} // namespace libNumerics

/// Scalar product.
template <typename T>
T dot(const libNumerics::vector<T>& u, const libNumerics::vector<T>& v)
{
    assert(u.nrow() == v.nrow());
    T d = (T)0;
    for(int i = u.nrow()-1; i >= 0; i--)
        d += u(i)*v(i);
    return d;
}

/// Cross product.
template <typename T>
libNumerics::vector<T> cross(const libNumerics::vector<T>& u,
                             const libNumerics::vector<T>& v)
{
    assert(u.nrow() == 3 && v.nrow() == 3);
    libNumerics::vector<T> w(3);
    w(0) = u(1)*v(2) - u(2)*v(1);
    w(1) = u(2)*v(0) - u(0)*v(2);
    w(2) = u(0)*v(1) - u(1)*v(0);
    return w;
}

/// Sum of all the vector elements
template <typename T>
T sum(const libNumerics::vector<T>& v)
{
	T s = (T)0;
	for (int i = v.nrow()-1; i >= 0; i--)
		s += v(i);
	return s;
}

/// Mean value of the vector
template <typename T>
T mean(const libNumerics::vector<T>& v)
{
	return (T) ( sum(v) / v.nrow() );
}

/// Index of Max value of the vector
template <typename T>
int max(const libNumerics::vector<T>& v)
{
	T m = v[0];
	int idm = 0;
	for (int i = 1; i < v.nrow()-1; i++)
	{
		if (v[i] > m)
		{
			m = v[i];
			idm = i;
		}
	}
	return idm;
}

#endif // MATRIX_H
