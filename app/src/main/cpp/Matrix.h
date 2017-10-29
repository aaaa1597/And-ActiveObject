#ifndef MATRIX_H
#define MATRIX_H

class Matrix {
public:
	static void LoadIdentity(float mat[4][4]);
	static void TranslateMatrix(float x, float y, float z, float iomat[4][4]);
	static void TranslateMatrix(float x, float y, float z, const float lhsm[4][4], float retmat[4][4]);
	static void ScaleMatrix(float x, float y, float z, float iomat[4][4]);
	static void ScaleMatrix(float x, float y, float z, const float lhsm[4][4], float retmat[4][4]);
	static void RotateMatrix(int angle, float x, float y, float z, float iomat[4][4]);
	static void RotateMatrix(int angle, float x, float y, float z, const float lhsm[4][4], float retmat[4][4]);
	static void MultiMatrix( const float lhsm[4][4], float iomat[4][4]);
	static void MultiMatrix( const float lhs[4][4], const float rhs[4][4], float retmat[4][4]);
};

#endif //MATRIX_H
