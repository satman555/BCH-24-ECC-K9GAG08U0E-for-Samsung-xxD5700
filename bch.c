/*
 * Reed Solomon encoder for the GRX350/GRX550 nand controller
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Benjamin Larsson <benjamin.larsson@iopsys.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

 //#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>
#include "winbase.h"
#include <string.h>
#include "stdbool.h"
#include <stdint.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>
//#include <unistd.h>
#include "unistd.h"
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <malloc.h>
#include <float.h>
#include <stddef.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>


#include "bch.h"
//#include "mtd.h"

#define ESC 27
#define MAX_BLOCK_SIZE 1104384
#define MAX_OOB_BLOCK_SIZE 64
//#define Good_Block_Marker 0x7F800 /* 0x1FE00  Summe Hex FF pro Byte 512 sector - unsigned char */
//#define Good_Block_Marker 0x20000 /* Summe Hex FF pro Byte 512 sector - uint32_t */

/*          Mein Teil              */
#define input_file "Dump.bin" // Nand Dump.bin
#define output_file "Dump_New.bin" // New Dump.bin File

#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#define mem_size 65536
#define ESC 27

#define ZEROS 0
#define ONES 1
#define RSECC4 2
#define RSECC3 3
#define BCHECC7 4
#define BCHECC13 5
#define BCHECC14_calculate 6
#define BCHECC14_correct 7
#define BCHECC42_calculate 8

static unsigned int bch = 8;
//static unsigned char bch_data[ECC_SIZE];

int ecc_mode = BCHECC42_calculate;
unsigned char block_buffer[MAX_BLOCK_SIZE];
unsigned char out_buffer[MAX_BLOCK_SIZE];
unsigned char summe_Page[ECC_SIZE];

int pruefen_auf_FF(struct nand_bch_control* nbc, const unsigned char* buf, unsigned char* code);

const char* oob_mode_string[9] = { "Zeros", "Ones", "Reed Solomon 4 bytes", "Reed Solomon 3 bytes",
"BCH8 ECC 7 Bytes with 4 bits correction on every 512 bytes flash area", "BCH8 ECC 13 Bytes with 8 bits correction on every 512 bytes flash area",
"BCH8 ECC 14 Bytes with 8 bits correction on every 512 bytes flash area", "BCH8 ECC 14 Bytes with 8 bits correction on every 512 bytes flash area", 
"BCH8 ECC 42 Bytes with 24 bits correction on every 1024 bytes flash area" };



void print_menu() {
	system("cls");  // очищаем экран

	// Форматируем шапку программы
	printf("************************************************************************************************************\n");
	printf("***************************************Nand Tool - Baby***Vers.01*******************************************\n");
	printf("************************************************************************************************************\n");
	printf("*******************************************All rights reserved**********************************************\n");
	printf("************************************************************************************************************\n\n");
	printf("****** Copy the programme ECC_Tool.exe and the file <Dump.bin> to the directory /ECC on the hard disk C *****\n");
	printf("*************************************************************************************************************\n\n");
	printf("Selection menu:\n");
	printf("1. BCH8 13 Bytes ECC with 8 bits correction on every 512 bytes flash area Ecc-Calculate.\n");
	printf("2. BCH8 13 Bytes ECC with 8 bits correction on every 512 bytes flash area Ecc-Correct.\n");
	printf("3. BCH8 14 Bytes ECC with 8 bits correction on every 512 bytes flash area Ecc-Calculate.\n");
	printf("4. BCH8_14 Bytes ECC with 8 bits correction on every 512 bytes flash area Ecc-Correct.\n");
	printf("5. BCH8 ECC 42 Bytes with 24 bits correction on every 1024 bytes flash area.\n");
	printf("6. NC.\n");
	printf("7. NC.\n");
	printf("8. NC.\n");
	printf("9. NC.\n");
	printf("10. NC.\n");
	printf("11. NC.\n");
	printf("12. Exit.\n\n");
	printf("Please confirm your choice:");
}

int get_variant(int count) {
	int variant;
	char s[100]; // Zeile für das Einlesen der eingegebenen Daten
	scanf("%s", s); // Zeichenkette lesen

	// wenn die Eingabe falsch ist, melden Sie sie und bitten Sie um Wiederholung
	while (sscanf(s, "%d", &variant) != 1 || variant < 1 || variant > count) {
		printf("Incorrect input. Please repeat.: "); // Anzeige einer Fehlermeldung
		scanf("%s", s); // neu lesen
	}
	return variant;
}

