
/** @file magfield.c
*
* Routines for studying magnetic field configuration and monopoles
* in spontaneously broken SU(2) + adjoint scalar theory.
* Everything here works also in the unbroken theory (but is physically "meaningless"),
* and we assume nothing about lattice boundary conditions.
*
* Note that in our implementation, the adjoint scalars are parametrized as
* 	A = 0.5 * a_i \sigma_i, where a_i is what is stored in the field array.
*
* These routines were originally adapted from David Weir's SCONE code.
*
*/

#include "su2.h"

// these routines make sense only if an adjoint scalar is present, so failsafe if here
#ifdef TRIPLET

/** Matrix multiplication, possibly with transpose.
 *
 * Multiply two arbitrary complex 2x2 matrices,
 * such as those produced by project_u1(),
 * and store the result in `in1`; if `dag` is nonzero then
 * take the transpose of `in2` first.
 *
 * NB: Does not change `in2`.
 */
void matmat(double *in1, double *in2, int dag) {

	/* Notation:
		matrix = u[0] + i u[1], u[2] + i [3]
					 	 u[4] + i [5], u[6] + i u[7]
	*/

  double a1 = in1[0];
  double b1 = in1[1];
  double c1 = in1[2];
  double d1 = in1[3];
  double e1 = in1[4];
  double f1 = in1[5];
  double g1 = in1[6];
  double h1 = in1[7];

  double a2 = in2[0];
  double b2 = in2[1];
  double c2 = in2[2];
  double d2 = in2[3];
  double e2 = in2[4];
  double f2 = in2[5];
  double g2 = in2[6];
  double h2 = in2[7];

  if(dag) {
    b2 = -1.0*b2;

    double temp;

    temp = c2;
    c2 = e2;
    e2 = temp;

    temp = d2;
    d2 = -1.0*f2;
    f2 = -1.0*temp;

    h2 = -1.0*h2;
  }

  in1[0] = a1*a2 - b1*b2 + c1*e2 - d1*f2;
  in1[1] = a2*b1 + a1*b2 + d1*e2 + c1*f2;
  in1[2] = a1*c2 - b1*d2 + c1*g2 - d1*h2;
  in1[3] = b1*c2 + a1*d2 + d1*g2 + c1*h2;
  in1[4] = a2*e1 - b2*f1 + e2*g1 - f2*h1;
  in1[5] = b2*e1 + a2*f1 + f2*g1 + e2*h1;
  in1[6] = c2*e1 - d2*f1 + g1*g2 - h1*h2;
  in1[7] = d2*e1 + c2*f1 + g2*h1 + g1*h2;

}

/* Create normalised adjoint Higgs field for 'projection', (\hat\Phi in hep-lat/0512006).
 * The full projector is defined above eq. (3.2).
 * .
 * Note that this is the correct projector only in the case of periodic boundary
 * conditions.
 */
void projector(double *proj, double *adjoint) {

	double a1 = adjoint[0];
	double a2 = adjoint[1];
	double a3 = adjoint[2];

	double modulus = sqrt(a1*a1 + a2*a2 + a3*a3);

	// store in our adjoint parametrization, so need factor of 2
	proj[0] = 2.0*a1/modulus;
	proj[1] = 2.0*a2/modulus;
	proj[2] = 2.0*a3/modulus;
}

