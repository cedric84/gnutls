/* ecc-internal.h

   Copyright (C) 2013, 2014 Niels Möller

   This file is part of GNU Nettle.

   GNU Nettle is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at your
       option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at your
       option) any later version.

   or both in parallel, as here.

   GNU Nettle is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see http://www.gnu.org/licenses/.
*/

/* Development of Nettle's ECC support was funded by the .SE Internet Fund. */

#ifndef GNUTLS_LIB_NETTLE_ECC_NETTLE_ECC_INTERNAL_H_INCLUDED
#define GNUTLS_LIB_NETTLE_ECC_NETTLE_ECC_INTERNAL_H_INCLUDED

#include <nettle/nettle-types.h>
#include <nettle/bignum.h>
#include <nettle/ecc-curve.h>
#include "gmp-glue.h"

/* Name mangling */
#define ecc_pp1_redc _gnutls_nettle_ecc_ecc_pp1_redc
#define ecc_pm1_redc _gnutls_nettle_ecc_ecc_pm1_redc
#define ecc_mod_add _gnutls_nettle_ecc_ecc_mod_add
#define ecc_mod_sub _gnutls_nettle_ecc_ecc_mod_sub
#define ecc_mod_mul_1 _gnutls_nettle_ecc_ecc_mod_mul_1
#define ecc_mod_addmul_1 _gnutls_nettle_ecc_ecc_mod_addmul_1
#define ecc_mod_submul_1 _gnutls_nettle_ecc_ecc_mod_submul_1
#define ecc_mod_mul _gnutls_nettle_ecc_ecc_mod_mul
#define ecc_mod_sqr _gnutls_nettle_ecc_ecc_mod_sqr
#define ecc_mod_mul_canonical _gnutls_nettle_ecc_ecc_mod_mul_canonical
#define ecc_mod_random _gnutls_nettle_ecc_ecc_mod_random
#define ecc_mod _gnutls_nettle_ecc_ecc_mod
#define ecc_mod_inv _gnutls_nettle_ecc_ecc_mod_inv
#define ecc_hash _gnutls_nettle_ecc_ecc_hash
#define gost_hash _gnutls_nettle_ecc_gost_hash
#define ecc_a_to_j _gnutls_nettle_ecc_ecc_a_to_j
#define ecc_j_to_a _gnutls_nettle_ecc_ecc_j_to_a
#define ecc_eh_to_a _gnutls_nettle_ecc_ecc_eh_to_a
#define ecc_dup_jj _gnutls_nettle_ecc_ecc_dup_jj
#define ecc_add_jja _gnutls_nettle_ecc_ecc_add_jja
#define ecc_add_jjj _gnutls_nettle_ecc_ecc_add_jjj
#define ecc_dup_eh _gnutls_nettle_ecc_ecc_dup_eh
#define ecc_add_eh _gnutls_nettle_ecc_ecc_add_eh
#define ecc_add_ehh _gnutls_nettle_ecc_ecc_add_ehh
#define ecc_dup_th _gnutls_nettle_ecc_ecc_dup_th
#define ecc_add_th _gnutls_nettle_ecc_ecc_add_th
#define ecc_add_thh _gnutls_nettle_ecc_ecc_add_thh
#define ecc_mul_g _gnutls_nettle_ecc_ecc_mul_g
#define ecc_mul_a _gnutls_nettle_ecc_ecc_mul_a
#define ecc_mul_g_eh _gnutls_nettle_ecc_ecc_mul_g_eh
#define ecc_mul_a_eh _gnutls_nettle_ecc_ecc_mul_a_eh
#define ecc_mul_m _gnutls_nettle_ecc_ecc_mul_m
#define cnd_copy _gnutls_nettle_ecc_cnd_copy
#define sec_add_1 _gnutls_nettle_ecc_sec_add_1
#define sec_sub_1 _gnutls_nettle_ecc_sec_sub_1
#define sec_tabselect _gnutls_nettle_ecc_sec_tabselect
#define sec_modinv _gnutls_nettle_ecc_sec_modinv
#define curve25519_eh_to_x _gnutls_nettle_ecc_curve25519_eh_to_x
#define curve448_eh_to_x _gnutls_nettle_ecc_curve448_eh_to_x