/*          Ende mein Teil              */
unsigned char g_num2alpha[256] = { 0,0,1,99,2,198,100,106,3,205,199,188,101,126,107,42,4,141,206,
78,200,212,189,225,102,221,127,49,108,32,43,243,5,87,142,232,207,
172,79,131,201,217,213,65,190,148,226,180,103,39,222,240,128,177,
50,53,109,69,33,18,44,13,244,56,6,155,88,26,143,121,233,112,208,
194,173,168,80,117,132,72,202,252,218,138,214,84,66,36,191,152,149,
249,227,94,181,21,104,97,40,186,223,76,241,47,129,230,178,63,51,
238,54,16,110,24,70,166,34,136,19,247,45,184,14,61,245,164,57,59,7,
158,156,157,89,159,27,8,144,9,122,28,234,160,113,90,209,29,195,123,
174,10,169,145,81,91,118,114,133,161,73,235,203,124,253,196,219,30,
139,210,215,146,85,170,67,11,37,175,192,115,153,119,150,92,250,82,
228,236,95,74,182,162,22,134,105,197,98,254,41,125,187,204,224,211,
77,140,242,31,48,220,130,171,231,86,179,147,64,216,52,176,239,38,
55,12,17,68,111,120,25,154,71,116,167,193,35,83,137,251,20,93,248,
151,46,75,185,96,15,237,62,229,246,135,165,23,58,163,60,183 };

unsigned char g_alpha2num[256] = { 1,2,4,8,16,32,64,128,135,137,149,173,221,61,122,244,111,222,59,118,
	236,95,190,251,113,226,67,134,139,145,165,205,29,58,116,232,87,174,
	219,49,98,196,15,30,60,120,240,103,206,27,54,108,216,55,110,220,63,
	126,252,127,254,123,246,107,214,43,86,172,223,57,114,228,79,158,187,
	241,101,202,19,38,76,152,183,233,85,170,211,33,66,132,143,153,181,
	237,93,186,243,97,194,3,6,12,24,48,96,192,7,14,28,56,112,224,71,142,
	155,177,229,77,154,179,225,69,138,147,161,197,13,26,52,104,208,39,
	78,156,191,249,117,234,83,166,203,17,34,68,136,151,169,213,45,90,
	180,239,89,178,227,65,130,131,129,133,141,157,189,253,125,250,115,
	230,75,150,171,209,37,74,148,175,217,53,106,212,47,94,188,255,121,
	242,99,198,11,22,44,88,176,231,73,146,163,193,5,10,20,40,80,160,
	199,9,18,36,72,144,167,201,21,42,84,168,215,41,82,164,207,25,50,
	100,200,23,46,92,184,247,105,210,35,70,140,159,185,245,109,218,51,
	102,204,31,62,124,248,119,238,91,182,235,81,162,195,1 };

/* Galois Field mul */
unsigned char g_mul(unsigned char arg1, unsigned char arg2) {
	unsigned char s = 0;
	if ((arg1 == 0) || (arg2 == 0)) {
		return 0;
	};
	s = (s + g_num2alpha[arg1]) % 255;
	s = (s + g_num2alpha[arg2]) % 255;
	return g_alpha2num[s];
}

/* Galois Field add */
unsigned char g_add(unsigned char  arg3, unsigned char  arg4) {
	unsigned char  s = 0;
	s = s ^ arg3;
	s = s ^ arg4;
	return s;
};

