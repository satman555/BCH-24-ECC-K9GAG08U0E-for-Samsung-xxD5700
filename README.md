# BCH-24-ECC-K9GAG08U0E-for-Samsung-xxD5700
Ich lerne C und habe hier einen Code, der ECC BCH für Nand Flash K9GAG08U0E für Samsung xxD5700 berechnen soll. Es berechnet auch ECC pro 42 Bytes, aber das Ergebnis stimmt nicht mit der Dump-Datei überein. Deshalb suche ich Hilfe, die sich meinen Code anschauen und die Fehler finden kann.
Ich kompiliere das Programm wie folgt: gcc -o bch.c bch_lib.c
Info zu Hardware: 
Nand Flash: K9GAG08U0E-SCB0
CPU: SEMS-21
Nand Flash Main Area 8192, aufgeteilt in 8 Teile zu je 1024 Byte. Verwendet wird ECC Code BCH-24, also erkannt/korrigiert bis 24 Bit-Fehler pro 1024 Bytes.