#define _nettle__secp_192r1 _gnutls_nettle_ecc__secp_192r1
extern const struct ecc_curve _nettle_secp_192r1;
#define _nettle__secp_224r1 _gnutls_nettle_ecc__secp_224r1
extern const struct ecc_curve _nettle_secp_224r1;
#define _nettle__secp_256r1 _gnutls_nettle_ecc__secp_256r1
extern const struct ecc_curve _nettle_secp_256r1;
#define _nettle__secp_384r1 _gnutls_nettle_ecc__secp_384r1
extern const struct ecc_curve _nettle_secp_384r1;
#define _nettle__secp_521r1 _gnutls_nettle_ecc__secp_521r1
extern const struct ecc_curve _nettle_secp_521r1;

/* Keep this structure internal for now. It's misnamed (since it's
   really implementing the equivalent twisted Edwards curve, with
   different coordinates). And we're not quite ready to provide
   general ecc operations over an arbitrary type of curve. */
#define _nettle__curve25519 _gnutls_nettle_ecc__curve25519
extern const struct ecc_curve _nettle_curve25519;
#define _nettle__curve448 _gnutls_nettle_ecc__curve448
extern const struct ecc_curve _nettle_curve448;

/* GOST curves, visible with underscore prefix for now */
#define _nettle__gost_gc256b _gnutls_nettle_ecc__gost_gc256b
extern const struct ecc_curve _nettle_gost_gc256b;
#define _nettle__gost_gc512a _gnutls_nettle_ecc__gost_gc512a
extern const struct ecc_curve _nettle_gost_gc512a;

#define ECC_MAX_SIZE ((521 + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS)

/* Window size for ecc_mul_a. Using 4 bits seems like a good choice,
   for both Intel x86_64 and ARM Cortex A9. For the larger curves, of
   384 and 521 bits, we could improve speed by a few percent if we go
   up to 5 bits, but I don't think that's worth doubling the
   storage. */
#define ECC_MUL_A_WBITS 4
/* And for ecc_mul_a_eh */
#define ECC_MUL_A_EH_WBITS 4

struct ecc_modulo;

/* Reduces from 2*ecc->size to ecc->size. */
/* Required to return a result < 2q. This property is inherited by
   mod_mul and mod_sqr. */
typedef void ecc_mod_func (const struct ecc_modulo *m, mp_limb_t *rp);

typedef void ecc_mod_inv_func (const struct ecc_modulo *m,
			       mp_limb_t *vp, const mp_limb_t *ap,
			       mp_limb_t *scratch);

/* Computes the square root of (u/v) (mod p) */
typedef int ecc_mod_sqrt_func (const struct ecc_modulo *m,
			       mp_limb_t *rp,
			       const mp_limb_t *up, const mp_limb_t *vp,
			       mp_limb_t *scratch);

typedef void ecc_add_func (const struct ecc_curve *ecc,
			   mp_limb_t *r,
			   const mp_limb_t *p, const mp_limb_t *q,
			   mp_limb_t *scratch);

typedef void ecc_dup_func (const struct ecc_curve *ecc,
			   mp_limb_t *r, const mp_limb_t *p,
			   mp_limb_t *scratch);

typedef void ecc_mul_g_func (const struct ecc_curve *ecc, mp_limb_t *r,
			     const mp_limb_t *np, mp_limb_t *scratch);

typedef void ecc_mul_func (const struct ecc_curve *ecc,
			   mp_limb_t *r,
			   const mp_limb_t *np, const mp_limb_t *p,
			   mp_limb_t *scratch);

typedef void ecc_h_to_a_func (const struct ecc_curve *ecc,
			      int flags,
			      mp_limb_t *r, const mp_limb_t *p,
			      mp_limb_t *scratch);

