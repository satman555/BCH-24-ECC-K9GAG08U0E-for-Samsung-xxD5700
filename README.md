# BCH-24-ECC-K9GAG08U0E-for-Samsung-xxD5700
I am learning C and I have a code here that is supposed to calculate ECC BCH for Nand Flash K9GAG08U0E for Samsung xxD5700. It also calculates ECC per 42 bytes, but the result does not match the dump file. So I'm looking for help who can look at my code and find the errors.
I compile the programme as follows: gcc -o bch.c bch_lib.c
Info about hardware: 
Nand Flash: K9GAG08U0E-SCB0
CPU: SEMS-21
Nand Flash Main Area 8192, divided into 8 parts of 1024 bytes each. ECC code BCH-24 is used, i.e. recognises/corrects up to 24 bit errors per 1024 bytes.
The size of the spare area is 436 bytes, the size of the ECC code is 42 bytes for 1024 bytes of data.
