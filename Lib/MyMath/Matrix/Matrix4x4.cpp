#include "Matrix4x4.h"

//================================================
// Matrix4x4行列の積
//================================================
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {

	Matrix4x4 matrix{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m[i][j] = 0;
			for (int k = 0; k < 4; ++k) {
				matrix.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return matrix;
}

//================================================
// Matrix4x4行列の逆行列
//================================================
Matrix4x4 Inverse(const Matrix4x4& m) {

	Matrix4x4 matrix = {};

	float det =
		m.m[0][0] * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
			m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
			m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][3] * m.m[2][2] * m.m[3][1]) -
		m.m[0][1] * (m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][0] +
			m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[1][0] * m.m[2][3] * m.m[3][2] -
			m.m[1][2] * m.m[2][0] * m.m[3][3] - m.m[1][3] * m.m[2][2] * m.m[3][0]) +
		m.m[0][2] * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
			m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
			m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][3] * m.m[2][1] * m.m[3][0]) -
		m.m[0][3] * (m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][1] * m.m[2][2] * m.m[3][0] +
			m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[1][0] * m.m[2][2] * m.m[3][1] -
			m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[1][2] * m.m[2][1] * m.m[3][0]);

	float invDet = 1.0f / det;

	matrix.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
		m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
		m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][3] * m.m[2][2] * m.m[3][1]) *
		invDet;
	matrix.m[0][1] = (m.m[0][1] * m.m[2][3] * m.m[3][2] + m.m[0][2] * m.m[2][1] * m.m[3][3] +
		m.m[0][3] * m.m[2][2] * m.m[3][1] - m.m[0][1] * m.m[2][2] * m.m[3][3] -
		m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]) *
		invDet;
	matrix.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
		m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][3] * m.m[3][2] -
		m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][3] * m.m[1][2] * m.m[3][1]) *
		invDet;
	matrix.m[0][3] = (m.m[0][1] * m.m[1][3] * m.m[2][2] + m.m[0][2] * m.m[1][1] * m.m[2][3] +
		m.m[0][3] * m.m[1][2] * m.m[2][1] - m.m[0][1] * m.m[1][2] * m.m[2][3] -
		m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]) *
		invDet;

	matrix.m[1][0] = (m.m[1][0] * m.m[2][3] * m.m[3][2] + m.m[1][2] * m.m[2][0] * m.m[3][3] +
		m.m[1][3] * m.m[2][2] * m.m[3][0] - m.m[1][0] * m.m[2][2] * m.m[3][3] -
		m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]) *
		invDet;
	matrix.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
		m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][3] * m.m[3][2] -
		m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][3] * m.m[2][2] * m.m[3][0]) *
		invDet;
	matrix.m[1][2] = (m.m[0][0] * m.m[1][3] * m.m[3][2] + m.m[0][2] * m.m[1][0] * m.m[3][3] +
		m.m[0][3] * m.m[1][2] * m.m[3][0] - m.m[0][0] * m.m[1][2] * m.m[3][3] -
		m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]) *
		invDet;
	matrix.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
		m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][3] * m.m[2][2] -
		m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][3] * m.m[1][2] * m.m[2][0]) *
		invDet;

	matrix.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
		m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
		m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][3] * m.m[2][1] * m.m[3][0]) *
		invDet;
	matrix.m[2][1] = (m.m[0][0] * m.m[2][3] * m.m[3][1] + m.m[0][1] * m.m[2][0] * m.m[3][3] +
		m.m[0][3] * m.m[2][1] * m.m[3][0] - m.m[0][0] * m.m[2][1] * m.m[3][3] -
		m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]) *
		invDet;
	matrix.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
		m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][0] * m.m[1][3] * m.m[3][1] -
		m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][3] * m.m[1][1] * m.m[3][0]) *
		invDet;
	matrix.m[2][3] = (m.m[0][0] * m.m[1][3] * m.m[2][1] + m.m[0][1] * m.m[1][0] * m.m[2][3] +
		m.m[0][3] * m.m[1][1] * m.m[2][0] - m.m[0][0] * m.m[1][1] * m.m[2][3] -
		m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1]) *
		invDet;

	matrix.m[3][0] = (m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[1][1] * m.m[2][0] * m.m[3][2] +
		m.m[1][2] * m.m[2][1] * m.m[3][0] - m.m[1][0] * m.m[2][1] * m.m[3][2] -
		m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]) *
		invDet;
	matrix.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
		m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][0] * m.m[2][2] * m.m[3][1] -
		m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][2] * m.m[2][1] * m.m[3][0]) *
		invDet;
	matrix.m[3][2] = (m.m[0][0] * m.m[1][2] * m.m[3][1] + m.m[0][1] * m.m[1][0] * m.m[3][2] +
		m.m[0][2] * m.m[1][1] * m.m[3][0] - m.m[0][0] * m.m[1][1] * m.m[3][2] -
		m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]) *
		invDet;
	matrix.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
		m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] -
		m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][2] * m.m[1][1] * m.m[2][0]) *
		invDet;

	if (det == 0) {

		return matrix;
	}

	return matrix;
}