struct ecc_modulo
{
  unsigned short bit_size;
  unsigned short size;
  unsigned short B_size;
  unsigned short redc_size;
  unsigned short invert_itch;
  unsigned short sqrt_itch;

  const mp_limb_t *m;
  /* B^size mod m. Expected to have at least 32 leading zeros
     (equality for secp_256r1). */
  const mp_limb_t *B;
  /* 2^{bit_size} - m, same value as above, but shifted. */
  const mp_limb_t *B_shifted;
  /* m +/- 1, for redc, excluding redc_size low limbs. */
  const mp_limb_t *redc_mpm1;
  /* (m+1)/2 */
  const mp_limb_t *mp1h;

  ecc_mod_func *mod;
  ecc_mod_func *reduce;
  ecc_mod_inv_func *invert;
  ecc_mod_sqrt_func *sqrt;
};

/* Represents an elliptic curve of the form

     y^2 = x^3 - 3x + b (mod p)
*/
struct ecc_curve
{
  /* The prime p. */
  struct ecc_modulo p;
  /* Group order. FIXME: Currently, many functions rely on q.size ==
     p.size. This has to change for radix-51 implementation of
     curve25519 mod p arithmetic. */
  struct ecc_modulo q;

  unsigned short use_redc;
  unsigned short pippenger_k;
  unsigned short pippenger_c;

  unsigned short add_hh_itch;
  unsigned short add_hhh_itch;
  unsigned short dup_itch;
  unsigned short mul_itch;
  unsigned short mul_g_itch;
  unsigned short h_to_a_itch;

  ecc_add_func *add_hh;
  ecc_add_func *add_hhh;
  ecc_dup_func *dup;
  ecc_mul_func *mul;
  ecc_mul_g_func *mul_g;
  ecc_h_to_a_func *h_to_a;

  /* Curve constant */
  const mp_limb_t *b;

  /* For redc, same as B mod p, otherwise 1. */
  const mp_limb_t *unit;

  /* Tables for multiplying by the generator, size determined by k and
     c. The first 2^c entries are defined by

       T[  j_0 +   j_1 2 +     ... + j_{c-1} 2^{c-1} ]
         = j_0 g + j_1 2^k g + ... + j_{c-1} 2^{k(c-1)} g

     The following entries differ by powers of 2^{kc},

       T[i] = 2^{kc} T[i-2^c]
  */  
  const mp_limb_t *pippenger_table;
};

/* In-place reduction. */
ecc_mod_func ecc_mod;
ecc_mod_func ecc_pp1_redc;
ecc_mod_func ecc_pm1_redc;

ecc_mod_inv_func ecc_mod_inv;

void
ecc_mod_add (const struct ecc_modulo *m, mp_limb_t *rp,
	     const mp_limb_t *ap, const mp_limb_t *bp);
void
ecc_mod_sub (const struct ecc_modulo *m, mp_limb_t *rp,
	     const mp_limb_t *ap, const mp_limb_t *bp);

void
ecc_mod_mul_1 (const struct ecc_modulo *m, mp_limb_t *rp,
	       const mp_limb_t *ap, const mp_limb_t b);

void
ecc_mod_addmul_1 (const struct ecc_modulo *m, mp_limb_t *rp,
		  const mp_limb_t *ap, mp_limb_t b);
void
ecc_mod_submul_1 (const struct ecc_modulo *m, mp_limb_t *rp,
		  const mp_limb_t *ap, mp_limb_t b);

/* The mul and sqr functions need 2*m->size limbs at rp */
void
ecc_mod_mul (const struct ecc_modulo *m, mp_limb_t *rp,
	     const mp_limb_t *ap, const mp_limb_t *bp);

void
ecc_mod_sqr (const struct ecc_modulo *m, mp_limb_t *rp,
	     const mp_limb_t *ap);

/* mul function produces a canonical result, 0 <= R < M, needs 2*m->size limbs
 * at rp.
 */
void
ecc_mod_mul_canonical (const struct ecc_modulo *m, mp_limb_t *rp,
		       const mp_limb_t *ap, const mp_limb_t *bp);