void reed_solomon_128bytes_ecc(unsigned char* data_bytes_partial, unsigned char* s, int strength) {
	unsigned char g[4] = { 0 };
	unsigned char temp[4] = { 0 };
	unsigned char degree;
	unsigned char bytes;
	unsigned char y;
	int i;
	unsigned char t[3] = { 0 };

	if (strength == RSECC4) {
		g[3] = 205;
		g[2] = 63;
		g[1] = 92;
		g[0] = 32;
		degree = 4;
		bytes = 128;

		for (i = 0; i <= bytes - 1; i++) {
			y = g_add(s[3], data_bytes_partial[i]);
			temp[1] = g_mul(y, g[3]);
			temp[2] = g_mul(y, g[2]);
			temp[3] = g_mul(y, g[1]);
			s[3] = g_add(s[2], temp[1]);
			s[2] = g_add(s[1], temp[2]);
			s[1] = g_add(s[0], temp[3]);
			s[0] = g_mul(y, g[0]);
		};
	}

	/* 3 bytes mode */
	if (strength == RSECC3) {

		g[3] = 0;
		g[2] = g_add(g_add(2, 4), 8);
		g[1] = g_add(g_mul(2, 4), g_add(g_mul(4, 8), g_mul(2, 8)));
		g[0] = g_mul(g_mul(2, 4), 8);
		bytes = 128;
		for (i = 0; i <= bytes - 1; i++) {
			y = g_add(s[2], data_bytes_partial[i]);
			temp[2] = g_mul(y, g[2]);
			temp[3] = g_mul(y, g[1]);
			s[2] = g_add(s[1], temp[2]);
			s[1] = g_add(s[0], temp[3]);
			s[0] = g_mul(y, g[0]);
		};
	}

	return;
};

/*
for(i=0;i<mtd->writesize/128;i++){
	s = reed_solomn_128bytes_ecc(buf+i*128);
	chip->oob_poi[i*4]=s[3];
	chip->oob_poi[i*4+1]=s[2];
	chip->oob_poi[i*4+2]=s[1];
	chip->oob_poi[i*4+3]=s[0];


	  for(i=0;i<mtd->writesize/128;i++){
			 s = reed_solomn_128bytes_ecc(buf+i*128);
			 chip->oob_poi[i*3]=s[2];
			 chip->oob_poi[i*3+1]=s[1];
			 chip->oob_poi[i*3+2]=s[0];
		 }


};
*/

