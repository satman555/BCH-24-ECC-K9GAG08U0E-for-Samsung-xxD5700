/*
 * Generic binary BCH encoding/decoding library
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright © 2011 Parrot S.A.
 *
 * Author: Ivan Djelic <ivan.djelic@parrot.com>
 *
 * Description:
 *
 * This library provides runtime configurable encoding/decoding of binary
 * Bose-Chaudhuri-Hocquenghem (BCH) codes.
*/
#ifndef _BCH_H
#define _BCH_H

//#include <linux/types.h>
#include <stdlib.h>

/**
 * struct bch_control - BCH control structure
 * @m:          Galois field order
 * @n:          maximum codeword size in bits (= 2^m-1)
 * @t:          error correction capability in bits
 * @ecc_bits:   ecc exact size in bits, i.e. generator polynomial degree (<=m*t)
 * @ecc_bytes:  ecc max size (m*t bits) in bytes
 * @a_pow_tab:  Galois field GF(2^m) exponentiation lookup table
 * @a_log_tab:  Galois field GF(2^m) log lookup table
 * @mod8_tab:   remainder generator polynomial lookup tables
 * @ecc_buf:    ecc parity words buffer
 * @ecc_buf2:   ecc parity words buffer
 * @xi_tab:     GF(2^m) base for solving degree 2 polynomial roots
 * @syn:        syndrome buffer
 * @cache:      log-based polynomial representation buffer
 * @elp:        error locator polynomial
 * @poly_2t:    temporary polynomials of degree 2t
 * @swap_bits:  swap bits within data and syndrome bytes
 */
struct bch_control {
       unsigned int    m;
       unsigned int    n;
       unsigned int    t;
       unsigned int    ecc_bits;
       unsigned int    ecc_bytes;
/* private: */
       uint16_t       *a_pow_tab;
       uint16_t       *a_log_tab;
       uint32_t       *mod8_tab;
       uint32_t       *ecc_buf;
       uint32_t       *ecc_buf2;
       unsigned int   *xi_tab;
       unsigned int   *syn;
       int            *cache;
       struct gf_poly *elp;
       struct gf_poly *poly_2t[4];
       bool		       swap_bits;
       uint8_t        * databuf;
};

struct mtd_info;

struct bch_control *init_bch(int m, int t, unsigned int prim_poly);
struct bch_control* bch_init(int m, int t, unsigned int prim_poly, bool swap_bits);

void free_bch(struct bch_control *bch);

void encode_bch(struct bch_control *bch, const uint8_t *data, unsigned int len, uint8_t *ecc);

int decode_bch(struct bch_control *bch, const uint8_t *data, unsigned int len,
              const uint8_t *recv_ecc, const uint8_t *calc_ecc,
              const unsigned int *syn, unsigned int *errloc);

/**
 * struct nand_bch_control - private NAND BCH control structure
 * @bch:       BCH control structure
 * @ecclayout: private ecc layout for this BCH configuration
 * @errloc:    error location array
 * @eccmask:   XOR ecc mask, allows erased pages to be decoded as valid
 */
struct nand_bch_control {
       struct bch_control   *bch;
//     struct nand_ecclayout ecclayout;
       unsigned int         *errloc;
       unsigned char        *eccmask;
       unsigned char        *code;
};

int pruefen_auf_FF(struct nand_bch_control* nbc, const unsigned char* buf, unsigned char* code);
struct nand_bch_control *nand_bch_init(unsigned int eccsize, unsigned int eccbytes);

//struct nand_bch_control* nand_bch_init2(unsigned int eccsize, unsigned int recv_ecc, unsigned int bch_data);
struct nand_bch_control* nand_bch_init2(unsigned int eccsize, unsigned int recv_ecc);

int nand_bch_calculate_ecc(struct nand_bch_control *nbc, const unsigned char *buf, unsigned char *code); /*   code - das ist ECC Bytes*/

int nand_bch_correct_data(struct nand_bch_control* nbc, unsigned char* buf, unsigned char* read_ecc, unsigned char* bch_data);

//void correct_bch( struct bch_control* bch, uint8_t* data, unsigned int len, unsigned int* errloc, int nerr);

/* Von Kernel Referenz */
//void encode_bch(struct bch_control* bch, const uint8_t* data, unsigned int len, uint8_t* ecc);    // Gleich
//int decode_bch(struct bch_control* bch, const uint8_t* data, unsigned int len, const uint8_t* recv_ecc, const uint8_t* calc_ecc,
//    const unsigned int* syn, unsigned int* errloc);
/* Von Kernel Referenz */

#define BLOCK_SIZE 128 /*  in Pages */
#define PAGE_SIZE 8192
#define OOB_SIZE   436
#define ECC_SIZE   1024
#define ECC_BLOCKS (PAGE_SIZE/ECC_SIZE) /* ECC_BLOCKS  4 */
#define CODE_SIZE  42   /*   14 Bytes ECC Code   */
//#define CODE_SIZE  13
#define ENABLE_14_CODE_SIZE  0   /*   BCH8 aktivieren 14 Bytes ECC Code berechnet, es wird geprüft Funktion <struct nand_bch_control *nand_bch_init>  */
#define ECC_BYTES (CODE_SIZE*ECC_BLOCKS) /*  14 x 4 ECC_BYTES  56 */
#define ECC_OFFSET 12 /* Ab wann im OBB Bereich ECC Code anfaengt */


#endif /* _BCH_H */