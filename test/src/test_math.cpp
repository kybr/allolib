#include <math.h>

#include "al/core/math/al_Vec.hpp"
#include "al/core/math/al_Quat.hpp"
#include "al/core/math/al_Mat.hpp"
#include "al/core/math/al_Functions.hpp"
// #include "al/core/math/al_Random.hpp"
#include "al/util/al_Frustum.hpp"
#include "al/util/al_Complex.hpp"
#include "al/util/al_Interval.hpp"

using namespace al;

template <class T>
inline bool eqVal(T x, T y, T eps=0.000001){
	return std::abs(x-y) < eps;
}

template <class T>
inline bool eqPtr(const T* x, const T* y, int n, T eps=0.0000001){
	for(int i=0; i<n; ++i){
		if(!eqVal(x[i], y[i], eps)) return false;
	}
	return true;
}

template <class T>
inline bool eq(const Quat<T>& a, const Quat<T>& b, T eps=0.000001){
	return eqPtr(&a[0], &b[0], 4, eps);
}

template <int N, class T>
inline bool eq(const Vec<N,T>& a, const Vec<N,T>& b, T eps=0.000001){
	return eqPtr(&a[0], &b[0], N, eps);
}

template <int N, class T>
inline bool eq(const Mat<N,T>& a, const Mat<N,T>& b, T eps=0.000001){
	return eqPtr(&a[0], &b[0], N*N, eps);
}

