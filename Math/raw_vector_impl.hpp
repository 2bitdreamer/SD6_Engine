#pragma once
#include <cmath>

#pragma warning(push)

#ifndef I_AM_RAW_VECTOR_HEADER
#error Only include from raw_vector.hpp
#else
#endif

namespace math { namespace vector {
	namespace detail {
		
			template<size_t D, typename T>
			T& _raw_vector<D,T>::operator[](size_t index)
			{
				return m_data[index];
			}

			template<size_t D, typename T>
			const T& _raw_vector<D,T>::operator[](size_t index) const
			{
				return m_data[index];
			}

			template<size_t D, typename T>
			T& _raw_vector<D,T>::At(size_t index)
			{
				return m_data[index];
			}

			template<size_t D, typename T>
			const T& _raw_vector<D,T>::At(size_t index) const
			{
				return m_data[index];
			}

			template<size_t D, typename T>
			raw_vector<D, T>& _raw_vector<D,T>::operator+=(const raw_vector<D, T>& other)
			{
				for(int index = 0; index < D; index++)
				{
					m_data[index] += other[index];
				}
				return downcast();
			}

			template<size_t D, typename T>
			raw_vector<D, T>& _raw_vector<D,T>::operator-=(const raw_vector<D, T>& other)
			{
				for(int index = 0; index < D; index++)
				{
					m_data[index] -= other[index];
				}
				return downcast();
			}

			template<size_t D, typename T>
			raw_vector<D, T>& _raw_vector<D,T>::operator*=(const raw_vector<D, T>& other)
			{
				for(int index = 0; index < D; index++)
				{
					m_data[index] *= other[index];
				}
				return downcast();
			}

			template<size_t D, typename T>
			raw_vector<D, T>& _raw_vector<D,T>::operator/=(const raw_vector<D, T>& other)
			{
				for(int index = 0; index < D; index++)
				{
					m_data[index] /= other[index];
				}
				return downcast();
			}

			template<size_t D, typename T>
			raw_vector<D, T>& _raw_vector<D,T>::operator+=(const T& c)
			{
				for(int index = 0; index < D; index++)
				{
					m_data[index] += c;
				}
				return downcast();
			}

			template<size_t D, typename T>
			raw_vector<D, T>& _raw_vector<D,T>::operator-=(const T& c)
			{
				for(int index = 0; index < D; index++)
				{
					m_data[index] -= c;
				}
				return downcast();
			}

			template<size_t D, typename T>
			raw_vector<D, T>& _raw_vector<D,T>::operator*=(const T& c)
			{
				for(int index = 0; index < D; index++)
				{
					m_data[index] *= c;
				}
				return downcast();
			}

			template<size_t D, typename T>
			raw_vector<D, T>& _raw_vector<D,T>::operator/=(const T& c)
			{
				for(int index = 0; index < D; index++)
				{
					m_data[index] /= c;
				}
				return downcast();
			}