int BCH8_24Bit_42ECC_Bytes_EccCalculate()
{
	/*            Kopf                     */
//	setlocale(LC_ALL, "de_DE.UTF-8");
	system("cls");  // Display leeren
	// Kopf
	printf("************************************************************************************************************\n");
	printf("************************************* ECC Tool - Error correcting codes ************************************\n");
	printf("************************************************************************************************************\n");
	printf("************************************************************************************************************\n\n");
	printf("***** Copy the programme ECC_Tool.exe and the file <Dump.bin> to the directory /ECC on the hard disk C *****\n");
	printf("************************************************************************************************************\n\n");

	unsigned int Blocks_per_Device, Block_size_in_pages, Page_with_Spare;
	int offset_main, offset_spare, pos, pos_block, pos_page, pos_spare, spare_to_ecc, pos_ecc;
	int eccbytes;

	int Blocksize_full_in_Bytes, b_idx;

	pos = 0;
	pos_spare = 0;
	pos_page = 0;
	Block_size_in_pages = BLOCK_SIZE;
	spare_to_ecc = 2;

	offset_main = 0;
	offset_spare = 6;
	
	int option = 0;
	unsigned int blocks, block_size, oob_block_size, oob_area_size;
	int p_idx, ei_idx;
	int z = 4;
	int page_size = PAGE_SIZE;
	int pages_in_block = 0;
	//int ecc_mode = BCHECC7;
	blocks = 0;
	block_size = 0;

	// Berechnen Dateigroesse in Bites */
	off64_t fileSize = 0;
	FILE* f = fopen64(input_file, "rb");
	if (f == NULL) {
		printf("File open error.\n");
		//_getch();
		system("pause");
		printf("Press any button to confirm.");
		//return 0;
		exit(0);
	}
	fseeko64(f, 0, SEEK_END);
	fileSize = ftello64(f);
	fclose(f);

	/*        Ende    Kopf                     */

	eccbytes = ((bch * 13) + 7) / 8;
	//printf(" -Parameter <eccbytes>: %d Bytes\n", eccbytes);
	//printf(" -Parameter <eccbytes>: %u Bytes\n", eccbytes);

	oob_area_size = OOB_SIZE;
	//const char* input_file = NULL;
	//const char* output_file = NULL;
	//FILE* in_file = NULL, * out_file = NULL;

	struct stat {
		int st_size;
	} st;

	block_size = page_size * Block_size_in_pages; /*     Ohne oob_area      */
	oob_block_size = oob_area_size * Block_size_in_pages; /*     Ohne oob_area      */

	//printf("\nffffffffffff       fffffffffffffff        Zusaetzliche Datei-Informationen:");
	//_getch();

	/* Initialize BCH lib data and parameters */

	unsigned int cptr, wptr; /*  ECC Bytes berechnet */
	unsigned int cptr_ecc_from_dump, wptr_data_from_dump;
	unsigned char bch_code[CODE_SIZE];  /*     CODE_SIZE  13 oder 14 ECC Bytes aus OBB Spare Bereich    */
	unsigned char buf_data[ECC_SIZE];  /*     ECC_SIZE  512 ECC Bytes   */
	unsigned char buf_ecc[CODE_SIZE];  /*     CODE_SIZE  13 oder 14 ECC Bytes aus OBB Spare Bereich    */

	//int *buf_data;  /*     ECC_SIZE  512 ECC Bytes   */
	//int *buf_ecc;  /*     CODE_SIZE  13 oder 14 ECC Bytes aus OBB Spare Bereich    */

	unsigned char bch_data[ECC_SIZE];
	struct nand_bch_control* nbch;
	nbch = nand_bch_init(ECC_SIZE, CODE_SIZE);  /*    ECC_SIZE =  512 und CODE_SIZE = 13   */
	//struct nand_bch_control* nbch2;
	//nbch2 = nand_bch_init2(ECC_SIZE, CODE_SIZE);
	//struct bch_control* bch;
	//bch = 
	//int pruefen_auf_FF(struct nand_bch_control* nbc, const unsigned char* buf, unsigned char* code);

	//printf("\nZusaetzliche Datei-Informationen:");
	//_getch();



	/* Validate input */
	if (block_size == 0) {
		printf("Block size can not be 0.\n");
		goto exit;
	}
	/*  MAX_BLOCK_SIZE = 524288 */
	if (block_size > MAX_BLOCK_SIZE) {
		printf("Block size to large.\n");
	}

	if (page_size == 0) {
		printf("Page size can not be 0.\n");
		goto exit;
	}
	if (page_size % 32) {
		printf("Page size not even multiple of 32.\n");
		goto exit;
	}


	if ((ecc_mode < 0) && (ecc_mode > BCHECC42_calculate)) {
		printf("-e unsupported ecc mode %d.\n", ecc_mode);
		goto exit;
	}

	if (input_file == NULL) {
		printf("-i input file name is missing.\n");
		goto exit;
	}
	if (output_file == NULL) {
		printf("-o output file name is missing.\n");
		goto exit;
	}

	// Lesen der Datei */
	//in_file = fopen(input_file, "r");
	FILE* in_file = fopen64(input_file, "rb");
	if (in_file == NULL) {
		printf("In file open error.\n");
		goto exit;
	}

	// Anlegen einer Datei */
	FILE* out_file = fopen64(output_file, "wb");
	if (out_file == NULL) {
		printf("Out file open error.\n");
		goto exit;
	}

	//printf("\nrrrrrrrrrrrrrrrrrr ttttttttttttt Datei-Informationen:");
	//_getch();

	//printf("\n");
	
	//printf("\n");
	Page_with_Spare = page_size + oob_area_size;
	Blocksize_full_in_Bytes = Page_with_Spare * Block_size_in_pages;
	Blocks_per_Device = fileSize / Blocksize_full_in_Bytes;

	st.st_size = Blocks_per_Device * block_size;
	blocks = st.st_size / block_size;
	//printf("\nS.386 information <blocks> %d\n", blocks);
	pages_in_block = block_size / page_size;

	printf("\nAdditional information about the file:");
	printf("\n");
	//printf("\n");
	printf(" -File size complete <Dump.bin>: %" PRId64 "Bytes\n", fileSize);
	printf(" -Number of <Block per Device>: %d Blocks\n", Blocks_per_Device);
	// printf("Input file %s\n", input_file);
	//printf("Size of input file: %d\n", (int)st.st_size);
	printf(" -Block size without Spare: %d\n", block_size); /* ohne spare */
	printf(" -Block size with Spare: %d\n", Blocksize_full_in_Bytes); /* komplett mit spare */
	//printf("Input file blocks %d\n", blocks);
	printf(" -Page size without Spare: %d\n", page_size);
	printf(" -Page size with Spare: %d\n", Page_with_Spare);
	printf(" -Pages in block: %d\n", pages_in_block);
	//printf("ECC mode: %d\n", ecc_mode);
	printf(" -OOB area generation: %s\n", oob_mode_string[ecc_mode]);
	printf(" -OOB area size: %u\n", oob_area_size);
	printf("************************************************************************************************************\n\n");

	printf("\nTo calculate ECC, please press any key.");
	printf("\n");
	printf("\n");
	_getch();

	/*  Berechnungsziklus  */
	for (b_idx = 0; b_idx < Blocks_per_Device; b_idx++) {
		//for (b_idx = 0; b_idx < Blocks_per_Device; b_idx++) {
		//printf("Block %d\n", b_idx); /* Zeigt bearbeitenden Block */
		//printf("Parameter <Blocks_per_Device>: %lld  <Nummer>\n", b_idx);
		//fseeko(in_file, b_idx, SEEK_SET);
		fread(block_buffer, Blocksize_full_in_Bytes, 1, in_file); /*  Es wird 1 Bloch ganz in Puffer geladen  */

		/* Ab jetzt wird 1 Block Seitenweise in Buffer geladen */
		for (p_idx = 0; p_idx < pages_in_block; p_idx++) {
			//printf("\tPage %d\n", p_idx);
			/* copy the initial page */

			//printf("\nS.452 Zikl <ei_idx> Anzahl wiederholen wiederholen: %d\n", p_idx);
			//memcpy(&out_buffer[p_idx * (page_size + oob_area_size)], &block_buffer[p_idx * page_size], page_size);

			/* Ab jetzt wird 1 Seite ohne Spare in Buffer <out_buffer> zum Bearbeiten geladen */
			//memcpy(&out_buffer[p_idx * (Page_with_Spare)], &block_buffer[p_idx * (Page_with_Spare)], page_size); /* Variable <page_size> - Page size without Spare */
			memcpy(&out_buffer[p_idx * (Page_with_Spare)], &block_buffer[p_idx * (Page_with_Spare)], Page_with_Spare);
			/* generate oob area data */
			//printf("offset %d 0x%x\n", (p_idx+1)* Page_with_Spare, (p_idx+1)*page_size); /* Zeigt bearbeitenden Page eines Blockes */

				//printf("\nS.427 case BCHECC14 Druecken Sie eine beliebige Taste.");
				//printf("\n");
				/* ECC_BLOCKS (PAGE_SIZE 2048 / ECC_SIZE 512) */
				/*  OOB_SIZE = 64; ECC_BYTES 52 = (CODE_SIZE 13 *ECC_BLOCKS 4) */
				/* page_size = 2048; */

				//memset(&out_buffer[(p_idx + 1) * page_size + p_idx * oob_area_size], 0xFF, oob_area_size); /* page_size = 2048; */

				/* Ab jetzt wird 1 Seite je 512 Bytes abgearbeitet */
				//memset(&out_buffer[(p_idx + 1) * page_size + p_idx * oob_area_size], 0xFF, oob_area_size); /* Spare komplett 64 Byte macht mit FF ; */
				//wptr = (p_idx + 1) * page_size + p_idx * oob_area_size + ECC_OFFSET; /* Potok <wptr> mit ECC_OFFSET 13 (OOB_SIZE 64 -ECC_BYTES 52) */
			//wptr_data_from_dump = p_idx * Page_with_Spare; /* Potok <wptr> mit ECC_OFFSET 13 (OOB_SIZE 64 -ECC_BYTES 52) */
			for (ei_idx = 0; ei_idx < page_size / ECC_SIZE; ei_idx++) /*    4 mal wiederholt sich*/
			{

				//printf("\nS.474 Zikl <ei_idx> Anzahl wiederholen wiederholen: %d\n", ei_idx);
				//printf("S. 438 Parameter <sizeof(*bch_code)>: %d Bytes\n", sizeof(bch_code)); /*  bch_code =13 Bytes     */
				//printf("S. 439 Parameter <p_idx * page_size)>: %d Bytes\n", p_idx* Page_with_Spare);

				//printf("S.455 Parameter <bch_code>: %d\n", sizeof(bch_code));

				//nand_bch_calculate_ecc(nbch, &block_buffer[p_idx * Page_with_Spare + ei_idx * 512], bch_code); /* bch_code = Errechnet ECC Code 13 Bytes*/
				//pruefen_auf_FF(nbch, &block_buffer[p_idx * Page_with_Spare], bch_code);

				memcpy(buf_data, &out_buffer[p_idx * Page_with_Spare + ei_idx * ECC_SIZE], ECC_SIZE); /* Daten mit offset werden in den Buffer buf_bp kopiert */
				//memcpy(buf_ecc, &out_buffer[(p_idx * Page_with_Spare) + page_size + ECC_OFFSET + ei_idx * CODE_SIZE], CODE_SIZE); /* Daten mit offset werden in den Buffer code kopiert */

				//buf_data = &bock_buffer[p_idx * Page_with_Spare + ei_idx * 512];
				//buf_ecc = &block_buffer[(p_idx * Page_with_Spare) + page_size + ECC_OFFSET + ei_idx * 14];
				//nand_bch_correct_data(nbch2, buf_data, buf_ecc, bch_data); /* bch_code = ECC Bytes wird berechnet,  */
				nand_bch_calculate_ecc(nbch, buf_data, bch_code); /* bch_code = 13 Bytes*/

				//pruefen_auf_FF(nbch, &block_buffer[p_idx * Page_with_Spare], bch_code);
				pruefen_auf_FF(nbch, &block_buffer[p_idx * Page_with_Spare], bch_code);
				//printf("\n");
				//ei_idx = ei_idx + 1;
				//printf("S.490 Buffer bch_data[0] bch_data[1] bch_data[2] bch_data[3]: %X %X %X %X  \n", bch_data[0], bch_data[1], bch_data[2], bch_data[3]);
				//printf("S.491 Buffer bch_data[508] bch_data[509] bch_data[510] bch_data[511]: %X %X %X %X  \n", bch_data[508], bch_data[509], bch_data[510], bch_data[511]);
				//_getch();

				//printf("S.496 ffer buf[0] buf[1] buf[2] buf[3]: %X %X %X %X  \n", buf_data[0], buf_data[1], buf_data[2], buf_data[3]);
				//rintf("S.1205 Buffer buf[508] buf[509] buf[510] buf[511]: %X %X %X %X  \n", buf[508], buf[509], buf[510], buf[511]);
				//_getch();

				memcpy(&out_buffer[p_idx * Page_with_Spare + page_size + ECC_OFFSET + ei_idx * (ECC_OFFSET + CODE_SIZE)], bch_code, CODE_SIZE); /* Daten mit offset werden in den Buffer buf_bp kopiert */
				//out_buffer[wptr_data_from_dump = ei_idx * ECC_SIZE] = buf_data;

				//for (cptr = 0; cptr < page_size / 512; cptr++) 
				//{
					//out_buffer[wptr_data_from_dump++] = bch_data; /* Hier wird ins Buffer ECC Code geschrieben bch_code[cptr] Berechnete ECC 13 Bytes*/
				//}
			}

			//_getch();
		}
		/* Ende 1 Block Seitenweise in Puffer abarbeiten */
		//printf("Block b_idx b_idx b_idx %d\n", b_idx);
		//printf("Block z zz z z z  %d\n", z);
		//printf("Block blocks blocks blocks %d\n", blocks);
		//_getch();
		/* Progress indicator */
		while (b_idx == z) {
			int progress = (b_idx * 100) / Blocks_per_Device;
			printf("\rProgress indicator: %2d%%", progress);
			//{
			//	printf("%c", "#");
			//}
			fflush(stdout);
			z = z + 4;
		}
		/* Ende Progress indicator */

		//fseeko(out_file, b_idx, SEEK_SET);
		fwrite(out_buffer, block_size + (oob_area_size * pages_in_block), 1, out_file); /* fwrite(quelle Fille, Groesse, Anzahl, wohin)  */

		//fseeko(in_file, b_idx, SEEK_SET);
		//fread(block_buffer, Blocksize_full_in_Bytes, 1, in_file); /*  Es wird 1 Bloch ganz in Puffer geladen  */
		//b_idx = b_idx + b_idx;

		//free(block_buffer);
		//block_buffer = NULL;

	}

	/**
int main( int argc, char ** argv )
{
	   FILE *ifh, *ofh;
	   char *ifile, *ofile;
	   unsigned char buf[ PAGR_SIZE+OOB_SIZE ];
	   unsigned char code[ CODE_SIZE ];
	   int  len, bptr, cptr, wptr;
	   struct nand_bch_control *nbch;


	   nbch = nand_bch_init( ECC_SIZE, CODE_SIZE );

	   memset( buf, 0xff, sizeof(buf) );

	   if ( argc < 3 )
	   {
			   printf("Usage:\n %s <file.bin> <file.oob>", argv[0] );
	   }

	   ifile = argv[1];
	   ofile = argv[2];

	   ifh = fopen( ifile, "rb" );
	   if ( ifh == NULL )
	   {
			   printf("Failed to open %s\n", ifile );
			   exit(1);
	   }
	   ofh = fopen( ofile, "wb" );
	   if ( ofh == NULL )
	   {
			   printf("Failed to open %s\n", ofile );
			   exit(1);
	   }

	   while (!feof(ifh))
	   {
			   len = fread( buf, sizeof(char), PAGE_SIZE, ifh );
			   if ( len != PAGE_SIZE )
			   {
					   break;
			   }

			   wptr = PAGE_SIZE + ECC_OFFSET;
			   for ( bptr=0; bptr < PAGE_SIZE; bptr += ECC_SIZE )
			   {
//                     __nand_calculate_ecc( &buf[bptr], 256, code );
					   nand_bch_calculate_ecc( nbch, &buf[bptr], code );

					   for ( cptr=0; cptr < CODE_SIZE; cptr++ )
					   {
							   buf[ wptr++ ] = code[ cptr ];
					   }
			   }

			   fwrite( buf, sizeof(char), sizeof(buf), ofh );

	   }

	   fclose(ifh);
	   fclose(ofh);
}

*/

exit:
	if (in_file) fclose(in_file);
	if (out_file) fclose(out_file);

	/* Progress indicator p*/
	printf("\rProgress indicator: %2d%%", 100);

	printf("\n");
	printf("\nDone. Press any button.");
	_getch();

	exit(0);

	_getch();
}