/* mod q operations. */
void
ecc_mod_random (const struct ecc_modulo *m, mp_limb_t *xp,
		void *ctx, nettle_random_func *random, mp_limb_t *scratch);

void
ecc_hash (const struct ecc_modulo *m,
	  mp_limb_t *hp,
	  size_t length, const uint8_t *digest);

void
gost_hash (const struct ecc_modulo *m,
	  mp_limb_t *hp,
	  size_t length, const uint8_t *digest);

/* Converts a point P in affine coordinates into a point R in jacobian
   coordinates. */
void
ecc_a_to_j (const struct ecc_curve *ecc,
	    mp_limb_t *r, const mp_limb_t *p);

/* Converts a point P in jacobian coordinates into a point R in affine
   coordinates. If op == 1, produce x coordinate only. If op == 2,
   produce the x coordinate only, and also reduce it modulo q. */
void
ecc_j_to_a (const struct ecc_curve *ecc,
	    int op,
	    mp_limb_t *r, const mp_limb_t *p,
	    mp_limb_t *scratch);

/* Converts a point P in homogeneous coordinates on an Edwards curve
   to affine coordinates. Meaning of op is the same as for
   ecc_j_to_a. */
void
ecc_eh_to_a (const struct ecc_curve *ecc,
	     int op,
	     mp_limb_t *r, const mp_limb_t *p,
	     mp_limb_t *scratch);

/* Group operations */

/* Point doubling, with jacobian input and output. Corner cases:
   Correctly sets R = 0 (r_Z = 0) if p = 0 or 2p = 0. */
void
ecc_dup_jj (const struct ecc_curve *ecc,
	    mp_limb_t *r, const mp_limb_t *p,
	    mp_limb_t *scratch);

/* Point addition, with jacobian output, one jacobian input and one
   affine input. Corner cases: Fails for the cases

     P = Q != 0                       Duplication of non-zero point
     P = 0, Q != 0 or P != 0, Q = 0   One input zero
   
     Correctly gives R = 0 if P = Q = 0 or P = -Q. */
void
ecc_add_jja (const struct ecc_curve *ecc,
	     mp_limb_t *r, const mp_limb_t *p, const mp_limb_t *q,
	     mp_limb_t *scratch);

/* Point addition with Jacobian input and output. */
void
ecc_add_jjj (const struct ecc_curve *ecc,
	     mp_limb_t *r, const mp_limb_t *p, const mp_limb_t *q,
	     mp_limb_t *scratch);

/* Point doubling on a twisted Edwards curve, with homogeneous
   cooordinates. */
void
ecc_dup_eh (const struct ecc_curve *ecc,
	    mp_limb_t *r, const mp_limb_t *p,
	    mp_limb_t *scratch);

void
ecc_add_eh (const struct ecc_curve *ecc,
	    mp_limb_t *r, const mp_limb_t *p, const mp_limb_t *q,
	    mp_limb_t *scratch);

void
ecc_add_ehh (const struct ecc_curve *ecc,
	     mp_limb_t *r, const mp_limb_t *p, const mp_limb_t *q,
	     mp_limb_t *scratch);

void
ecc_dup_th (const struct ecc_curve *ecc,
	    mp_limb_t *r, const mp_limb_t *p,
	    mp_limb_t *scratch);

void
ecc_add_th (const struct ecc_curve *ecc,
	    mp_limb_t *r, const mp_limb_t *p, const mp_limb_t *q,
	    mp_limb_t *scratch);

void
ecc_add_thh (const struct ecc_curve *ecc,
	     mp_limb_t *r, const mp_limb_t *p, const mp_limb_t *q,
	     mp_limb_t *scratch);

/* Computes N * the group generator. N is an array of ecc_size()
   limbs. It must be in the range 0 < N < group order, then R != 0,
   and the algorithm can work without any intermediate values getting
   to zero. */ 
