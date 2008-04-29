// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_MATRIX_H_INCLUDED__
#define __IRR_MATRIX_H_INCLUDED__

#include "irrTypes.h"
#include "vector3d.h"
#include "vector2d.h"
#include "plane3d.h"
#include "aabbox3d.h"
#include "rect.h"
#include "irrString.h"

namespace irr
{
namespace core
{

	//! 4x4 matrix. Mostly used as transformation matrix for 3d calculations.
	/* Matrix4 is mainly used by the Irrlicht engine for doing transformations.
	The matrix is a D3D style matrix, row major with translations in the 4th row.
	*/
	template <class T>
	class CMatrix4
	{
		public:

			//! Constructor Flags
			enum eConstructor
			{
				EM4CONST_NOTHING = 0,
				EM4CONST_COPY,
				EM4CONST_IDENTITY,
				EM4CONST_TRANSPOSED,
				EM4CONST_INVERSE,
				EM4CONST_INVERSE_TRANSPOSED
			};

			CMatrix4( eConstructor constructor = EM4CONST_IDENTITY );
			CMatrix4( const CMatrix4<T>& other,eConstructor constructor = EM4CONST_COPY);

			//! Simple operator for directly accessing every element of the matrix.
			T& operator()(const s32 row, const s32 col) { definitelyIdentityMatrix=false; return M[ row * 4 + col ]; }

			//! Simple operator for directly accessing every element of the matrix.
			const T& operator()(const s32 row, const s32 col) const { return M[row * 4 + col]; }

			//! Simple operator for linearly accessing every element of the matrix.
			T& operator[](u32 index) { definitelyIdentityMatrix=false; return M[index]; }

			//! Simple operator for linearly accessing every element of the matrix.
			const T& operator[](u32 index) const { return M[index]; }

			//! Sets this matrix equal to the other matrix.
			inline CMatrix4<T>& operator=(const CMatrix4<T> &other);

			//! Sets all elements of this matrix to the value.
			inline CMatrix4<T>& operator=(const T& scalar);

			//! Returns pointer to internal array
			const T* pointer() const { return M; }
			T* pointer() { definitelyIdentityMatrix=false; return M; }

			//! Returns true if other matrix is equal to this matrix.
			bool operator==(const CMatrix4<T> &other) const;

			//! Returns true if other matrix is not equal to this matrix.
			bool operator!=(const CMatrix4<T> &other) const;

			//! Add another matrix.
			CMatrix4<T> operator+(const CMatrix4<T>& other) const;

			//! Add another matrix.
			CMatrix4<T>& operator+=(const CMatrix4<T>& other);

			//! Subtract another matrix.
			CMatrix4<T> operator-(const CMatrix4<T>& other) const;

			//! Subtract another matrix.
			CMatrix4<T>& operator-=(const CMatrix4<T>& other);

			//! set this matrix to the product of two matrices
			inline CMatrix4<T>& setbyproduct(const CMatrix4<T>& other_a,const CMatrix4<T>& other_b );

			//! set this matrix to the product of two matrices, no logical optimation
			//! use it if you know you never have a identity matrix
			CMatrix4<T>& setbyproduct_nocheck(const CMatrix4<T>& other_a,const CMatrix4<T>& other_b );

			//! Multiply by another matrix.
			CMatrix4<T> operator*(const CMatrix4<T>& other) const;

			//! Multiply by another matrix.
			CMatrix4<T>& operator*=(const CMatrix4<T>& other);

			//! Multiply by scalar.
			CMatrix4<T> operator*(const T& scalar) const;

			//! Multiply by scalar.
			CMatrix4<T>& operator*=(const T& scalar);

			//! Set matrix to identity.
			inline CMatrix4<T>& makeIdentity();

			//! Returns true if the matrix is the identity matrix
			inline bool isIdentity() const;

			//! Returns true if the matrix is the identity matrix
			bool isIdentity_integer_base () const;

			//! Set the translation of the current matrix. Will erase any previous values.
			CMatrix4<T>& setTranslation( const vector3d<T>& translation );

			//! Gets the current translation
			vector3d<T> getTranslation() const;

			//! Set the inverse translation of the current matrix. Will erase any previous values.
			CMatrix4<T>& setInverseTranslation( const vector3d<T>& translation );

			//! Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
			inline CMatrix4<T>& setRotationRadians( const vector3d<T>& rotation );

			//! Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
			CMatrix4<T>& setRotationDegrees( const vector3d<T>& rotation );

			//! Returns the rotation, as set by setRotation(). This code was orginally written by by Chev.
			core::vector3d<T> getRotationDegrees() const;

			//! Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
			inline CMatrix4<T>& setInverseRotationRadians( const vector3d<T>& rotation );

			//! Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
			CMatrix4<T>& setInverseRotationDegrees( const vector3d<T>& rotation );

			//! Set Scale
			CMatrix4<T>& setScale( const vector3d<T>& scale );

			//! Set Scale
			CMatrix4<T>& setScale( const T scale ) { return setScale(core::vector3d<T>(scale,scale,scale)); }

			//! Get Scale
			core::vector3d<T> getScale() const;

			//! Translate a vector by the inverse of the translation part of this matrix.
			void inverseTranslateVect( vector3df& vect ) const;

			//! Rotate a vector by the inverse of the rotation part of this matrix.
			void inverseRotateVect( vector3df& vect ) const;

			//! Rotate a vector by the rotation part of this matrix.
			void rotateVect( vector3df& vect ) const;

			//! An alternate transform vector method, writing into a second vector
			void rotateVect(vector3df& out,const core::vector3df& in) const;

			//! An alternate transform vector method, writing into an array of 3 floats
			void rotateVect(T *out,const core::vector3df &in) const;

			//! Transforms the vector by this matrix
			void transformVect( vector3df& vect) const;

			//! Transforms input vector by this matrix and stores result in output vector
			void transformVect( vector3df& out, const vector3df& in ) const;

			//! An alternate transform vector method, writing into an array of 4 floats
			void transformVect(T *out,const core::vector3df &in) const;

			//! Translate a vector by the translation part of this matrix.
			void translateVect( vector3df& vect ) const;

			//! Transforms a plane by this matrix
			void transformPlane( core::plane3d<f32> &plane) const;

			//! Transforms a plane by this matrix ( some problems to solve..)
			void transformPlane_new( core::plane3d<f32> &plane) const;

			//! Transforms a plane by this matrix
			void transformPlane( const core::plane3d<f32> &in, core::plane3d<f32> &out) const;

			//! Transforms a axis aligned bounding box
			/** The result box of this operation may not be very accurate. For
			accurate results, use transformBoxEx() */
			void transformBox(core::aabbox3d<f32>& box) const;

			//! Transforms a axis aligned bounding box more accurately than transformBox()
			/** The result box of this operation should by quite accurate, but this operation
			is slower than transformBox(). */
			void transformBoxEx(core::aabbox3d<f32>& box) const;

			//! Multiplies this matrix by a 1x4 matrix
			void multiplyWith1x4Matrix(T* matrix) const;