/* Anfang */
int BCH8_8Bit_13ECC_Bytes_EccCalculate() {

}
/*  Ende */

		/* Anfang */
int BCH8_8Bit_13ECC_Bytes_EccCorrect() {

}
/*  Ende */

	/* Anfang */
int BCH8_8Bit_14ECC_Bytes_EccCalculate() {

}
/*  Ende */

	/* Anfang */
int BCH8_8Bit_14ECC_Bytes_EccCorrect() {

}
/*  Ende */

	/* Anfang */
int NC1() {

}
/*  Ende */
		/* Anfang */
int NC2() {

}
/*  Ende */
		/* Anfang */
int NC3() {

}
/*  Ende */
				/*** Anfang ***/
int NC4() {

}
/*  Ende */
					/*** Anfang ***/
int NC5() {

}
/*  Ende */
					/*** Anfang ***/
int NC6() {

}
/*  Ende */

int main() {
	int variant; // gewaehlter Menuepunkt
	//setlocale(LC_ALL, "Rus");
	do {
		print_menu(); // das Menue anzuzeigen
		variant = get_variant(12); // liefert die Nummer des gewaehlten Menuepunkts
		switch (variant) {
		case 1:
			BCH8_8Bit_13ECC_Bytes_EccCalculate();
			break;

		case 2:
			BCH8_8Bit_13ECC_Bytes_EccCorrect();
			break;

		case 3:
			BCH8_8Bit_14ECC_Bytes_EccCalculate();
			break;

		case 4:
			BCH8_8Bit_14ECC_Bytes_EccCorrect();
			break;

		case 5:
			BCH8_24Bit_42ECC_Bytes_EccCalculate();
			break;

		case 6:
			NC1();
			break;

		case 7:
			NC2();
			break;

		case 8:
			NC3();
			break;

		case 9:
			NC4();
			break;

		case 10:
			NC5();
			break;

		case 11:
			NC6();
			break;

		}

		if (variant != 12)
			//	_getch();
			system("pause>nul"); // задерживаем выполнение, чтобы пользователь мог увидеть результат выполнения выбранного пункта
	} while (variant != 12);
	return 0;
}