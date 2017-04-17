/*******************************************************************************
    Taichi - Physically based Computer Graphics Library

    Copyright (c) 2016 Yuanming Hu <yuanmhu@gmail.com>

    All rights reserved. Use of this source code is governed by
    the MIT license as written in the LICENSE file.
*******************************************************************************/

#include "mpm_utils.h"

TC_NAMESPACE_BEGIN

void polar_decomp(const Matrix2 &A, Matrix2 &r, Matrix2 &s) {
    Matrix2 u, sig, v;
    svd(A, u, sig, v);
    r = u * glm::transpose(v);
    s = v * sig * glm::transpose(v);
}

// TODO: make it faster...
void svd(const Matrix2 &A_, Matrix2 &u, Matrix2 &sig, Matrix2 &v) {
	Matrix2d A(A_);
    if (frobenius_norm2(A - Matrix2d(A[0][0])) < 1e-7) {
        u = v = Matrix2(1);
		sig = Matrix2((real)(A_[0][0]));
        return;
    }
    Matrix2d AtA = glm::transpose(A) * A;
    const double a = AtA[0][0], b = AtA[0][1], c = AtA[1][0], d = AtA[1][1];
    const double D = max(0.0, (a - d) * (a - d) + 4.0 * b * c);
    const double delta = 0.5 * sqrt(D);
	// singular values of A are eigenvalues of A^T A
    const Vector2d sigma(0.5 * (a + d) + delta, 0.5 * (a + d) - delta);

	// eigenvectors of A^T A are columns of V

	if (sigma[0] < 1e-7 && sigma[1] < 1e-7) {
		sig = Matrix2(0.0f);
		u = v = Matrix2(1.0f);
		return;
	}
	else {
		if (delta > 1e-7) {
			// the first eigenvector
			Vector2d v_0(b, a - sigma[0]);
			if (sgn(v_0[0]) == 0 && sgn(v_0[1]) == 0) {
				Vector2d v_0(b, a - sigma[0]);
				v_0 = Vector2(d - sigma[0], c);
			}
			v_0 = glm::normalize(v_0);

			// the second eigenvector
			if (sigma[1] > 1e-7) {
				Vector2d v_1(AtA[1][1] - sigma[1], c);
				if (sgn(v_1[0]) == 0 && sgn(v_1[1]) == 0) {
					v_1 = Vector2(b, a - sigma[1]);
				}
				v_1 = glm::normalize(v_1);
				v = Matrix2(v_0[0], v_1[0], v_0[1], v_1[1]);
				sig = Matrix2(sqrt(sigma[0]), 0, 0, sqrt(sigma[1]));
				u = A_ * v * Matrix2(1.0f / sig[0][0], 0, 0, 1.0f / sig[1][1]);
			}
			else {
				// second singular value is 0
				v = Matrix2(v_0[0], -v_0[1], v_0[1], v_0[0]);
				sig = Matrix2(sqrt(sigma[0]), 0, 0, 0);
				u = A_ * v * Matrix2(1.0f / sig[0][0], 0, 0, 1.0f);
				// note, glm is colume major...
				u[1][0] = u[0][1];
				u[1][1] = -u[0][0];
			}
		} else {
			// two eigen(singular)values are the same. Simple scaling matrix.
			v = Matrix2(1, 0, 0, 1);
			sig = Matrix2(sqrt(sigma[0]), 0, 0, sqrt(sigma[1]));
			u = A_ * v * Matrix2(1.0f / sig[0][0], 0, 0, 1.0f / sig[1][1]);
		}
	}
}


TC_NAMESPACE_END