/**
 * Creates the projected "U(1) link" at a given site and direction,
 * see Eq (3.2) in hep-lat/0512006.
 * The result of the operation is stored in `pro`.
 *
 * NB! It is an 8-element fully complex 2x2 matrix that cannot be parametrized
 * in terms of real numbers times Pauli matrices. Note the comments
 * identifying elements below.
 */
 void project_u1(params const* p, fields const* f, long i, int dir, double* pro) {
	 /* Notation: hl = "left" adjoint field, normalized
	 * 						hr = "right" adjoint field, normalized
	 *						u = SU(2) link variable between "left" and "right"
	 */
	 long nextsite = p->next[i][dir];

	 // create the projectors
	 double hl[3], hr[3];
	 projector(hl, f->su2triplet[i]);
	 projector(hr, f->su2triplet[nextsite]);

	 double* u = f->su2link[i][dir];

	 // multiply these and store the elements in pro

	 // (1,1) element, real and imag parts
	 pro[0] = (4*u[0] + 2*hl[2]*u[0] + hl[0]*hr[0]*u[0] + hl[1]*hr[1]*u[0] +
		 				2*hr[2]*u[0] + hl[2]*hr[2]*u[0] + 2*hl[1]*u[1] - 2*hr[1]*u[1] -
						hl[2]*hr[1]*u[1] + hl[1]*hr[2]*u[1] - 2*hl[0]*u[2] + 2*hr[0]*u[2] +
						hl[2]*hr[0]*u[2] - hl[0]*hr[2]*u[2] - hl[1]*hr[0]*u[3] +
						hl[0]*hr[1]*u[3])/16.0;

   pro[1] = (4*u[0] + 2*hl[2]*u[0] + hl[0]*hr[0]*u[0] + hl[1]*hr[1]*u[0] +
						2*hr[2]*u[0] + hl[2]*hr[2]*u[0] + 2*hl[1]*u[1] - 2*hr[1]*u[1] -
						hl[2]*hr[1]*u[1] + hl[1]*hr[2]*u[1] - 2*hl[0]*u[2] + 2*hr[0]*u[2] +
						hl[2]*hr[0]*u[2] - hl[0]*hr[2]*u[2] - hl[1]*hr[0]*u[3] +
						hl[0]*hr[1]*u[3])/16.0;

	 // (1,2) element, real and imag
	 pro[2] = (2*hl[0]*u[0] + 2*hr[0]*u[0] + hl[2]*hr[0]*u[0] - hl[0]*hr[2]*u[0] +
		 				hl[1]*hr[0]*u[1] + hl[0]*hr[1]*u[1] + 4*u[2] + 2*hl[2]*u[2] -
						hl[0]*hr[0]*u[2] + hl[1]*hr[1]*u[2] - 2*hr[2]*u[2] - hl[2]*hr[2]*u[2]
						- 2*hl[1]*u[3] + 2*hr[1]*u[3] + hl[2]*hr[1]*u[3] +
						hl[1]*hr[2]*u[3])/16.0;

	 pro[3] = (2*hl[0]*u[0] + 2*hr[0]*u[0] + hl[2]*hr[0]*u[0] - hl[0]*hr[2]*u[0] +
		 				hl[1]*hr[0]*u[1] + hl[0]*hr[1]*u[1] + 4*u[2] + 2*hl[2]*u[2] -
						hl[0]*hr[0]*u[2] + hl[1]*hr[1]*u[2] - 2*hr[2]*u[2] - hl[2]*hr[2]*u[2]
						- 2*hl[1]*u[3] + 2*hr[1]*u[3] + hl[2]*hr[1]*u[3] +
						hl[1]*hr[2]*u[3])/16.0;

	 // (2,1) element, real and imag
	 pro[4] = (2*hl[0]*u[0] + 2*hr[0]*u[0] - hl[2]*hr[0]*u[0] + hl[0]*hr[2]*u[0] -
		 				hl[1]*hr[0]*u[1] - hl[0]*hr[1]*u[1] - 4*u[2] + 2*hl[2]*u[2] +
		 				hl[0]*hr[0]*u[2] - hl[1]*hr[1]*u[2] - 2*hr[2]*u[2] + hl[2]*hr[2]*u[2]
						- 2*hl[1]*u[3] + 2*hr[1]*u[3] - hl[2]*hr[1]*u[3] -
						hl[1]*hr[2]*u[3])/16.0;

	 pro[5] = (2*hl[0]*u[0] + 2*hr[0]*u[0] - hl[2]*hr[0]*u[0] + hl[0]*hr[2]*u[0] -
		 				hl[1]*hr[0]*u[1] - hl[0]*hr[1]*u[1] - 4*u[2] + 2*hl[2]*u[2] +
						hl[0]*hr[0]*u[2] - hl[1]*hr[1]*u[2] - 2*hr[2]*u[2] + hl[2]*hr[2]*u[2]
						- 2*hl[1]*u[3] + 2*hr[1]*u[3] - hl[2]*hr[1]*u[3] -
						hl[1]*hr[2]*u[3])/16.0;

	 // (2,2) element, real and imag
	 pro[6] = (4*u[0] - 2*hl[2]*u[0] + hl[0]*hr[0]*u[0] + hl[1]*hr[1]*u[0] -
		 				2*hr[2]*u[0] + hl[2]*hr[2]*u[0] - 2*hl[1]*u[1] + 2*hr[1]*u[1] -
						hl[2]*hr[1]*u[1] + hl[1]*hr[2]*u[1] + 2*hl[0]*u[2] - 2*hr[0]*u[2] +
						hl[2]*hr[0]*u[2] - hl[0]*hr[2]*u[2] - hl[1]*hr[0]*u[3] +
						hl[0]*hr[1]*u[3])/16.0;

	 pro[7] = (4*u[0] - 2*hl[2]*u[0] + hl[0]*hr[0]*u[0] + hl[1]*hr[1]*u[0] -
		 				2*hr[2]*u[0] + hl[2]*hr[2]*u[0] - 2*hl[1]*u[1] + 2*hr[1]*u[1] -
						hl[2]*hr[1]*u[1] + hl[1]*hr[2]*u[1] + 2*hl[0]*u[2] - 2*hr[0]*u[2] +
						hl[2]*hr[0]*u[2] - hl[0]*hr[2]*u[2] - hl[1]*hr[0]*u[3] +
						hl[0]*hr[1]*u[3])/16.0;

	 // done! these projected matrices can now be multiplied using matmat()
 }