			template<size_t D, typename T>
			bool math::vector::detail::_raw_vector<D, T>::operator<(const raw_vector<D, T>& other) const
			{
				for (int index = 0; index < D; index++)
				{
					const T& element = other.m_data[index];
					const T& thisData = m_data[index];
					if (element < thisData)
						return true;
					else if (thisData < element)
						return false;
				}
				return false;
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::operator+(const raw_vector<D, T>& other) const
			{
				return raw_vector<D,T>(downcast()) += other;
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::operator-(const raw_vector<D, T>& other) const
			{
				return raw_vector<D,T>(downcast()) -= other;
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::operator*(const raw_vector<D, T>& other) const
			{
				return raw_vector<D,T>(downcast()) *= other;
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::operator/(const raw_vector<D, T>& other) const
			{
				return raw_vector<D,T>(downcast()) /= other;;
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::operator+(const T& c) const
			{
				return raw_vector<D,T>(downcast()) += c;
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::operator-(const T& c) const
			{
				return raw_vector<D,T>(downcast()) -= c;
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::operator*(const T& c) const
			{
				return raw_vector<D,T>(downcast()) *= c;
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::operator/(const T& c) const
			{
				return raw_vector<D,T>(downcast()) /= c;
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::operator-() const
			{
				return raw_vector<D,T>(downcast()) -= c;
			}

			template<size_t D, typename T>
			T _raw_vector<D,T>::Dot(const _raw_vector& v) const
			{
				T dotProd = (T)0;
				for(int index = 0; index < D; index++)
				{
					dotProd += (m_data[index] * v[index]);
				}
				return dotProd;
			}

			template<size_t D, typename T>
			T _raw_vector<D,T>::LengthSquared() const
			{
				T lengthSquared = (T)0;
				for(int index = 0; index < D; index++)
				{
					T currentData = m_data[index];
					lengthSquared += (currentData * currentData);
				}
				return lengthSquared;
			}

			template<size_t D, typename T>
			decltype(std::sqrt(std::declval<T>())) _raw_vector<D, T>::Length() const
			{
				return std::sqrt(LengthSquared());
			}

			template<size_t D, typename T>
			T _raw_vector<D,T>::DistanceSquared(const raw_vector<D, T>& other) const
			{
				T distanceSquared = (T)0;
				for(int index = 0; index < D; index++)
				{
					T diff = m_data[index] - other[index];
					distanceSquared += (diff * diff);
				}
				return distanceSquared;
			}

			template<size_t D, typename T>
			decltype(std::sqrt(std::declval<T>())) _raw_vector<D,T>::Distance(const raw_vector<D, T>& other) const
			{

				return std::sqrt(DistanceSquared(other));
			}

			template<size_t D, typename T>
			raw_vector<D, T> _raw_vector<D,T>::Unit() const
			{
				return raw_vector<D,T>(downcast())/= Length();
			}

			template<size_t D, typename T>
			bool _raw_vector<D,T>::operator==(const raw_vector<D, T>& other) const
			{
				bool equal = true;
				int index = 0;
				while(equal && index < D)
				{
					equal = (other[index] == m_data[index]);
					index++;
				}
				return equal;
			}

			template<size_t D, typename T>
			bool _raw_vector<D,T>::operator!=(const raw_vector<D, T>& other) const
			{
				return !(m_data == other);
			}

			template<size_t D, typename T>
			_raw_vector<D,T>::_raw_vector()
			{
				
			}

			template<size_t D, typename T>
			 _raw_vector<D,T>& _raw_vector<D,T>::operator=(const _raw_vector& other)
			{
				for(int index = 0; (index < D); index++)
				{
					m_data[index] = other[index];
				}
				
				return (*this);
			}

			template<size_t D, typename T>
			template<size_t D2>
			_raw_vector<D,T>::_raw_vector(const _raw_vector<D2, T>& that)
			{
				for(int index = 0; index < D; index++)
				{
					if(index < D2)
						m_data[index] = that[index];
					else
						m_data[index] = 0;
				}
			}
	}

	template<size_t D, typename T>
	template<size_t D2>
	raw_vector<D,T>::raw_vector(const raw_vector<D2, T>& that):detail::_raw_vector<D,T>(that)
	{
	}


	template<typename T>
	raw_vector<4, T>::raw_vector(const T& x, const T& y, const T& z, const T& w)
	{
		(*this)[0] = x;
		(*this)[1] = y;
		(*this)[2] = z;
		(*this)[3] = w;
	}


	template<typename T>
	template<size_t D2>
	raw_vector<4, T>::raw_vector(const raw_vector<D2, T>& that):detail::_raw_vector<4,T>(that)
	{
	}

	template<typename T>
	raw_vector<3, T>::raw_vector(const T& x, const T& y, const T& z)
	{
		(*this)[0] = x;
		(*this)[1] = y;
		(*this)[2] = z;
	}

	template<typename T>
	raw_vector<3, T>::raw_vector(EulerAngles heading)
	{
		(*this)[0] = std::cos(DegreesToRadians(heading.yawDegreesAboutZ)) * std::cos(DegreesToRadians(heading.pitchDegreesAboutY));
		(*this)[1] = std::sin(DegreesToRadians(heading.yawDegreesAboutZ)) * std::cos(DegreesToRadians(heading.pitchDegreesAboutY));
		(*this)[2] = -std::sin(DegreesToRadians(heading.pitchDegreesAboutY));
		
	}

	template <typename T>
	template<size_t D2>
	raw_vector<3, T>::raw_vector(const raw_vector<D2, T>& that):detail::_raw_vector<3,T>(that)
	{
	}

	template<typename T>
	raw_vector<3,T> raw_vector<3, T>::Cross(const raw_vector<3, T>& vr) const
	{
		raw_vector<3,T> crossProd = raw_vector<3,T>();
		raw_vector<3,T> vl = (*this);
		crossProd[0] = (vl.y() * vr.z()) - (vl.z() * vr.y());
		crossProd[1] = (vl.z() * vr.x()) - (vl.x() * vr.z());
		crossProd[2] = (vl.x() * vr.y()) - (vl.y() * vr.x());
		return crossProd;
	}

	template<typename T>
	raw_vector<2, T>::raw_vector(const T& x, const T& y)
	{
		(*this)[0] = x;
		(*this)[1] = y;
	}


	template<typename T>
	template<size_t D2>
	raw_vector<2, T>::raw_vector(const raw_vector<D2, T>& that):detail::_raw_vector<2,T>(that)
	{
	}

	template<typename T>
	raw_vector<2,T> raw_vector<2,T>::Rotate( float angleInDegrees)
	{
		raw_vector<3,T> vl = (*this);
		T x2 = (vl[0] * std::cos(DegreesToRadians(angleInDegrees))) - (vl[1] * std::sin(DegreesToRadians(angleInDegrees)));
		T y2 = (vl[1] * std::cos(DegreesToRadians(angleInDegrees))) + (vl[0] * std::sin(DegreesToRadians(angleInDegrees)));
		return Vec2(x2,y2);
	}
}}

#pragma warning(pop)