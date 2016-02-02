#pragma once
#pragma warning(push)

#include <cstdlib>
#include "EulerAngles.hpp"
#include <utility>

namespace math { namespace vector {
  template<size_t D, typename T>
  class raw_vector;
} }

typedef math::vector::raw_vector<2, int> Vec2i;
typedef math::vector::raw_vector<3, int> Vec3i;
typedef math::vector::raw_vector<3, int> Vec4i;
typedef math::vector::raw_vector<2, float> Vec2;
typedef math::vector::raw_vector<3, float> Vec3;
typedef math::vector::raw_vector<4, float> Vec4;
typedef math::vector::raw_vector<4, unsigned char> RGBA;

namespace math { namespace vector {
  namespace detail {
    template<size_t D, typename T>
    class _raw_vector {
    public:
      typedef T& reference;
      typedef const T& const_reference;
      
      T& operator[] (size_t index);
      inline const T& operator[] (size_t index) const;
      
      T& At(size_t index);
      const T& At(size_t index) const;
      
      raw_vector<D, T>& operator+=(const raw_vector<D, T>& other);
      raw_vector<D, T>& operator-=(const raw_vector<D, T>& other);
	 
      raw_vector<D, T>& operator*=(const raw_vector<D, T>& other);
      raw_vector<D, T>& operator/=(const raw_vector<D, T>& other);
      raw_vector<D, T>& operator+=(const T& c);
      raw_vector<D, T>& operator-=(const T& c);
      raw_vector<D, T>& operator*=(const T& c);
      raw_vector<D, T>& operator/=(const T& c);
      
      raw_vector<D, T> operator+(const raw_vector<D, T>& other) const;
      raw_vector<D, T> operator-(const raw_vector<D, T>& other) const;
      raw_vector<D, T> operator*(const raw_vector<D, T>& other) const;
      raw_vector<D, T> operator/(const raw_vector<D, T>& other) const;
      raw_vector<D, T> operator+(const T& c) const;
      raw_vector<D, T> operator-(const T& c) const;
      raw_vector<D, T> operator*(const T& c) const;
      raw_vector<D, T> operator/(const T& c) const;

	  bool operator==(const raw_vector<D, T>& other) const;
	  bool operator<(const raw_vector<D, T>& other) const;
      
      raw_vector<D, T> operator-() const;
      
      //template <size_t D2, typename = typename std::enable_if<D2 != D>::type>
      //raw_vector<D2, T> resize() const;
      
      T Dot(const _raw_vector& v) const;
      T LengthSquared() const;
	  decltype(std::sqrt(std::declval<T>())) _raw_vector<D, T>::Length() const;
	  T _raw_vector<D, T>::DistanceSquared(const raw_vector<D, T>& other) const;
	  decltype(std::sqrt(std::declval<T>())) _raw_vector<D, T>::Distance(const raw_vector<D, T>& other) const;
      raw_vector<D, T> Unit() const;
      
      bool operator!=(const raw_vector<D, T>& other) const;
      
    protected:
      _raw_vector();
      _raw_vector& operator=(const _raw_vector& other);
      template <size_t D2>
      _raw_vector(const _raw_vector<D2, T>& that);
      
    private:
      T m_data[D];
	  raw_vector<D, T> const& downcast() const  { return static_cast<raw_vector<D, T> const&>(*this); }
	  raw_vector<D, T> & downcast() { return static_cast<raw_vector<D, T>&>(*this); }
    };
  }
  
  template<size_t D, typename T>
  class raw_vector : public detail::_raw_vector<D, T> {
  public:
    typedef T T;
    typedef T& reference;
    typedef const T& const_reference;
    
    raw_vector() : detail::_raw_vector<D, T>() {}
    raw_vector& operator=(const raw_vector& other) {
      detail::_raw_vector<D, T>::operator=(other);
      return *this;
    }
    template <size_t D2>
    raw_vector(const raw_vector<D2, T>& that);
    