/* Calculate the projected Abelian "field strength" alpha_ij,
* eq (3.3) in hep-lat/0512006, at a given site i and directions dir1, dir2.
* So the return value is alpha(x_i)_{dir1, dir2}
*/
double alpha_proj(params const* p, fields const* f, long i, int dir1, int dir2) {
	long nextsite;

	// produce the projected link variables
	double u1[8], u2[8], u3[8], u4[8];

	project_u1(p, f, i, dir1, u1);
	nextsite = p->next[i][dir1];
	project_u1(p, f, nextsite, dir2, u2);
	nextsite = p->next[i][dir2];
	project_u1(p, f, nextsite, dir1, u3);
	project_u1(p, f, i, dir2, u4);

	// now calculate arg Tr u1.u2.u3^+.u4^+ using matmat()
	// u1 <- u1.u2 etc
	matmat(u1, u2, 0);
	matmat(u1, u3, 1);
	matmat(u1, u4, 1);

	// is there any ambiguity in defining the complex angle?
	double alpha = atan2(u1[1] + u1[7], u1[0] + u1[6]);

	// normalize by 2/g, in lattice units
	alpha *= sqrt(p->betasu2);

	// Note that alpha_ij is antisymmetric in the indices
	return alpha;
}

/* Calculate magnetic field B_i(x) at a given site and direction,
* eq. 3.4 in hep-lat/0512006 (B_i(x) = 0.5 * eps_{ijk} alpha_{jk}).
* Works in arbitrary p.dim dimensions.
*/
double magfield(params const* p, fields const* f, long i, int dir) {

	/* Two loops over the directions, with always d1 < d2.
	* Also, need to account for the Levi-Civita symbol eps_{ijk}, with i = dir fixed.
	* Now j < k always, so to get the correct sign we need 3 if checks (below).
	*/
	double res = 0.0;
	for (int d1=0; d1<p->dim; d1++) {
		if (d1 == dir) {
			continue;
		}
		// always d1 < d2
		for (int d2=d1+1; d2<p->dim; d2++) {

			if (d2 == dir) {
				continue;
			}

			double alpha = alpha_proj(p, f, i, d1, d2);

			if (d1 > dir) {
				// now dir < d1 < d2, so indices in eps_{ijk} are always in normal order
				res += alpha;
			} else if (d2 > dir) {
				// now d1 < dir < d2, so we need 1 permutation to bring indices to normal order
				res -= alpha;
			} else {
				// d1 < d2 < dir, so need 2 permutations
				res += alpha;
			}
		}

	}
	// should multiply by 2 because alpha_ij is antisymmetric and we only looped over i<j,
	// but the overall normalization of B kills this.

	return res;
}

/* Calculate magnetic charge density in a hypercube running in the positive directions
* from lattice site i. Eq. (3.5) in hep-lat/0512006.
*/
double magcharge_cube(params const* p, fields const* f, long i) {

	double res = 0.0;
	for (int dir=0; dir<p->dim; dir++) {
		double B1 = magfield(p, f, i, dir);
		double B2 = magfield(p, f, p->next[i][dir], dir);
		res += B2 - B1;
	}

	// this should be quantized in units of 4pi/g:
	//printf("Charges at site %ld: %lf\n", i, res * 2.0*M_PI*sqrt(p->betasu2));

	return res;
}

#endif // end ifdef TRIPLET