			//! Calculates inverse of matrix. Slow.
			//! \return Returns false if there is no inverse matrix.
			bool makeInverse();


			//! Inverts a primitive matrix which only contains a translation and a rotation
			//! \param out: where result matrix is written to.
			bool getInversePrimitive ( CMatrix4<T>& out ) const;

			//! returns the inversed matrix of this one
			//! \param out: where result matrix is written to.
			//! \return Returns false if there is no inverse matrix.
			bool getInverse(CMatrix4<T>& out) const;

			//! Builds a right-handed perspective projection matrix based on a field of view
			CMatrix4<T>& buildProjectionMatrixPerspectiveFovRH(f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar);

			//! Builds a left-handed perspective projection matrix based on a field of view
			CMatrix4<T>& buildProjectionMatrixPerspectiveFovLH(f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar);

			//! Builds a right-handed perspective projection matrix.
			CMatrix4<T>& buildProjectionMatrixPerspectiveRH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar);

			//! Builds a left-handed perspective projection matrix.
			CMatrix4<T>& buildProjectionMatrixPerspectiveLH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar);

			//! Builds a left-handed orthogonal projection matrix.
			CMatrix4<T>& buildProjectionMatrixOrthoLH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar);

			//! Builds a right-handed orthogonal projection matrix.
			CMatrix4<T>& buildProjectionMatrixOrthoRH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar);

			//! Builds a left-handed look-at matrix.
			CMatrix4<T>& buildCameraLookAtMatrixLH(
					const vector3df& position,
					const vector3df& target,
					const vector3df& upVector);

			//! Builds a right-handed look-at matrix.
			CMatrix4<T>& buildCameraLookAtMatrixRH(
					const vector3df& position,
					const vector3df& target,
					const vector3df& upVector);

			//! Builds a matrix that flattens geometry into a plane.
			//! \param light: light source
			//! \param plane: plane into which the geometry if flattened into
			//! \param point: value between 0 and 1, describing the light source.
			//! If this is 1, it is a point light, if it is 0, it is a directional light.
			CMatrix4<T>& buildShadowMatrix(const core::vector3df& light, core::plane3df plane, f32 point=1.0f);

			//! Builds a matrix which transforms a normalized Device Coordinate to Device Coordinates.
			/** Used to scale <-1,-1><1,1> to viewport, for example from von <-1,-1> <1,1> to the viewport <0,0><0,640> */
			CMatrix4<T>& buildNDCToDCMatrix( const core::rect<s32>& area, f32 zScale);

			//! creates a new matrix as interpolated matrix from two other ones.
			//! \param b: other matrix to interpolate with
			//! \param time: Must be a value between 0 and 1.
			CMatrix4<T> interpolate(const core::CMatrix4<T>& b, f32 time) const;

			//! returns transposed matrix
			CMatrix4<T> getTransposed() const;

			//! returns transposed matrix to a plain 4x4 float matrix
			inline void getTransposed( CMatrix4<T>& dest ) const;

			/*!
				construct 2D Texture transformations
				rotate about center, scale, and transform.
			*/
			CMatrix4<T>& buildTextureTransform( f32 rotateRad,
					const core::vector2df &rotatecenter,
					const core::vector2df &translate,
					const core::vector2df &scale);

			//! set texture transformation rotation
			//! rotate about z axis, recenter at (0.5,0.5)
			//! doesn't clear other elements than those affected
			//! \param radAngle Angle in radians
			//! \return Altered matrix
			CMatrix4<T>& setTextureRotationCenter( f32 radAngle );

			//! set texture transformation translation
			//! doesn't clear other elements than those affected
			//! \param x Offset on x axis
			//! \param y Offset on y axis
			//! \return Altered matrix
			CMatrix4<T>& setTextureTranslate( f32 x, f32 y );

			//! set texture transformation scale
			//! doesn't clear other elements than those affected
			//! \param sx Scale factor on x axis
			//! \param sy Scale factor on y axis
			//! \return Altered matrix
			CMatrix4<T>& setTextureScale( f32 sx, f32 sy );

			//! set texture transformation scale, and recenter at (0.5,0.5)
			//! doesn't clear other elements than those affected
			//! \param sx Scale factor on x axis
			//! \param sy Scale factor on y axis
			//! \return Altered matrix
			CMatrix4<T>& setTextureScaleCenter( f32 sx, f32 sy );

			//! sets all matrix data members at once
			CMatrix4<T>& setM(const T* data);

			//! sets if the matrix is definitely identity matrix
			void setDefinitelyIdentityMatrix( bool isDefinitelyIdentityMatrix);

			//! gets if the matrix is definitely identity matrix
			bool getDefinitelyIdentityMatrix() const;

		private:
			//! Matrix data, stored in row-major order
			T M[16];
			mutable bool definitelyIdentityMatrix;
	};

	template <class T>
	inline CMatrix4<T>::CMatrix4( eConstructor constructor ) : definitelyIdentityMatrix(false)
	{
		switch ( constructor )
		{
			case EM4CONST_NOTHING:
			case EM4CONST_COPY:
				break;
			case EM4CONST_IDENTITY:
			case EM4CONST_INVERSE:
			default:
				makeIdentity();
				break;
		}
	}

	template <class T>
	inline CMatrix4<T>::CMatrix4( const CMatrix4<T>& other, eConstructor constructor) : definitelyIdentityMatrix(false)
	{
		switch ( constructor )
		{
			case EM4CONST_IDENTITY:
				makeIdentity();
				break;
			case EM4CONST_NOTHING:
				break;
			case EM4CONST_COPY:
				*this = other;
				break;
			case EM4CONST_TRANSPOSED:
				other.getTransposed(*this);
				break;
			case EM4CONST_INVERSE:
				if (!other.getInverse(*this))
					memset(M, 0, 16*sizeof(T));
				break;
			case EM4CONST_INVERSE_TRANSPOSED:
				if (!other.getInverse(*this))
					memset(M, 0, 16*sizeof(T));
				else
					*this=getTransposed();
				break;
		}
	}

	//! Add another matrix.
	template <class T>
	inline CMatrix4<T> CMatrix4<T>::operator+(const CMatrix4<T>& other) const
	{
		CMatrix4<T> temp ( EM4CONST_NOTHING );

		temp[0] = M[0]+other[0];
		temp[1] = M[1]+other[1];
		temp[2] = M[2]+other[2];
		temp[3] = M[3]+other[3];
		temp[4] = M[4]+other[4];
		temp[5] = M[5]+other[5];
		temp[6] = M[6]+other[6];
		temp[7] = M[7]+other[7];
		temp[8] = M[8]+other[8];
		temp[9] = M[9]+other[9];
		temp[10] = M[10]+other[10];
		temp[11] = M[11]+other[11];
		temp[12] = M[12]+other[12];
		temp[13] = M[13]+other[13];
		temp[14] = M[14]+other[14];
		temp[15] = M[15]+other[15];

		return temp;
	}

	//! Add another matrix.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::operator+=(const CMatrix4<T>& other)
	{
		M[0]+=other[0];
		M[1]+=other[1];
		M[2]+=other[2];
		M[3]+=other[3];
		M[4]+=other[4];
		M[5]+=other[5];
		M[6]+=other[6];
		M[7]+=other[7];
		M[8]+=other[8];
		M[9]+=other[9];
		M[10]+=other[10];
		M[11]+=other[11];
		M[12]+=other[12];
		M[13]+=other[13];
		M[14]+=other[14];
		M[15]+=other[15];

		return *this;
	}

	//! Subtract another matrix.
	template <class T>
	inline CMatrix4<T> CMatrix4<T>::operator-(const CMatrix4<T>& other) const
	{
		CMatrix4<T> temp ( EM4CONST_NOTHING );

		temp[0] = M[0]-other[0];
		temp[1] = M[1]-other[1];
		temp[2] = M[2]-other[2];
		temp[3] = M[3]-other[3];
		temp[4] = M[4]-other[4];
		temp[5] = M[5]-other[5];
		temp[6] = M[6]-other[6];
		temp[7] = M[7]-other[7];
		temp[8] = M[8]-other[8];
		temp[9] = M[9]-other[9];
		temp[10] = M[10]-other[10];
		temp[11] = M[11]-other[11];
		temp[12] = M[12]-other[12];
		temp[13] = M[13]-other[13];
		temp[14] = M[14]-other[14];
		temp[15] = M[15]-other[15];

		return temp;
	}

	//! Subtract another matrix.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::operator-=(const CMatrix4<T>& other)
	{
		M[0]-=other[0];
		M[1]-=other[1];
		M[2]-=other[2];
		M[3]-=other[3];
		M[4]-=other[4];
		M[5]-=other[5];
		M[6]-=other[6];
		M[7]-=other[7];
		M[8]-=other[8];
		M[9]-=other[9];
		M[10]-=other[10];
		M[11]-=other[11];
		M[12]-=other[12];
		M[13]-=other[13];
		M[14]-=other[14];
		M[15]-=other[15];

		return *this;
	}

	//! Multiply by scalar.
	template <class T>
	inline CMatrix4<T> CMatrix4<T>::operator*(const T& scalar) const
	{
		CMatrix4<T> temp ( EM4CONST_NOTHING );

		temp[0] = M[0]*scalar;
		temp[1] = M[1]*scalar;
		temp[2] = M[2]*scalar;
		temp[3] = M[3]*scalar;
		temp[4] = M[4]*scalar;
		temp[5] = M[5]*scalar;
		temp[6] = M[6]*scalar;
		temp[7] = M[7]*scalar;
		temp[8] = M[8]*scalar;
		temp[9] = M[9]*scalar;
		temp[10] = M[10]*scalar;
		temp[11] = M[11]*scalar;
		temp[12] = M[12]*scalar;
		temp[13] = M[13]*scalar;
		temp[14] = M[14]*scalar;
		temp[15] = M[15]*scalar;

		return temp;
	}

	//! Multiply by scalar.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::operator*=(const T& scalar)
	{
		M[0]*=scalar;
		M[1]*=scalar;
		M[2]*=scalar;
		M[3]*=scalar;
		M[4]*=scalar;
		M[5]*=scalar;
		M[6]*=scalar;
		M[7]*=scalar;
		M[8]*=scalar;
		M[9]*=scalar;
		M[10]*=scalar;
		M[11]*=scalar;
		M[12]*=scalar;
		M[13]*=scalar;
		M[14]*=scalar;
		M[15]*=scalar;

		return *this;
	}

	//! Multiply by another matrix.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::operator*=(const CMatrix4<T>& other)
	{
		// do checks on your own in order to avoid copy creation
		if ( !other.isIdentity() )
		{
			if ( this->isIdentity() )
			{
				return (*this = other);
			}
			else
			{
				CMatrix4<T> temp ( *this );
				return setbyproduct_nocheck( temp, other );
			}
		}
		return *this;
	}

	//! multiply by another matrix
	// set this matrix to the product of two other matrices
	// goal is to reduce stack use and copy
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setbyproduct_nocheck(const CMatrix4<T>& other_a,const CMatrix4<T>& other_b )
	{
		const T *m1 = other_a.M;
		const T *m2 = other_b.M;

		M[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
		M[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
		M[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
		M[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

		M[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
		M[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
		M[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
		M[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

		M[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
		M[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
		M[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
		M[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

		M[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
		M[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
		M[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
		M[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
		definitelyIdentityMatrix=false;
		return *this;
	}


	//! multiply by another matrix
	// set this matrix to the product of two other matrices
	// goal is to reduce stack use and copy
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setbyproduct(const CMatrix4<T>& other_a, const CMatrix4<T>& other_b )
	{
		if ( other_a.isIdentity () )
			return (*this = other_b);
		else
		if ( other_b.isIdentity () )
			return (*this = other_a);
		else
			return setbyproduct_nocheck(other_a,other_b);
	}

	//! multiply by another matrix
	template <class T>
	inline CMatrix4<T> CMatrix4<T>::operator*(const CMatrix4<T>& m2) const
	{
		// Testing purpose..
		if ( this->isIdentity() )
			return m2;
		if ( m2.isIdentity() )
			return *this;

		CMatrix4<T> m3 ( EM4CONST_NOTHING );

		const T *m1 = M;

		m3[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
		m3[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
		m3[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
		m3[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

		m3[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
		m3[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
		m3[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
		m3[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

		m3[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
		m3[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
		m3[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
		m3[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

		m3[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
		m3[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
		m3[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
		m3[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
		return m3;
	}



	template <class T>
	inline vector3d<T> CMatrix4<T>::getTranslation() const
	{
		return vector3d<T>(M[12], M[13], M[14]);
	}


	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setTranslation( const vector3d<T>& translation )
	{
		M[12] = translation.X;
		M[13] = translation.Y;
		M[14] = translation.Z;
		definitelyIdentityMatrix=false;
		return *this;
	}

	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setInverseTranslation( const vector3d<T>& translation )
	{
		M[12] = -translation.X;
		M[13] = -translation.Y;
		M[14] = -translation.Z;
		definitelyIdentityMatrix=false;
		return *this;
	}

	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setScale( const vector3d<T>& scale )
	{
		M[0] = scale.X;
		M[5] = scale.Y;
		M[10] = scale.Z;
		definitelyIdentityMatrix=false;
		return *this;
	}

	template <class T>
	inline vector3d<T> CMatrix4<T>::getScale() const
	{
		return vector3d<T>(M[0],M[5],M[10]);
	}

	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setRotationDegrees( const vector3d<T>& rotation )
	{
		return setRotationRadians( rotation * core::DEGTORAD );
	}

	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setInverseRotationDegrees( const vector3d<T>& rotation )
	{
		return setInverseRotationRadians( rotation * core::DEGTORAD );
	}

	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setRotationRadians( const vector3d<T>& rotation )
	{
		const f64 cr = cos( rotation.X );
		const f64 sr = sin( rotation.X );
		const f64 cp = cos( rotation.Y );
		const f64 sp = sin( rotation.Y );
		const f64 cy = cos( rotation.Z );
		const f64 sy = sin( rotation.Z );

		M[0] = (T)( cp*cy );
		M[1] = (T)( cp*sy );
		M[2] = (T)( -sp );

		const f64 srsp = sr*sp;
		const f64 crsp = cr*sp;

		M[4] = (T)( srsp*cy-cr*sy );
		M[5] = (T)( srsp*sy+cr*cy );
		M[6] = (T)( sr*cp );

		M[8] = (T)( crsp*cy+sr*sy );
		M[9] = (T)( crsp*sy-sr*cy );
		M[10] = (T)( cr*cp );
		definitelyIdentityMatrix=false;
		return *this;
	}


	//! Returns the rotation, as set by setRotation(). This code was sent
	//! in by Chev.
	template <class T>
	inline core::vector3d<T> CMatrix4<T>::getRotationDegrees() const
	{
		const CMatrix4<T> &mat = *this;

		f64 Y = -asin(mat(0,2));
		const f64 C = cos(Y);
		Y *= RADTODEG64;

		f64 rotx, roty, X, Z;

		if (fabs(C)>ROUNDING_ERROR_64)
		{
			const T invC = 1.0/C;
			rotx = mat(2,2) * invC;
			roty = mat(1,2) * invC;
			X = atan2( roty, rotx ) * RADTODEG64;
			rotx = mat(0,0) * invC;
			roty = mat(0,1) * invC;
			Z = atan2( roty, rotx ) * RADTODEG64;
		}
		else
		{
			X = 0.0;
			rotx = mat(1,1);
			roty = -mat(1,0);
			Z = atan2( roty, rotx ) * RADTODEG64;
		}

		// fix values that get below zero
		// before it would set (!) values to 360
		// that where above 360:
		if (X < 0.0) X += 360.0;
		if (Y < 0.0) Y += 360.0;
		if (Z < 0.0) Z += 360.0;

		return vector3d<T>((T)X,(T)Y,(T)Z);
	}


	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setInverseRotationRadians( const vector3d<T>& rotation )
	{
		f64 cr = cos( rotation.X );
		f64 sr = sin( rotation.X );
		f64 cp = cos( rotation.Y );
		f64 sp = sin( rotation.Y );
		f64 cy = cos( rotation.Z );
		f64 sy = sin( rotation.Z );

		M[0] = (T)( cp*cy );
		M[4] = (T)( cp*sy );
		M[8] = (T)( -sp );

		f64 srsp = sr*sp;
		f64 crsp = cr*sp;

		M[1] = (T)( srsp*cy-cr*sy );
		M[5] = (T)( srsp*sy+cr*cy );
		M[9] = (T)( sr*cp );

		M[2] = (T)( crsp*cy+sr*sy );
		M[6] = (T)( crsp*sy-sr*cy );
		M[10] = (T)( cr*cp );
		definitelyIdentityMatrix=false;
		return *this;
	}


	/*!
	*/
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::makeIdentity()
	{
		memset(M, 0, 16*sizeof(T));
		M[0] = M[5] = M[10] = M[15] = (T)1;
		definitelyIdentityMatrix=true;
		return *this;
	}


	/*
		check identity with epsilon
		solve floating range problems..
	*/
	template <class T>
	inline bool CMatrix4<T>::isIdentity() const
	{
		if (definitelyIdentityMatrix)
			return true;
		if (	!equals ( M[ 0], (T)1 ) ||
				!equals ( M[ 5], (T)1 ) ||
				!equals ( M[10], (T)1 ) ||
				!equals ( M[15], (T)1 )
			)
			return false;

		for (s32 i=0; i<4; ++i)
			for (s32 j=0; j<4; ++j)
				if ((j != i) && (!iszero((*this)(i,j))))
					return false;

		definitelyIdentityMatrix=true;
		return true;
	}

	/*
		doesn't solve floating range problems..
		but takes care on +/- 0 on translation because we are changing it..
		reducing floating point branches
		but it needs the floats in memory..
	*/
	template <class T>
	inline bool CMatrix4<T>::isIdentity_integer_base() const
	{
		if (definitelyIdentityMatrix)
			return true;
		if(IR(M[0])!=F32_VALUE_1)	return false;
		if(IR(M[1])!=0)			return false;
		if(IR(M[2])!=0)			return false;
		if(IR(M[3])!=0)			return false;

		if(IR(M[4])!=0)			return false;
		if(IR(M[5])!=F32_VALUE_1)	return false;
		if(IR(M[6])!=0)			return false;
		if(IR(M[7])!=0)			return false;

		if(IR(M[8])!=0)			return false;
		if(IR(M[9])!=0)			return false;
		if(IR(M[10])!=F32_VALUE_1)	return false;
		if(IR(M[11])!=0)		return false;

		if(IR(M[12])!=0)		return false;
		if(IR(M[13])!=0)		return false;
		if(IR(M[13])!=0)		return false;
		if(IR(M[15])!=F32_VALUE_1)	return false;
		definitelyIdentityMatrix=true;
		return true;
	}


	template <class T>
	inline void CMatrix4<T>::rotateVect( vector3df& vect ) const
	{
		vector3df tmp = vect;
		vect.X = tmp.X*M[0] + tmp.Y*M[4] + tmp.Z*M[8];
		vect.Y = tmp.X*M[1] + tmp.Y*M[5] + tmp.Z*M[9];
		vect.Z = tmp.X*M[2] + tmp.Y*M[6] + tmp.Z*M[10];
	}

	//! An alternate transform vector method, writing into a second vector
	template <class T>
	inline void CMatrix4<T>::rotateVect(core::vector3df& out, const core::vector3df& in) const
	{
		out.X = in.X*M[0] + in.Y*M[4] + in.Z*M[8];
		out.Y = in.X*M[1] + in.Y*M[5] + in.Z*M[9];
		out.Z = in.X*M[2] + in.Y*M[6] + in.Z*M[10];
	}

	//! An alternate transform vector method, writing into an array of 3 floats
	template <class T>
	inline void CMatrix4<T>::rotateVect(T *out, const core::vector3df& in) const
	{
		out[0] = in.X*M[0] + in.Y*M[4] + in.Z*M[8];
		out[1] = in.X*M[1] + in.Y*M[5] + in.Z*M[9];
		out[2] = in.X*M[2] + in.Y*M[6] + in.Z*M[10];
	}

	template <class T>
	inline void CMatrix4<T>::inverseRotateVect( vector3df& vect ) const
	{
		vector3df tmp = vect;
		vect.X = tmp.X*M[0] + tmp.Y*M[1] + tmp.Z*M[2];
		vect.Y = tmp.X*M[4] + tmp.Y*M[5] + tmp.Z*M[6];
		vect.Z = tmp.X*M[8] + tmp.Y*M[9] + tmp.Z*M[10];
	}

	template <class T>
	inline void CMatrix4<T>::transformVect( vector3df& vect) const
	{
		f32 vector[3];

		vector[0] = vect.X*M[0] + vect.Y*M[4] + vect.Z*M[8] + M[12];
		vector[1] = vect.X*M[1] + vect.Y*M[5] + vect.Z*M[9] + M[13];
		vector[2] = vect.X*M[2] + vect.Y*M[6] + vect.Z*M[10] + M[14];

		vect.X = vector[0];
		vect.Y = vector[1];
		vect.Z = vector[2];
	}

	template <class T>
	inline void CMatrix4<T>::transformVect( vector3df& out, const vector3df& in) const
	{
		out.X = in.X*M[0] + in.Y*M[4] + in.Z*M[8] + M[12];
		out.Y = in.X*M[1] + in.Y*M[5] + in.Z*M[9] + M[13];
		out.Z = in.X*M[2] + in.Y*M[6] + in.Z*M[10] + M[14];
	}


	template <class T>
	inline void CMatrix4<T>::transformVect(T *out,const vector3df &in) const
	{
		out[0] = in.X*M[0] + in.Y*M[4] + in.Z*M[8] + M[12];
		out[1] = in.X*M[1] + in.Y*M[5] + in.Z*M[9] + M[13];
		out[2] = in.X*M[2] + in.Y*M[6] + in.Z*M[10] + M[14];
		out[3] = in.X*M[3] + in.Y*M[7] + in.Z*M[11] + M[15];
	}


	//! Transforms a plane by this matrix
	template <class T>
	inline void CMatrix4<T>::transformPlane( core::plane3d<f32> &plane) const
	{
		vector3df member;
		transformVect(member, plane.getMemberPoint());

		vector3df origin(0,0,0);
		transformVect(plane.Normal);
		transformVect(origin);

		plane.Normal -= origin;
		plane.D = - member.dotProduct(plane.Normal);
	}

	//! Transforms a plane by this matrix
	template <class T>
	inline void CMatrix4<T>::transformPlane_new( core::plane3d<f32> &plane) const
	{
		// rotate normal -> rotateVect ( plane.n );
		vector3df n;
		n.X = plane.Normal.X*M[0] + plane.Normal.Y*M[4] + plane.Normal.Z*M[8];
		n.Y = plane.Normal.X*M[1] + plane.Normal.Y*M[5] + plane.Normal.Z*M[9];
		n.Z = plane.Normal.X*M[2] + plane.Normal.Y*M[6] + plane.Normal.Z*M[10];

		// compute new d. -> getTranslation(). dotproduct ( plane.n )
		plane.D -= M[12] * n.X + M[13] * n.Y + M[14] * n.Z;
		plane.Normal.X = n.X;
		plane.Normal.Y = n.Y;
		plane.Normal.Z = n.Z;
	}

	//! Transforms a plane by this matrix
	template <class T>
	inline void CMatrix4<T>::transformPlane( const core::plane3d<f32> &in, core::plane3d<f32> &out) const
	{
		out = in;
		transformPlane( out );
	}

	//! Transforms a axis aligned bounding box
	template <class T>
	inline void CMatrix4<T>::transformBox(core::aabbox3d<f32>& box) const
	{
		if (isIdentity() )
			return;

		transformVect(box.MinEdge);
		transformVect(box.MaxEdge);
		box.repair();
	}

	//! Transforms a axis aligned bounding box more accurately than transformBox()
	template <class T>
	inline void CMatrix4<T>::transformBoxEx(core::aabbox3d<f32>& box) const
	{
		f32 Amin[3];
		f32 Amax[3];
		f32 Bmin[3];
		f32 Bmax[3];

		Amin[0] = box.MinEdge.X;
		Amin[1] = box.MinEdge.Y;
		Amin[2] = box.MinEdge.Z;

		Amax[0] = box.MaxEdge.X;
		Amax[1] = box.MaxEdge.Y;
		Amax[2] = box.MaxEdge.Z;

		Bmin[0] = Bmax[0] = M[12];
		Bmin[1] = Bmax[1] = M[13];
		Bmin[2] = Bmax[2] = M[14];

		u32 i, j;
		const CMatrix4<T> &m = *this;

		for (i = 0; i < 3; ++i)
		{
			for (j = 0; j < 3; ++j)
			{
				f32 a = m(j,i) * Amin[j];
				f32 b = m(j,i) * Amax[j];

				if (a < b)
				{
					Bmin[i] += a;
					Bmax[i] += b;
				}
				else
				{
					Bmin[i] += b;
					Bmax[i] += a;
				}
			}
		}

		box.MinEdge.X = Bmin[0];
		box.MinEdge.Y = Bmin[1];
		box.MinEdge.Z = Bmin[2];

		box.MaxEdge.X = Bmax[0];
		box.MaxEdge.Y = Bmax[1];
		box.MaxEdge.Z = Bmax[2];
	}


	//! Multiplies this matrix by a 1x4 matrix
	template <class T>
	inline void CMatrix4<T>::multiplyWith1x4Matrix(T* matrix) const
	{
		/*
		0  1  2  3
		4  5  6  7
		8  9  10 11
		12 13 14 15
		*/

		T mat[4];
		mat[0] = matrix[0];
		mat[1] = matrix[1];
		mat[2] = matrix[2];
		mat[3] = matrix[3];

		matrix[0] = M[0]*mat[0] + M[4]*mat[1] + M[8]*mat[2] + M[12]*mat[3];
		matrix[1] = M[1]*mat[0] + M[5]*mat[1] + M[9]*mat[2] + M[13]*mat[3];
		matrix[2] = M[2]*mat[0] + M[6]*mat[1] + M[10]*mat[2] + M[14]*mat[3];
		matrix[3] = M[3]*mat[0] + M[7]*mat[1] + M[11]*mat[2] + M[15]*mat[3];
	}

	template <class T>
	inline void CMatrix4<T>::inverseTranslateVect( vector3df& vect ) const
	{
		vect.X = vect.X-M[12];
		vect.Y = vect.Y-M[13];
		vect.Z = vect.Z-M[14];
	}

	template <class T>
	inline void CMatrix4<T>::translateVect( vector3df& vect ) const
	{
		vect.X = vect.X+M[12];
		vect.Y = vect.Y+M[13];
		vect.Z = vect.Z+M[14];
	}


	template <class T>
	inline bool CMatrix4<T>::getInverse(CMatrix4<T>& out) const
	{
		/// Calculates the inverse of this Matrix
		/// The inverse is calculated using Cramers rule.
		/// If no inverse exists then 'false' is returned.

		if ( this->isIdentity() )
		{
			out=*this;
			return true;
		}

		const CMatrix4<T> &m = *this;

		f32 d = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) -
			(m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
			(m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)) +
			(m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) -
			(m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
			(m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0));

		if( core::iszero ( d ) )
			return false;

		d = core::reciprocal ( d );

		out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) +
				m(1, 2) * (m(2, 3) * m(3, 1) - m(2, 1) * m(3, 3)) +
				m(1, 3) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)));
		out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2)) +
				m(2, 2) * (m(0, 3) * m(3, 1) - m(0, 1) * m(3, 3)) +
				m(2, 3) * (m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1)));
		out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) +
				m(3, 2) * (m(0, 3) * m(1, 1) - m(0, 1) * m(1, 3)) +
				m(3, 3) * (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)));
		out(0, 3) = d * (m(0, 1) * (m(1, 3) * m(2, 2) - m(1, 2) * m(2, 3)) +
				m(0, 2) * (m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1)) +
				m(0, 3) * (m(1, 2) * m(2, 1) - m(1, 1) * m(2, 2)));
		out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) +
				m(1, 3) * (m(2, 2) * m(3, 0) - m(2, 0) * m(3, 2)) +
				m(1, 0) * (m(2, 3) * m(3, 2) - m(2, 2) * m(3, 3)));
		out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(0, 3) * m(3, 0)) +
				m(2, 3) * (m(0, 2) * m(3, 0) - m(0, 0) * m(3, 2)) +
				m(2, 0) * (m(0, 3) * m(3, 2) - m(0, 2) * m(3, 3)));
		out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) +
				m(3, 3) * (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) +
				m(3, 0) * (m(0, 3) * m(1, 2) - m(0, 2) * m(1, 3)));
		out(1, 3) = d * (m(0, 2) * (m(1, 3) * m(2, 0) - m(1, 0) * m(2, 3)) +
				m(0, 3) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
				m(0, 0) * (m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2)));
		out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0)) +
				m(1, 0) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
				m(1, 1) * (m(2, 3) * m(3, 0) - m(2, 0) * m(3, 3)));
		out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(0, 1) * m(3, 0)) +
				m(2, 0) * (m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1)) +
				m(2, 1) * (m(0, 3) * m(3, 0) - m(0, 0) * m(3, 3)));
		out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) +
				m(3, 0) * (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) +
				m(3, 1) * (m(0, 3) * m(1, 0) - m(0, 0) * m(1, 3)));
		out(2, 3) = d * (m(0, 3) * (m(1, 1) * m(2, 0) - m(1, 0) * m(2, 1)) +
				m(0, 0) * (m(1, 3) * m(2, 1) - m(1, 1) * m(2, 3)) +
				m(0, 1) * (m(1, 0) * m(2, 3) - m(1, 3) * m(2, 0)));
		out(3, 0) = d * (m(1, 0) * (m(2, 2) * m(3, 1) - m(2, 1) * m(3, 2)) +
				m(1, 1) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
				m(1, 2) * (m(2, 1) * m(3, 0) - m(2, 0) * m(3, 1)));
		out(3, 1) = d * (m(2, 0) * (m(0, 2) * m(3, 1) - m(0, 1) * m(3, 2)) +
				m(2, 1) * (m(0, 0) * m(3, 2) - m(0, 2) * m(3, 0)) +
				m(2, 2) * (m(0, 1) * m(3, 0) - m(0, 0) * m(3, 1)));
		out(3, 2) = d * (m(3, 0) * (m(0, 2) * m(1, 1) - m(0, 1) * m(1, 2)) +
				m(3, 1) * (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) +
				m(3, 2) * (m(0, 1) * m(1, 0) - m(0, 0) * m(1, 1)));
		out(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) +
				m(0, 1) * (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) +
				m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)));
		out.definitelyIdentityMatrix = definitelyIdentityMatrix;
		return true;
	}


	//! Inverts a primitive matrix which only contains a translation and a rotation
	//! \param out: where result matrix is written to.
	template <class T>
	inline bool CMatrix4<T>::getInversePrimitive ( CMatrix4<T>& out ) const
	{
		out.M[0 ]	= M[0];
		out.M[1 ]	= M[4];
		out.M[2 ]	= M[8];
		out.M[3 ]	= 0;

		out.M[4 ]	= M[1];
		out.M[5 ]	= M[5];
		out.M[6 ]	= M[9];
		out.M[7 ]	= 0;

		out.M[8 ]	= M[2];
		out.M[9 ]	= M[6];
		out.M[10]	= M[10];
		out.M[11]	= 0;

		out.M[12]	= (T)-(M[12]*M[0] + M[13]*M[1] + M[14]*M[2]);
		out.M[13]	= (T)-(M[12]*M[4] + M[13]*M[5] + M[14]*M[6]);
		out.M[14]	= (T)-(M[12]*M[8] + M[13]*M[9] + M[14]*M[10]);
		out.M[15]	= 1;
		out.definitelyIdentityMatrix = definitelyIdentityMatrix;
		return true;
	}

	/*!
	*/
	template <class T>
	inline bool CMatrix4<T>::makeInverse()
	{
		if (definitelyIdentityMatrix)
			return true;

		CMatrix4<T> temp ( EM4CONST_NOTHING );

		if (getInverse(temp))
		{
			*this = temp;
			return true;
		}

		return false;
	}



	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::operator=(const CMatrix4<T> &other)
	{
		if (this==&other)
			return *this;
		memcpy(M, other.M, 16*sizeof(T));
		definitelyIdentityMatrix=other.definitelyIdentityMatrix;
		return *this;
	}



	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::operator=(const T& scalar)
	{
		for (s32 i = 0; i < 16; ++i)
			M[i]=scalar;
		definitelyIdentityMatrix=false;
		return *this;
	}



	template <class T>
	inline bool CMatrix4<T>::operator==(const CMatrix4<T> &other) const
	{
		if (definitelyIdentityMatrix && other.definitelyIdentityMatrix)
			return true;
		for (s32 i = 0; i < 16; ++i)
			if (M[i] != other.M[i])
				return false;

		return true;
	}



	template <class T>
	inline bool CMatrix4<T>::operator!=(const CMatrix4<T> &other) const
	{
		return !(*this == other);
	}



	//! Builds a right-handed perspective projection matrix based on a field of view
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveFovRH(
			f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar)
	{
		const f64 h = 1.0/tan(fieldOfViewRadians/2.0);
		const T w = h / aspectRatio;

		M[0] = w;
		M[1] = 0;
		M[2] = 0;
		M[3] = 0;

		M[4] = 0;
		M[5] = (T)h;
		M[6] = 0;
		M[7] = 0;

		M[8] = 0;
		M[9] = 0;
		M[10] = (T)(zFar/(zNear-zFar)); // DirectX version
//		M[10] = (T)(zFar+zNear/(zNear-zFar)); // OpenGL version
		M[11] = -1;

		M[12] = 0;
		M[13] = 0;
		M[14] = (T)(zNear*zFar/(zNear-zFar)); // DirectX version
//		M[14] = (T)(2.0f*zNear*zFar/(zNear-zFar)); // OpenGL version
		M[15] = 0;
		definitelyIdentityMatrix=false;
		return *this;
	}



	//! Builds a left-handed perspective projection matrix based on a field of view
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveFovLH(
			f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar)
	{
		const f64 h = 1.0/tan(fieldOfViewRadians/2.0);
		const T w = (T)(h / aspectRatio);

		M[0] = w;
		M[1] = 0;
		M[2] = 0;
		M[3] = 0;

		M[4] = 0;
		M[5] = (T)h;
		M[6] = 0;
		M[7] = 0;

		M[8] = 0;
		M[9] = 0;
		M[10] = (T)(zFar/(zFar-zNear));
		M[11] = 1;

		M[12] = 0;
		M[13] = 0;
		M[14] = (T)(-zNear*zFar/(zFar-zNear));
		M[15] = 0;
		definitelyIdentityMatrix=false;
		return *this;
	}


	//! Builds a left-handed orthogonal projection matrix.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoLH(
			f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar)
	{
		M[0] = (T)(2/widthOfViewVolume);
		M[1] = 0;
		M[2] = 0;
		M[3] = 0;

		M[4] = 0;
		M[5] = (T)(2/heightOfViewVolume);
		M[6] = 0;
		M[7] = 0;

		M[8] = 0;
		M[9] = 0;
		M[10] = (T)(1/(zFar-zNear));
		M[11] = 0;

		M[12] = 0;
		M[13] = 0;
		M[14] = (T)(zNear/(zNear-zFar));
		M[15] = 1;
		definitelyIdentityMatrix=false;
		return *this;
	}



	//! Builds a right-handed orthogonal projection matrix.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoRH(
			f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar)
	{
		M[0] = (T)(2/widthOfViewVolume);
		M[1] = 0;
		M[2] = 0;
		M[3] = 0;

		M[4] = 0;
		M[5] = (T)(2/heightOfViewVolume);
		M[6] = 0;
		M[7] = 0;

		M[8] = 0;
		M[9] = 0;
		M[10] = (T)(1/(zNear-zFar));
		M[11] = 0;

		M[12] = 0;
		M[13] = 0;
		M[14] = (T)(zNear/(zNear-zFar));
		M[15] = -1;
		definitelyIdentityMatrix=false;
		return *this;
	}


	//! Builds a right-handed perspective projection matrix.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveRH(
			f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar)
	{
		M[0] = (T)(2*zNear/widthOfViewVolume);
		M[1] = 0;
		M[2] = 0;
		M[3] = 0;

		M[4] = 0;
		M[5] = (T)(2*zNear/heightOfViewVolume);
		M[6] = 0;
		M[7] = 0;

		M[8] = 0;
		M[9] = 0;
		M[10] = (T)(zFar/(zNear-zFar));
		M[11] = -1;

		M[12] = 0;
		M[13] = 0;
		M[14] = (T)(zNear*zFar/(zNear-zFar));
		M[15] = 0;
		definitelyIdentityMatrix=false;
		return *this;
	}


	//! Builds a left-handed perspective projection matrix.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveLH(
			f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar)
	{
		M[0] = (T)(2*zNear/widthOfViewVolume);
		M[1] = 0;
		M[2] = 0;
		M[3] = 0;

		M[4] = 0;
		M[5] = (T)(2*zNear/heightOfViewVolume);
		M[6] = 0;
		M[7] = 0;

		M[8] = 0;
		M[9] = 0;
		M[10] = (T)(zFar/(zFar-zNear));
		M[11] = 1;

		M[12] = 0;
		M[13] = 0;
		M[14] = (T)(zNear*zFar/(zNear-zFar));
		M[15] = 0;
		definitelyIdentityMatrix=false;
		return *this;
	}


	//! Builds a matrix that flattens geometry into a plane.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildShadowMatrix(const core::vector3df& light, core::plane3df plane, f32 point)
	{
		plane.Normal.normalize();
		const f32 d = plane.Normal.dotProduct(light);

		M[ 0] = (T)(-plane.Normal.X * light.X + d);
		M[ 1] = (T)(-plane.Normal.X * light.Y);
		M[ 2] = (T)(-plane.Normal.X * light.Z);
		M[ 3] = (T)(-plane.Normal.X * point);

		M[ 4] = (T)(-plane.Normal.Y * light.X);
		M[ 5] = (T)(-plane.Normal.Y * light.Y + d);
		M[ 6] = (T)(-plane.Normal.Y * light.Z);
		M[ 7] = (T)(-plane.Normal.Y * point);

		M[ 8] = (T)(-plane.Normal.Z * light.X);
		M[ 9] = (T)(-plane.Normal.Z * light.Y);
		M[10] = (T)(-plane.Normal.Z * light.Z + d);
		M[11] = (T)(-plane.Normal.Z * point);

		M[12] = (T)(-plane.D * light.X);
		M[13] = (T)(-plane.D * light.Y);
		M[14] = (T)(-plane.D * light.Z);
		M[15] = (T)(-plane.D * point + d);
		definitelyIdentityMatrix=false;
		return *this;
	}

	//! Builds a left-handed look-at matrix.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildCameraLookAtMatrixLH(
				const vector3df& position,
				const vector3df& target,
				const vector3df& upVector)
	{
		vector3df zaxis = target - position;
		zaxis.normalize();

		vector3df xaxis = upVector.crossProduct(zaxis);
		xaxis.normalize();

		vector3df yaxis = zaxis.crossProduct(xaxis);

		M[0] = (T)xaxis.X;
		M[1] = (T)yaxis.X;
		M[2] = (T)zaxis.X;
		M[3] = 0;

		M[4] = (T)xaxis.Y;
		M[5] = (T)yaxis.Y;
		M[6] = (T)zaxis.Y;
		M[7] = 0;

		M[8] = (T)xaxis.Z;
		M[9] = (T)yaxis.Z;
		M[10] = (T)zaxis.Z;
		M[11] = 0;

		M[12] = (T)-xaxis.dotProduct(position);
		M[13] = (T)-yaxis.dotProduct(position);
		M[14] = (T)-zaxis.dotProduct(position);
		M[15] = 1;
		definitelyIdentityMatrix=false;
		return *this;
	}



	//! Builds a right-handed look-at matrix.
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildCameraLookAtMatrixRH(
				const vector3df& position,
				const vector3df& target,
				const vector3df& upVector)
	{
		vector3df zaxis = position - target;
		zaxis.normalize();

		vector3df xaxis = upVector.crossProduct(zaxis);
		xaxis.normalize();

		vector3df yaxis = zaxis.crossProduct(xaxis);

		M[0] = (T)xaxis.X;
		M[1] = (T)yaxis.X;
		M[2] = (T)zaxis.X;
		M[3] = 0;

		M[4] = (T)xaxis.Y;
		M[5] = (T)yaxis.Y;
		M[6] = (T)zaxis.Y;
		M[7] = 0;

		M[8] = (T)xaxis.Z;
		M[9] = (T)yaxis.Z;
		M[10] = (T)zaxis.Z;
		M[11] = 0;

		M[12] = (T)-xaxis.dotProduct(position);
		M[13] = (T)-yaxis.dotProduct(position);
		M[14] = (T)-zaxis.dotProduct(position);
		M[15] = 1;
		definitelyIdentityMatrix=false;
		return *this;
	}


	//! creates a new matrix as interpolated matrix from this and the passed one.
	//! \param b: Second matrix to interpolate with
	//! \param time: Must be a value between 0 and 1.
	template <class T>
	inline CMatrix4<T> CMatrix4<T>::interpolate(const core::CMatrix4<T>& b, f32 time) const
	{
		CMatrix4<T> mat ( EM4CONST_NOTHING );

		for (u32 i=0; i < 16; i += 4)
		{
			mat.M[i+0] = (T)(M[i+0] + ( b.M[i+0] - M[i+0] ) * time);
			mat.M[i+1] = (T)(M[i+1] + ( b.M[i+1] - M[i+1] ) * time);
			mat.M[i+2] = (T)(M[i+2] + ( b.M[i+2] - M[i+2] ) * time);
			mat.M[i+3] = (T)(M[i+3] + ( b.M[i+3] - M[i+3] ) * time);
		}
		return mat;
	}

	//! returns transposed matrix
	template <class T>
	inline CMatrix4<T> CMatrix4<T>::getTransposed() const
	{
		CMatrix4<T> t ( EM4CONST_NOTHING );
		getTransposed ( t );
		return t;
	}

	//! returns transposed matrix
	template <class T>
	inline void CMatrix4<T>::getTransposed( CMatrix4<T>& o ) const
	{
		o[ 0] = M[ 0];
		o[ 1] = M[ 4];
		o[ 2] = M[ 8];
		o[ 3] = M[12];

		o[ 4] = M[ 1];
		o[ 5] = M[ 5];
		o[ 6] = M[ 9];
		o[ 7] = M[13];

		o[ 8] = M[ 2];
		o[ 9] = M[ 6];
		o[10] = M[10];
		o[11] = M[14];

		o[12] = M[ 3];
		o[13] = M[ 7];
		o[14] = M[11];
		o[15] = M[15];
		o.definitelyIdentityMatrix=definitelyIdentityMatrix;
	}


	// used to scale <-1,-1><1,1> to viewport
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildNDCToDCMatrix( const core::rect<s32>& viewport, f32 zScale)
	{
		const f32 scaleX = (viewport.getWidth() - 0.75f ) / 2.0f;
		const f32 scaleY = -(viewport.getHeight() - 0.75f ) / 2.0f;

		const f32 dx = -0.5f + ( (viewport.UpperLeftCorner.X + viewport.LowerRightCorner.X ) / 2.0f );
		const f32 dy = -0.5f + ( (viewport.UpperLeftCorner.Y + viewport.LowerRightCorner.Y ) / 2.0f );

		makeIdentity();
		M[12] = (T)dx;
		M[13] = (T)dy;
		return setScale(core::vector3d<T>((T)scaleX, (T)scaleY, (T)zScale));
	}

	/*!
		Generate texture coordinates as linear functions so that:
			u = Ux*x + Uy*y + Uz*z + Uw
			v = Vx*x + Vy*y + Vz*z + Vw
		The matrix M for this case is:
			Ux  Vx  0  0
			Uy  Vy  0  0
			Uz  Vz  0  0
			Uw  Vw  0  0
	*/

	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::buildTextureTransform( f32 rotateRad,
			const core::vector2df &rotatecenter,
			const core::vector2df &translate,
			const core::vector2df &scale)
	{
		const f32 c = cosf(rotateRad);
		const f32 s = sinf(rotateRad);

		M[0] = (T)(c * scale.X);
		M[1] = (T)(s * scale.Y);
		M[2] = 0;
		M[3] = 0;

		M[4] = (T)(-s * scale.X);
		M[5] = (T)(c * scale.Y);
		M[6] = 0;
		M[7] = 0;

		M[8] = (T)(c * scale.X * rotatecenter.X + -s * rotatecenter.Y + translate.X);
		M[9] = (T)(s * scale.Y * rotatecenter.X +  c * rotatecenter.Y + translate.Y);
		M[10] = 1;
		M[11] = 0;

		M[12] = 0;
		M[13] = 0;
		M[14] = 0;
		M[15] = 1;
		definitelyIdentityMatrix=false;
		return *this;
	}

	//! rotate about z axis, center ( 0.5, 0.5 )
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setTextureRotationCenter( f32 rotateRad )
	{
		const f32 c = cosf(rotateRad);
		const f32 s = sinf(rotateRad);
		M[0] = (T)c;
		M[1] = (T)s;

		M[4] = (T)-s;
		M[5] = (T)c;

		M[8] = (T)(0.5f * ( s - c) + 0.5f);
		M[9] = (T)(-0.5f * ( s + c) + 0.5f);
		definitelyIdentityMatrix = definitelyIdentityMatrix && (rotateRad==0.0f);
		return *this;
	}

	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setTextureTranslate ( f32 x, f32 y )
	{
		M[8] = (T)x;
		M[9] = (T)y;
		definitelyIdentityMatrix = definitelyIdentityMatrix && (x==0.0f) && (y==0.0f);
		return *this;
	}

	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setTextureScale ( f32 sx, f32 sy )
	{
		M[0] = (T)sx;
		M[5] = (T)sy;
		definitelyIdentityMatrix = definitelyIdentityMatrix && (sx==1.0f) && (sy==1.0f);
		return *this;
	}

	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setTextureScaleCenter( f32 sx, f32 sy )
	{
		M[0] = (T)sx;
		M[5] = (T)sy;
		M[8] = (T)(0.5f - 0.5f * sx);
		M[9] = (T)(0.5f - 0.5f * sy);
		definitelyIdentityMatrix = definitelyIdentityMatrix && (sx==1.0f) && (sy==1.0f);
		return *this;
	}

	//! sets all matrix data members at once
	template <class T>
	inline CMatrix4<T>& CMatrix4<T>::setM(const T* data)
	{
		memcpy(M,data, 16*sizeof(T));

		definitelyIdentityMatrix = false;
		return *this;
	}

	//! sets if the matrix is definitely identity matrix
	template <class T>
	inline void CMatrix4<T>::setDefinitelyIdentityMatrix( bool isDefinitelyIdentityMatrix)
	{
		definitelyIdentityMatrix = isDefinitelyIdentityMatrix;
	}

	//! gets if the matrix is definitely identity matrix
	template <class T>
	inline bool CMatrix4<T>::getDefinitelyIdentityMatrix() const
	{
		return definitelyIdentityMatrix;
	}

	//! Multiply by scalar.
	template <class T>
	inline CMatrix4<T> operator*(const T scalar, const CMatrix4<T>& mat)
	{
		return mat*scalar;
	}

	typedef CMatrix4<f32> matrix4;
	const matrix4 IdentityMatrix(matrix4::EM4CONST_IDENTITY);

} // end namespace core
} // end namespace irr

#endif