//================================================
// Matrix4x4の単位行列
//================================================
Matrix4x4 MakeIdentity4x4() {

	Matrix4x4 matrix{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m[i][j] = (i == j) ? 1.0f : 0.0f;
		}
	}

	return matrix;
}

//================================================
// Matrix4x4の拡縮行列
//================================================
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	Matrix4x4 scaleMatrix = {
		scale.x, 0.0f, 0.0f ,0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f ,0.0f, 0.0f, 1.0f
	};

	return scaleMatrix;
}

//================================================
// Matrix4x4のx軸回転行列
//================================================
Matrix4x4 MakePitchMatrix(float radian) {

	float cosTheta = std::cos(radian);
	float sinTheta = std::sin(radian);

	Matrix4x4 pitchMatrix = {
		1.0f, 0.0f,0.0f,0.0f,
		0.0f, cosTheta, sinTheta, 0.0f,
		0.0f, -sinTheta, cosTheta, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	return pitchMatrix;
}

//================================================
// Matrix4x4のy軸回転行列
//================================================
Matrix4x4 MakeYawMatrix(float radian) {

	float cosTheta = std::cos(radian);
	float sinTheta = std::sin(radian);

	Matrix4x4 yawMatrix = {
		cosTheta, 0.0f, -sinTheta, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sinTheta, 0.0f, cosTheta, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return yawMatrix;
}

//================================================
// Matrix4x4のz軸回転行列
//================================================
Matrix4x4 MakeRollMatrix(float radian) {

	float cosTheta = std::cos(radian);
	float sinTheta = std::sin(radian);

	Matrix4x4 rollMatrix = {
		cosTheta, sinTheta, 0.0f, 0.0f,
		-sinTheta, cosTheta, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f,0.0f,0.0f, 1.0f
	};

	return rollMatrix;
}

//================================================
// Matrix4x4の回転行列
//================================================
Matrix4x4 MakeRotateMatrix(const Vector3& rotate) {

	Matrix4x4 pitchMatrix = MakePitchMatrix(rotate.x);	// X軸回転行列
	Matrix4x4 yawMatrix = MakeYawMatrix(rotate.y);		// Y軸回転行列
	Matrix4x4 rollMatrix = MakeRollMatrix(rotate.z);	// Z軸回転行列

	Matrix4x4 rotateMatrix = Multiply(pitchMatrix, Multiply(yawMatrix, rollMatrix));

	return rotateMatrix;
}

//================================================
// Matrix4x4の平行移動行列
//================================================
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {

	Matrix4x4 translateMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translate.x, translate.y, translate.z, 1.0f
	};

	return translateMatrix;
}

//================================================
// Matrix4x4のアフィン変換
//================================================
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {

	Matrix4x4 matrix = {};

	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	matrix = Multiply(scaleMatrix, rotateMatrix);
	matrix = Multiply(matrix, translateMatrix);

	return matrix;
}

//================================================
// 正射影行列
//================================================
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {

	Matrix4x4 matrix = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m[i][j] = 0.0f;
		}
	}

	matrix.m[0][0] = 2.0f / (right - left);
	matrix.m[1][1] = 2.0f / (top - bottom);
	matrix.m[2][2] = 1.0f / (farClip - nearClip);
	matrix.m[3][0] = (left + right) / (left - right);
	matrix.m[3][1] = (top + bottom) / (bottom - top);
	matrix.m[3][2] = nearClip / (nearClip - farClip);
	matrix.m[3][3] = 1.0f;

	return matrix;
}