// TODO break test into sections or separate tests as needed. Look at:
// https://github.com/philsquared/Catch/blob/master/docs/tutorial.md
TEST_CASE("Math"){
	// Vec
	{
		const int N = 4;

		// Should be able to hold objects with constructors
		{ Vec<1, Vec<1, int> > t; }
		{ Vec<4, Complex<float> > t; }
		{ Vec<5, char> t; }
		{ Vec<0,int> t; }

		//Vec<0,int>().print();
		//Vec<1,int>(1).print();
		//Vec<2,int>(1,2).print();
		//std::cout << Vec<0,int>();

		Vec<N,double> a, b, c;	REQUIRE(a.size() == N);

		a[0] = 0;				REQUIRE(a[0] == 0);
								REQUIRE(a.elems()[0] == 0);

		a.x = 1;				REQUIRE(a[0] == 1);
		a.y = 2;				REQUIRE(a[1] == 2);
		a.z = 3;				REQUIRE(a[2] == 3);
		a.w = 4;				REQUIRE(a[3] == 4);

		a.set(1);				REQUIRE(a == 1);
		b.set(a);				REQUIRE(b == 1);

		{
		a.set(1);
		b.set(0);
		double * p = a.elems();	REQUIRE(p[0] == a[0]);
		b.set(p);				REQUIRE(a == b);

		char c1[] = {4,4,4,4};
		a.set(c1);				REQUIRE(a == 4);

		char c2[] = {1,0,1,0,1,0,1,0};
		a.set(c2,2);			REQUIRE(a == 1);

		a.zero();
		a.set(Vec<N-1,int>(1,2,3), 4);
								REQUIRE(a[0] == 1); REQUIRE(a[1] == 2); REQUIRE(a[2] == 3); REQUIRE(a[3] == 4);
		}

		a.zero();				REQUIRE(a == 0);

		a = 1;					REQUIRE(a == 1);
								REQUIRE(!(a != 1));
		b = a;					REQUIRE(b == a);
								REQUIRE(!(b != a));
		a = 3;
		b = 3;
		a -= b;					REQUIRE(a == 0);
		a += b;					REQUIRE(a == 3);
		a -= 3;					REQUIRE(a == 0);
		a += 3;					REQUIRE(a == 3);

		a *= b;					REQUIRE(a == 9);
		a /= b;					REQUIRE(a == 3);
		a *= 3;					REQUIRE(a == 9);
		a /= 3;					REQUIRE(a == 3);

		a = b + b;				REQUIRE(a == 6);
		a = b - b;				REQUIRE(a == 0);
		a = b * b;				REQUIRE(a == 9);
		a = b / b;				REQUIRE(a == 1);

		a = 2. + a;				REQUIRE(a == 3);
		a = 6. - a;				REQUIRE(a == 3);
		a = 2. * a;				REQUIRE(a == 6);
//		a = 1. / a;				REQUIRE(a == 1./6);

		a = +1;
		b = -1;					REQUIRE(a == -b);

		a = -1;
		b = +1;
		REQUIRE(a.dot(b) ==-N);
		REQUIRE(a.dot(a) == N);
		REQUIRE(a.sum() == -N);
		REQUIRE(a.sumAbs() == N);
		REQUIRE(a.mag() == sqrt(N));
		REQUIRE(b.mag() == sqrt(N));
		REQUIRE(a.mag() == abs(a));
		REQUIRE(a.magSqr() == N);
		REQUIRE(b.magSqr() == N);
		REQUIRE(a.norm1() == N);
		REQUIRE(a.norm2() == sqrt(N));

		a.set(1).negate();		REQUIRE(a == -1);
		a.set(1).normalize();	REQUIRE(a == 1./sqrt(N));
		REQUIRE(a == b.set(10).normalized());

		b = a = 1;
		REQUIRE(concat(a,b) == 1);

		// conversion
		{
		a = 0;
		Vec<N+1, double> t = concat(a, Vec<1,char>(1));
		REQUIRE(t.size() == a.size()+1);
		}

		for(int i=0; i<a.size(); ++i) a[i]=i;
		REQUIRE(a.get(0,1) == Vec2d(0,1));
		REQUIRE(a.get(2,2) == Vec2d(2,2));
		REQUIRE(a.get(2,1,0) == Vec3d(2,1,0));

		{
		for(int i=0; i<a.size(); ++i) a[i]=i;

		Vec<2, double> t;
		t = a.sub<2>();			REQUIRE(t[0] == 0); REQUIRE(t[1] == 1);
		t = a.sub<2>(2);		REQUIRE(t[0] == 2); REQUIRE(t[1] == 3);
		}

		REQUIRE(eqVal(angle(Vec3d(1,0,0), Vec3d(1, 0, 0)), 0.));
		REQUIRE(eqVal(angle(Vec3d(1,0,0), Vec3d(0, 1, 0)), M_PI_2));
		REQUIRE(eqVal(angle(Vec3d(1,0,0), Vec3d(0,-1, 0)), M_PI_2));

		{
		Vec3d r;
		centroid3(r, Vec3d(1,0,0), Vec3d(0,1,0), Vec3d(0,0,1));
		REQUIRE( eq(r, Vec3d(1/3.)) );

		normal(r, Vec3d(1,0,0), Vec3d(0,1,0), Vec3d(-1,0,0));
		REQUIRE( eq(r, Vec3d(0,0,1)) );

		Vec3d pos(1,2,3);
		Vec3d to(4,5,6);
		Vec3d rel = to - pos;

		REQUIRE(rel[0]==3); REQUIRE(rel[1]==3); REQUIRE(rel[2]==3);

		}

		a = 0;
		b = 1;
		REQUIRE(min(a,b) == 0);
		REQUIRE(max(a,b) == 1);

	}


	// Vec3
	{
		Vec3d a, b, c;

		a.set(1,0,0);
		b.set(0,1,0);
		c.set(0,0,1);
			REQUIRE(c == cross(a,b));
			REQUIRE(c == a.cross(b));

		a = b;
			REQUIRE(a == b);
	}


	// Mat
	{
		/*#define CHECK2(mat, a,b,c, d,e,f, g,h,i){\
			const auto& m = mat;\
			REQUIRE(eq(m(0,0),a)); REQUIRE(eq(m(0,1),b)); REQUIRE(eq(m(0,2),c));\
			REQUIRE(eq(m(1,0),d)); REQUIRE(eq(m(1,1),e)); REQUIRE(eq(m(1,2),f));\
			REQUIRE(eq(m(2,0),g)); REQUIRE(eq(m(2,1),h)); REQUIRE(eq(m(2,2),i));\
		}*/
		#define CHECK2(mat, a,b,c, d,e,f, g,h,i)\
			REQUIRE(eq(mat, Mat3d(a,b,c, d,e,f, g,h,i)))

		// factory functions
		CHECK2(Mat3d::identity(),				1,0,0, 0,1,0, 0,0,1);
		CHECK2(Mat3d::scaling(2),				2,0,0, 0,2,0, 0,0,1);
		CHECK2(Mat3d::scaling(2,3),				2,0,0, 0,3,0, 0,0,1);
		CHECK2(Mat3d::scaling(Vec2d(2,3)),		2,0,0, 0,3,0, 0,0,1);
		CHECK2(Mat3d::translation(2,3),			1,0,2, 0,1,3, 0,0,1);
		CHECK2(Mat3d::translation(Vec2d(2,3)),	1,0,2, 0,1,3, 0,0,1);
		CHECK2(Mat3d::rotation(M_PI/2, 0,1),		0,-1,0,1,0,0, 0,0,1);

		Mat3d a;//, b;

		a.setIdentity();	CHECK2(a, 1,0,0, 0,1,0, 0,0,1);

		REQUIRE(a.trace() == 3);

		a += 2;		CHECK2(a, 3,2,2, 2,3,2, 2,2,3);
		a -= 1;		CHECK2(a, 2,1,1, 1,2,1, 1,1,2);
		a *= 2;		CHECK2(a, 4,2,2, 2,4,2, 2,2,4);
		a /= 2;		CHECK2(a, 2,1,1, 1,2,1, 1,1,2);

		a.setIdentity();
		a = a+2;	CHECK2(a, 3,2,2, 2,3,2, 2,2,3);
		a = a-1;	CHECK2(a, 2,1,1, 1,2,1, 1,1,2);
		a = a*2;	CHECK2(a, 4,2,2, 2,4,2, 2,2,4);
		a = a/2;	CHECK2(a, 2,1,1, 1,2,1, 1,1,2);

		a.setIdentity();
		a = 2.+a;	CHECK2(a, 3,2,2, 2,3,2, 2,2,3);
		a = 4.-a;	CHECK2(a, 1,2,2, 2,1,2, 2,2,1);
		a = 2.*a;	CHECK2(a, 2,4,4, 4,2,4, 4,4,2);

		a.set(	1,2,3,
				4,5,6,
				7,8,9
		);

		REQUIRE(a.col(0) == Vec3d(1,4,7));
		REQUIRE(a.col(1) == Vec3d(2,5,8));
		REQUIRE(a.col(2) == Vec3d(3,6,9));
		REQUIRE(a.row(0) == Vec3d(1,2,3));
		REQUIRE(a.row(1) == Vec3d(4,5,6));
		REQUIRE(a.row(2) == Vec3d(7,8,9));

		a.transpose();

		REQUIRE(a.col(0) == Vec3d(1,2,3));
		REQUIRE(a.col(1) == Vec3d(4,5,6));
		REQUIRE(a.col(2) == Vec3d(7,8,9));
		REQUIRE(a.row(0) == Vec3d(1,4,7));
		REQUIRE(a.row(1) == Vec3d(2,5,8));
		REQUIRE(a.row(2) == Vec3d(3,6,9));

		// test special operations
		{
			Mat<2,double> m(
				2,4,
				0,3
			);

			REQUIRE(eqVal(determinant(m), 6.));

			Mat<2,double> inv = m;
			REQUIRE(invert(inv));
			REQUIRE(eq(m*inv, Mat<2,double>::identity()));
		}

		{
			Mat<3,double> m(
				2,5,7,
				0,3,6,
				0,0,4
			);

			REQUIRE(eqVal(determinant(m), 24.));

			Mat<3,double> inv = m;
			REQUIRE(invert(inv));
			REQUIRE(eq(m*inv, Mat<3,double>::identity()));
		}

		#undef CHECK2
	}


	{	Complexd c(0,0);
		#define T(x, y) REQUIRE(x == y);
		T(c, Complexd(0,0))
		c.fromPolar(1, 0.2);	T(c, Polard(0.2))
		c.fromPolar(2.3);		T(c, Polard(2.3))
		REQUIRE(c != Complexd(0,0));
		T(c.conj(), Complexd(c.r, -c.i))
		#undef T

//		#define T(x, y) REQUIRE(almostEqual(x,y,2));
//		c.normalize();			T(c.norm(), 1)
//		double p=0.1; c(1,0); c *= Polard(1, p); T(c.arg(), p)
//
//		c.fromPolar(4,0.2);
//		T(c.sqrt().norm(), 2)
//		T(c.sqrt().arg(), 0.1)
//		#undef T
	}

	// Quat
	{
		struct printQuat{
			void operator()(const Quatd& v){ printf("%g %g %g %g\n", v[0], v[1], v[2], v[3]); }
		};

		// Test factory functions
		REQUIRE(Quatd::identity() == Quatd(1,0,0,0));


		// Test basic mathematical operations
		Quatd q(0,0,0,0);

		REQUIRE(q == Quatd(0,0,0,0));

		q.setIdentity();

		REQUIRE(q == Quatd(1,0,0,0));
		REQUIRE(q != Quatd(1,0,0,1));

		q.set(1,2,4,10);

		REQUIRE(-q == Quatd(-1,-2,-4,-10));
		REQUIRE(q.conj()	== Quatd(q.w, -q.x, -q.y, -q.z));
		REQUIRE(q.dot(q) == 121);
		REQUIRE(q.mag() == 11);
		REQUIRE(q.magSqr() == 121);
		REQUIRE(eq(q.sgn(), Quatd(1./11, 2./11, 4./11, 10./11)));


		// Test rotation of vectors by quaternion
		q.fromAxisAngle(M_2PI/4, 1,0,0);
		REQUIRE(eq(q, Quatd(sqrt(2)/2, sqrt(2)/2,0,0)));
		{
			Vec3d v(0,1,0);
			v = q.rotate(v);
			//printf("%g %g %g\n", v[0], v[1], v[2]);
			REQUIRE(eq(v, Vec3d(0,0,1)));
		}

		q.fromAxisAngle(M_2PI/4, 0,1,0);
		REQUIRE(eq(q, Quatd(sqrt(2)/2, 0,sqrt(2)/2,0)));
		{
			Vec3d v(0,0,1);
			v = q.rotate(v);
			//printf("%g %g %g\n", v[0], v[1], v[2]);
			REQUIRE(eq(v, Vec3d(1,0,0)));
		}

		q.fromAxisAngle(M_2PI/4, 0,0,1);
		{
			Vec3d v(1,0,0);
			v = q.rotate(v);
			//printf("%g %g %g\n", v[0], v[1], v[2]);
			REQUIRE(eq(v, Vec3d(0,1,0)));
		}

		// Test fromAxis* consistency
		REQUIRE(q.fromAxisAngle(M_2PI/8, 1,0,0) == Quatd().fromAxisX(M_2PI/8));
		REQUIRE(q.fromAxisAngle(M_2PI/8, 0,1,0) == Quatd().fromAxisY(M_2PI/8));
		REQUIRE(q.fromAxisAngle(M_2PI/8, 0,0,1) == Quatd().fromAxisZ(M_2PI/8));

		// Test AxisAngle<->Quat conversion
		{
			q.fromEuler(M_2PI/7, M_2PI/8, M_2PI/9);		// set to something non-trival...
			double angle, ax,ay,az;
			q.toAxisAngle(angle, ax,ay,az);
			Quatd b = q.fromAxisAngle(angle, ax,ay,az);
			REQUIRE(((q == b) || (q == b.conj())));
		}

		// Test consistency between conversions from Euler angles and axes
		REQUIRE(
			eq(
			q.fromEuler(M_2PI/8,M_2PI/8,M_2PI/8),
			(Quatd().fromAxisY(M_2PI/8) * Quatd().fromAxisX(M_2PI/8)) * Quatd().fromAxisZ(M_2PI/8)
			)
		);

		// Test roundtrip Euler/quat conversion
		{
			float eps = 0.00001;
			int N = 16;
			for(int k=0; k<N; ++k){
			for(int j=0; j<N; ++j){
			for(int i=0; i<N; ++i){
				float kludge = 0.1;
				float az = (float(k)/N-0.5) * 2*M_PI + kludge;
				float el = (float(j)/N-0.5) * 2*M_PI + kludge;
				float ba = (float(i)/N-0.5) * 2*M_PI + kludge;

				Quatf a,b;
				a.fromEuler(az, el, ba);
				a.toEuler(az, el, ba);
				b.fromEuler(az, el, ba);
				//a.print(); b.print(); printf("\n");

				REQUIRE((eq(a, b, eps) || eq(a, -b, eps)));
			}}}
		}

		// Test roundtrip matrix/quat conversions
		{
			double mat4[16];
			Quatd b;
			q.toMatrix(mat4);
			b = q.fromMatrix(mat4);
			REQUIRE((q == b || q == b.conj()));

			q.toMatrixTransposed(mat4);
			b = q.fromMatrixTransposed(mat4);
			REQUIRE((q == b || q == b.conj()));
		}


		// Test Quat to component coordinate frame
		{
			Quatd q;

			Vec3d vx, vy, vz;

			q.fromAxisAngle(M_2PI/4, 1,0,0);
			q.toVectorX(vx);
			q.toVectorY(vy);
			q.toVectorZ(vz);
			REQUIRE(eq(vx, Vec3d(1, 0,0)));
			REQUIRE(eq(vy, Vec3d(0, 0,1)));
			REQUIRE(eq(vz, Vec3d(0,-1,0)));

			q.fromAxisAngle(M_2PI/4, 0,1,0);
			q.toVectorX(vx);
			q.toVectorY(vy);
			q.toVectorZ(vz);
			REQUIRE(eq(vx, Vec3d(0,0,-1)));
			REQUIRE(eq(vy, Vec3d(0,1, 0)));
			REQUIRE(eq(vz, Vec3d(1,0, 0)));

			q.fromAxisAngle(M_2PI/4, 0,0,1);
			q.toVectorX(vx);
			q.toVectorY(vy);
			q.toVectorZ(vz);
			REQUIRE(eq(vx, Vec3d(0,1,0)));
			REQUIRE(eq(vy, Vec3d(-1,0,0)));
			REQUIRE(eq(vz, Vec3d(0,0,1)));
		}



//		int smps = 100;
//		Quatd q1 = Quatd::fromAxisAngle(10, .707, .707, 0);
//		Quatd q2 = Quatd::fromAxisAngle(60, .707, 0, .707);
//		Quatd buf[smps];
//		Quatd::slerp_buffer(q1, q2, buf, smps);
//		for (int i=0; i<smps; i++) {
//			double t, x, y, z;
//			buf[i].toAxisAngle(&t, &x, &y, &z);
//			//printf("%f %f %f %f\n", t, x, y, z);
//		}
	}



	// Simple Functions
	{
	const double pinf = INFINITY;		// + infinity
	const double ninf =-INFINITY;		// - infinity

	#define T(x,y, r) REQUIRE(al::atLeast(x,y) == r);
	T(0.,1., 1.) T(+0.1,1., 1.) T(-0.1,1., -1.)
	#undef T

	#define T(x, y) REQUIRE(al::ceilEven(x) == y);
	T(0, 0) T(1, 2) T(2, 2) T(3, 4) T(1001, 1002)
	#undef T

	#define T(x, y) REQUIRE(al::ceilPow2(x) == y);
	T(0, 0) T(1, 1) T(2, 2) T(3, 4)
	T(500, 512) T(999, 1024)
	#undef T

	#define T(x, y) REQUIRE(al::clip(x) == y);
	T(0., 0.) T(0.5, 0.5) T(1., 1.) T(1.2, 1.) T(-0.5, 0.) T(pinf, 1.) T(ninf, 0.)
	#undef T

	#define T(x, y) REQUIRE(al::clipS(x) == y);
	T(0., 0.) T(0.5, 0.5) T(1., 1.) T(1.2, 1.) T(-0.5, -0.5) T(-1., -1) T(-1.2, -1.)
	#undef T

	#define T(x,r) REQUIRE(al::even(x) == r);
	T(0,true) T(1,false) T(-2,true)
	#undef T

	#define T(x, y) REQUIRE(al::factorial(x) == y);
	T(0, 1) T(1, 1) T(2, 2*1) T(3, 3*2*1) T(4, 4*3*2*1)
	T(5, 5*4*3*2*1) T(6, 6*5*4*3*2*1) T(7, 7*6*5*4*3*2*1) T(8, 8*7*6*5*4*3*2*1)
	T(9, 9*8*7*6*5*4*3*2*1) T(10, 10*9*8*7*6*5*4*3*2*1) T(11, 11*10*9*8*7*6*5*4*3*2*1)
	T(12, 12*11*10*9*8*7*6*5*4*3*2*1)
	#undef T

	for(int i=0; i<=12; ++i){
		REQUIRE(
			al::aeq(al::factorialSqrt(i), sqrt(al::factorial(i)))
		);
	}

	#define T(x, y) REQUIRE(al::floorPow2(x) == y);
	T(0, 1) T(1, 1) T(2, 2) T(3, 2)
	T(513, 512) T(1090, 1024)
	#undef T

	#define T(x, y) REQUIRE(eqVal(al::fold(x), y));
	T(0., 0.) T(0.5, 0.5) T(1., 1.) T(1.2, 0.8) T(-0.2, 0.2)
	T(2.2, 0.2) T(3.2, 0.8) T(4.2, 0.2) T(5.2, 0.8)
	#undef T

	#define T(x,y,r) REQUIRE(al::gcd(x,y) == r);
	T(7,7,7) T(7,4,1) T(8,4,4)
	#undef T

	#define T(x,y,r) REQUIRE(al::lcm(x,y)==r);
	T(7,3,21) T(8,4,8) T(3,1,3)
	#undef T

	#define T(x,y,r) REQUIRE(al::lessAbs(x,y)==r);
	T(0.1,1., true) T(-0.1,1., true) T(1.,1., false) T(-1.,1., false)
	#undef T

	#define T(x,r) REQUIRE(al::odd(x) == r);
	T(0,false) T(1,true) T(-2,false)
	#undef T

	#define T(x) REQUIRE(al::pow2(x) == x*x);
	T(0) T(1) T(2) T(3) T(-1) T(-2) T(-3)
	#undef T

	#define T(x) REQUIRE(al::pow2S(x) == x*std::abs(x));
	T(0) T(1) T(2) T(3) T(-1) T(-2) T(-3)
	#undef T

	#define T(x) REQUIRE(al::pow3(x) == x*x*x);
	T(0) T(1) T(2) T(3) T(-1) T(-2) T(-3)
	#undef T

	#define T(x) REQUIRE(al::pow3Abs(x) == std::abs(x*x*x));
	T(0) T(1) T(2) T(3) T(-1) T(-2) T(-3)
	#undef T

	#define T(x) REQUIRE(al::pow4(x) == x*x*x*x);
	T(0) T(1) T(2) T(3) T(-1) T(-2) T(-3)
	#undef T

	#define T(x) REQUIRE(al::pow5(x) == x*x*x*x*x);
	T(0) T(1) T(2) T(3) T(-1) T(-2) T(-3)
	#undef T

	#define T(x) REQUIRE(al::pow6(x) == x*x*x*x*x*x);
	T(0) T(1) T(2) T(3) T(-1) T(-2) T(-3)
	#undef T

	#define T(x) REQUIRE(al::pow8(x) == x*x*x*x*x*x*x*x);
	T(0) T(1) T(2) T(3) T(-1) T(-2) T(-3)
	#undef T

	#define T(x) REQUIRE(al::pow16(x) == x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x);
	T(0) T(1) T(2) T(3) T(-1) T(-2) T(-3)
	#undef T

	#define T(x) REQUIRE(eqVal(al::pow64(x), x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x));
	T(0.) T(1.) T(1.01) T(1.02) T(-1.) T(-1.01) T(-1.02)
	#undef T

//	#define T(x,r) REQUIRE(al::powerOf2(x) == r);
//	T(0, false) T(1, true) T(2, true) T(3, false) T(4, true)
//	#undef T

	#define T(x,y,r) REQUIRE(al::round(x,y) == r);
	T(0.0,0.1, 0.0) T(0.1,0.1, 0.1) T(0.15,0.1, 0.1) T(-0.15,0.1, -0.1)
	#undef T

	#define T(x,y,r) REQUIRE(al::roundAway(x,y) == r);
	T(0.0,0.1, 0.0) T(0.1,0.1, 0.1) T(0.15,0.1, 0.2) T(-0.15,0.1, -0.2)
	#undef T

	#define T(x,r) REQUIRE(al::sgn(x) == r);
	T(-0.1, -1.) T(0.1, 1.) T(0., 0.)
	#undef T

	#define T(x1,y1,x2,y2, r) REQUIRE(al::slope(x1,y1,x2,y2) == r);
	T(3.,3.,4.,4., 1.) T(3.,-3.,4.,-4., -1.)
	#undef T

	{	double x=1, y=0;
		sort(x,y); REQUIRE(x==0); REQUIRE(y==1);
		sort(x,y); REQUIRE(x==0); REQUIRE(y==1);
	}

	#define T(x,y) REQUIRE(al::sumOfSquares(x) == y);
	T(1., 1.) T(2., 1*1+2*2) T(3., 1*1+2*2+3*3) T(4., 1*1+2*2+3*3+4*4) T(5., 1*1+2*2+3*3+4*4+5*5)
	#undef T

	#define T(x,r) REQUIRE(al::trailingZeroes(x) == r);
	T(0, 0) T(1, 0) T(2, 1) T(3, 0) T(4, 2) T(8, 3) T(9, 0)
	#undef T

	#define T(x,l,h,r) REQUIRE(al::within(x,l,h)==r);
	T(0,0,1, true) T(1,0,1, true)
	#undef T

//	printf("%.20g\n", wrap<double>(-32.0, 32.0, 0.));  // should be 0.0
//	printf("%.20g\n", wrap<double>(-64.0, 32.0, 0.));  // should be 0.0
//	printf("%.20g\n", wrap<double>(-1e-16, 32., 0.));  // should be 31.999999999999996447

	#define T(x, y) REQUIRE(eqVal(al::wrap(x, 1., -1.), y));
	T(0., 0.)	T( 0.5, 0.5) T( 1.,-1.) T( 1.2,-0.8) T( 2.2, 0.2)
				T(-0.5,-0.5) T(-1.,-1.) T(-1.2, 0.8) T(-2.2,-0.2)
	#undef T
	}

	#define T(x, y) REQUIRE(eqVal(al::wrapPhase(x), y));
	T(0., 0.)	T( 1., 1.) T( M_PI,-M_PI) T( M_PI+1, -M_PI+1) T( 7*M_PI+1, -M_PI+1)
				T(-1.,-1.) T(-M_PI,-M_PI) T(-M_PI-1,  M_PI-1) T(-7*M_PI+1, -M_PI+1)
	#undef T

	// Special Functions
	{
		struct F{

			// Pl,-m(x) = (-1)^m (l-m)! / (l+m)! Pl,m(x)
			static double testLegendreP(int l, int m, double x){
				switch(l){
				case 0:			return 1;
				case 1:
					switch(m){
					case -1:	return -1./(2) * testLegendreP(l,-m,x);
					case  0:	return x;
					case  1:	return -sqrt(1 - x*x);
					}
					break;
				case 2:
					switch(m){
					case -2:	return +1./(4*3*2*1) * testLegendreP(l,-m,x);
					case -1:	return -1./(  3*2  ) * testLegendreP(l,-m,x);
					case  0:	return 0.5 * (3*x*x - 1);
					case  1:	return -3 * x * sqrt(1 - x*x);
					case  2:	return 3 * (1 - x*x);
					}
					break;
				case 3:
					switch(m){
					case -3:	return -1. / (6*5*4*3*2*1) * testLegendreP(l,-m,x);
					case -2:	return +1. / (  5*4*3*2  ) * testLegendreP(l,-m,x);
					case -1:	return -1. / (    4*3    ) * testLegendreP(l,-m,x);
					case  0:	return 0.5 * x * (5*x*x - 3);
					case  1:	return 1.5 * (1 - 5*x*x) * sqrt(1-x*x);
					case  2:	return 15 * x * (1 - x*x);
					case  3:	return -15 * al::pow3(sqrt(1-x*x));
					}
					break;
				}
				return 0;	// undefined
			}

			static double testLaguerre(int n, int k, double x){
				switch(n){
				case 0:	return 1;
				case 1:	return -x + k + 1;
				case 2: return (1./2)*(x*x - 2*(k+2)*x + (k+2)*(k+1));
				case 3: return (1./6)*(-x*x*x + 3*(k+3)*x*x - 3*(k+2)*(k+3)*x + (k+1)*(k+2)*(k+3));
				default: return 0;
				}
			}
		};


		const int M = 2000;	// granularity of domain

		// test associated legendre
		for(int l=0; l<=3; ++l){
		for(int m=0; m<=l; ++m){
		for(int i=0; i<M; ++i){
			double theta = double(i)*M_PI / M;
			double a = al::legendreP(l, m, theta);
			double b = F::testLegendreP(l, m, cos(theta));
	//		if(!al::aeq(a, b, 1<<16)){
			if(!(std::abs(a - b)<1e-10)){
				printf("\nP(%d, %d, %g) = %.16g (actual = %.16g)\n", l,m, cos(theta), a,b);
				REQUIRE(false);
			}
		}}}

		// test laguerre
		for(int n=0; n<=3; ++n){
		for(int i=0; i<M; ++i){
			double x =double(i)/M * 4;
			double a = al::laguerreL(n,1, x);
			double b = F::testLaguerre(n,1, x);

			if(!(std::abs(a - b)<1e-10)){
				printf("\nL(%d, %g) = %.16g (actual = %.16g)\n", n, x, a,b);
				REQUIRE(false);
			}
		}}

		// TODO: spherical harmonics
//		for(int l=0; l<=SphericalHarmonic::L_MAX; ++l){
//		for(int m=-l; m<=l; ++m){
//	//		double c = SphericalHarmonic::coef(l,m);
//	//		double t = computeCoef(l,m);
//	//		REQUIRE(c == t);
//		}}
//
//		for(int j=0; j<M; ++j){	double ph = double(j)/M * M_PI;
//		for(int i=0; i<M; ++i){ double th = double(i)/M * M_2PI;
//
//		}}
	}


	// Interval
	{
		Interval<double> i(0,1);

		REQUIRE(i.min()==0); REQUIRE(i.max()==1);

		i.min(2);
		REQUIRE(i.min()==1); REQUIRE(i.max()==2);

		i.max(0);
		REQUIRE(i.min()==0); REQUIRE(i.max()==1);

		i.endpoints(-1,1);
		REQUIRE(i.min()==-1); REQUIRE(i.max()==1);

		REQUIRE(i.center()	==0);
		REQUIRE(i.diameter()	==2);
		REQUIRE(i.radius()	==1);

		REQUIRE(i.proper());

		i.endpoints(0,0);
		REQUIRE(i.degenerate());

		i.centerDiameter(1, 4);
		REQUIRE(i.center()	==1);
		REQUIRE(i.diameter()	==4);
		REQUIRE(i.min()==-1); REQUIRE(i.max()==3);

		i.center(2);
		REQUIRE(i.min()==0); REQUIRE(i.max()==4);

		i.diameter(6);
		REQUIRE(i.min()==-1); REQUIRE(i.max()==5);

		i.endpoints(-1, 1);
		REQUIRE(i.toUnit(0) == 0.5);

		REQUIRE(Interval<int>(0,1) == Interval<int>(0,1));
		REQUIRE(Interval<int>(0,2) != Interval<int>(0,1));
		REQUIRE((Interval<int>(0,2) += Interval<int>(-1,2)) == Interval<int>(-1,4));
		REQUIRE((Interval<int>(0,2) -= Interval<int>(-1,2)) == Interval<int>(-2,3));
	}


	// Random
	#if 0
	{
		using namespace al::rnd;

		// Ensure uniqueness of sequences
		REQUIRE(seed() != seed());

		{	LinCon a,b;
			REQUIRE(a() != b()); // sequences are unique
			REQUIRE(a() != a());	// successive values are unique
		}

		{	MulLinCon a,b;
			REQUIRE(a() != b()); // sequences are unique
			REQUIRE(a() != a()); // successive values are unique
		}

		{	Tausworthe a,b;
			REQUIRE(a() != b()); // sequences are unique
			REQUIRE(a() != a()); // successive values are unique
		}


		Random<> r;
		int N = 1000000;
		for(int i=0; i<N; ++i){ float v=r.uniform(); REQUIRE(  0 <= v); REQUIRE(v < 1); }
		for(int i=0; i<N; ++i){ int v=r.uniform(20,  0); REQUIRE(  0 <= v); REQUIRE(v < 20); }
		for(int i=0; i<N; ++i){ int v=r.uniform(20, 10); REQUIRE( 10 <= v); REQUIRE(v < 20); }
		for(int i=0; i<N; ++i){ int v=r.uniform(20,-10); REQUIRE(-10 <= v); REQUIRE(v < 20); }

		for(int i=0; i<N; ++i){ float v=r.uniformS(); REQUIRE(  -1 <= v); REQUIRE(v <  1); }
		for(int i=0; i<N; ++i){ int v=r.uniformS(20); REQUIRE( -20 <= v); REQUIRE(v < 20); }

		//for(int i=0; i<32; ++i) printf("% g ", r.uniformS());
		//for(int i=0; i<32; ++i) printf("%d ", r.prob(0.1));
		//for(int i=0; i<128; ++i) printf("% g\n", r.gaussian());

		int arr[] = {0,1,2,3,4,5,6,7};
		r.shuffle(arr, 8);
		//for(int i=0; i<8; ++i) printf("%d\n", arr[i]);
		//printf("\n");

		// Test uniformity of random sequence
		{
			Random<> r;
			const int N=64;
			const int M=1000;
			const int eps=M*0.2;
			int histo[N] = {0};

			for(int i=0; i<M*N; ++i){
				int idx = r.uniform(N);
				++histo[idx];
			}

			for(int i=0; i<N; ++i){
				int cnt = histo[i];
				//printf("%d\n", cnt);
				REQUIRE(M-eps < cnt); REQUIRE(cnt < M+eps);
			}
		}
	}
	#endif


	{
		al::Plane<double> p;
		p.fromNormalAndPoint(Vec3d(1,0,0), Vec3d(1,1,1));

		REQUIRE(p.distance(Vec3d(1,0,0)) ==  0);
		REQUIRE(p.distance(Vec3d(0,1,0)) == -1);
		REQUIRE(p.distance(Vec3d(0,0,1)) == -1);
		REQUIRE(p.distance(Vec3d(2,0,0)) ==  1);
	}

	{
		Frustumd f;
		f.fbl = Vec3d(-1,-1,-1);
		f.fbr = Vec3d( 1,-1,-1);
		f.ftl = Vec3d(-1, 1,-1);
		f.ftr = Vec3d( 1, 1,-1);
		f.nbl = Vec3d(-1,-1, 1);
		f.nbr = Vec3d( 1,-1, 1);
		f.ntl = Vec3d(-1, 1, 1);
		f.ntr = Vec3d( 1, 1, 1);
		f.computePlanes();

//		Plane<double>& p = f.pl[Frustumd::LEFT];
//		printf("%g %g %g\n", p.normal()[0], p.normal()[1], p.normal()[2]);
//
//		Vec3d nrm = cross(f.fbl-f.nbl, f.ntl-f.nbl);
//		printf("%g %g %g\n", nrm[0], nrm[1], nrm[2]);
//
//		printf("%g %g %g\n", (f.fbl-f.nbl)[0], (f.fbl-f.nbl)[1], (f.fbl-f.nbl)[2]);
//		printf("%g %g %g\n", (f.ntl-f.nbl)[0], (f.ntl-f.nbl)[1], (f.ntl-f.nbl)[2]);

		REQUIRE(f.testPoint(Vec3d(0,0,0)) == Frustumd::INSIDE);
		REQUIRE(f.testPoint(Vec3d(2,1,1)) == Frustumd::OUTSIDE);

		REQUIRE(f.testSphere(Vec3d(0,0,0), 0.9) == Frustumd::INSIDE);
		REQUIRE(f.testSphere(Vec3d(0,0,0), 1.1) == Frustumd::INTERSECT);
		REQUIRE(f.testSphere(Vec3d(2,2,2), 0.5) == Frustumd::OUTSIDE);
	}
}