  protected:
  private:
    template <size_t D2, typename T2> friend class detail::_raw_vector;
  };
  
  template<class T>
  class raw_vector<4, T> : public detail::_raw_vector<4, T> {
  public:
    typedef T T;
    typedef T& reference;
    typedef const T& const_reference;
    
    raw_vector<4, T>() : detail::_raw_vector<4, T>() {}
    raw_vector<4, T>& operator=(const raw_vector<4, T>& other) {
      detail::_raw_vector<4, T>::operator=(other);
      return *this;
    }
    raw_vector<4, T>(const T& x, const T& y, const T& z, const T& w);
    template <size_t D2>
    raw_vector<4, T>(const raw_vector<D2, T>& that);
    
    reference x() { return (*this)[0]; }
    const_reference x() const { return (*this)[0]; }
    reference y() { return (*this)[1]; }
    const_reference y() const { return (*this)[1]; }
    reference z() { return (*this)[2]; }
    const_reference z() const { return (*this)[2]; }
    reference w() { return (*this)[3]; }
    const_reference w() const { return (*this)[3]; }
    reference r() { return (*this)[0]; }
    const_reference r() const { return (*this)[0]; }
    reference g() { return (*this)[1]; }
    const_reference g() const { return (*this)[1]; }
    reference b() { return (*this)[2]; }
    const_reference b() const { return (*this)[2]; }
    reference a() { return (*this)[3]; }
    const_reference a() const { return (*this)[3]; }
    
  private:
    template <size_t D2, typename T2> friend class detail::_raw_vector;
  };
  
  template<class T>
  class raw_vector<3, T> : public detail::_raw_vector<3, T> {
  public:
    raw_vector<3, T>() : detail::_raw_vector<3, T>() {}
    raw_vector<3, T>& operator=(const raw_vector<3, T>& other) {
      detail::_raw_vector<3, T>::operator=(other);
      return *this;
    }
    raw_vector<3, T>(const T& x, const T& y, const T& z);
	raw_vector<3, T>(EulerAngles heading);
    template <size_t D2>
    raw_vector<3, T>(const raw_vector<D2, T>& that);

    raw_vector<3, T> Cross(const raw_vector<3, T>& vr) const;
    
	T& x() { return (*this)[0]; }
	const T& x() const { return (*this)[0]; }
	T& y() { return (*this)[1]; }
	const T& y() const { return (*this)[1]; }
	T& z() { return (*this)[2]; }
	const T& z() const { return (*this)[2]; }
	T& r() { return (*this)[0]; }
	const T& r() const { return (*this)[0]; }
	T& g() { return (*this)[1]; }
	const T& g() const { return (*this)[1]; }
	T& b() { return (*this)[2]; }
	const T& b() const { return (*this)[2]; }
    
  private:
    template <size_t D2, typename T2> friend class detail::_raw_vector;
  };
  
  template<class T>
  class raw_vector<2, T> : public detail::_raw_vector<2, T> {
  public:
    typedef T T;
    typedef T& reference;
    typedef const T& const_reference;
    
    raw_vector<2, T>() : detail::_raw_vector<2, T>() {}
    raw_vector<2, T>& operator=(const raw_vector<2, T>& other) {
      detail::_raw_vector<2, T>::operator=(other);
      return *this;
    }
    raw_vector<2, T>(const T& x, const T& y);
    template <size_t D2>
    raw_vector<2, T>(const raw_vector<D2, T>& that);
    
	raw_vector<2, T>  Rotate(float angleInDegrees);

    reference x() { return (*this)[0]; }
    const_reference x() const { return (*this)[0]; }
    reference y() { return (*this)[1]; }
    const_reference y() const { return (*this)[1]; }
  private:
    template <size_t D2, typename T2> friend class detail::_raw_vector;
  };
} }

#define I_AM_RAW_VECTOR_HEADER
#include "raw_vector_impl.hpp"
#undef I_AM_RAW_VECTOR_HEADER
#pragma warning(pop)