void
ecc_mul_g (const struct ecc_curve *ecc, mp_limb_t *r,
	   const mp_limb_t *np, mp_limb_t *scratch);

/* Computes N * P. The scalar N is the same as for ecc_mul_g. P is a
   non-zero point on the curve, in affine coordinates. Output R is a
   non-zero point, in Jacobian coordinates. */
void
ecc_mul_a (const struct ecc_curve *ecc,
	   mp_limb_t *r,
	   const mp_limb_t *np, const mp_limb_t *p,
	   mp_limb_t *scratch);

void
ecc_mul_g_eh (const struct ecc_curve *ecc, mp_limb_t *r,
	      const mp_limb_t *np, mp_limb_t *scratch);

void
ecc_mul_a_eh (const struct ecc_curve *ecc,
	      mp_limb_t *r,
	      const mp_limb_t *np, const mp_limb_t *p,
	      mp_limb_t *scratch);

void
ecc_mul_m (const struct ecc_modulo *m,
	   mp_limb_t a24,
	   unsigned bit_low, unsigned bit_high,
	   mp_limb_t *qx, const uint8_t *n, const mp_limb_t *px,
	   mp_limb_t *scratch);

void
cnd_copy (int cnd, mp_limb_t *rp, const mp_limb_t *ap, mp_size_t n);

mp_limb_t
sec_add_1 (mp_limb_t *rp, mp_limb_t *ap, mp_size_t n, mp_limb_t b);

mp_limb_t
sec_sub_1 (mp_limb_t *rp, mp_limb_t *ap, mp_size_t n, mp_limb_t b);

void
sec_tabselect (mp_limb_t *rp, mp_size_t rn,
	       const mp_limb_t *table, unsigned tn,
	       unsigned k);

void
curve25519_eh_to_x (mp_limb_t *xp, const mp_limb_t *p,
		    mp_limb_t *scratch);

void
curve448_eh_to_x (mp_limb_t *xp, const mp_limb_t *p,
		  mp_limb_t *scratch);

/* Current scratch needs: */
#define ECC_MOD_INV_ITCH(size) (2*(size))
#define ECC_J_TO_A_ITCH(size) (5*(size))
#define ECC_EH_TO_A_ITCH(size, inv) (2*(size)+(inv))
#define ECC_DUP_JJ_ITCH(size) (5*(size))
#define ECC_DUP_EH_ITCH(size) (5*(size))
#define ECC_DUP_TH_ITCH(size) (5*(size))
#define ECC_ADD_JJA_ITCH(size) (6*(size))
#define ECC_ADD_JJJ_ITCH(size) (8*(size))
#define ECC_ADD_EH_ITCH(size) (6*(size))
#define ECC_ADD_EHH_ITCH(size) (7*(size))
#define ECC_ADD_TH_ITCH(size) (6*(size))
#define ECC_ADD_THH_ITCH(size) (7*(size))
#define ECC_MUL_G_ITCH(size) (9*(size))
#define ECC_MUL_G_EH_ITCH(size) (9*(size))
#if ECC_MUL_A_WBITS == 0
#define ECC_MUL_A_ITCH(size) (12*(size))
#else
#define ECC_MUL_A_ITCH(size) \
  (((3 << ECC_MUL_A_WBITS) + 11) * (size))
#endif
#if ECC_MUL_A_EH_WBITS == 0
#define ECC_MUL_A_EH_ITCH(size) (12*(size))
#else
#define ECC_MUL_A_EH_ITCH(size) \
  (((3 << ECC_MUL_A_EH_WBITS) + 10) * (size))
#endif
#define ECC_MUL_M_ITCH(size) (11*(size))
#define ECC_ECDSA_SIGN_ITCH(size) (12*(size))
#define ECC_GOSTDSA_SIGN_ITCH(size) (12*(size))
#define ECC_MOD_RANDOM_ITCH(size) (size)
#define ECC_HASH_ITCH(size) (1+(size))

#endif /* GNUTLS_LIB_NETTLE_ECC_NETTLE_ECC_INTERNAL_H_INCLUDED */
