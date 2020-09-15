/** @file */
#ifndef _WAVE_PARSER_H_
#define _WAVE_PARSER_H_
#define _CRT_SECURE_NO_WARNINGS
//#define DEBUG
#define READING(data) { if(data!=1){ printf("Cannot read from file");  fclose(file); \
																			return 1; }}
 
																			
#include <stdint.h>
#include <stdlib.h>	
#include <stdio.h>
#include <stdbool.h>
/** Struktura pliku WAVE*/
typedef struct
{
	char chunk_id[4];
	int32_t chunk_size;
	char format[4];
	///<SUBCHUNK 1 - HEADER
	char subchunk1_id[4];
	int32_t subchunk1_size;
	int16_t audio_format;
	int16_t num_channels;
	int32_t sample_rate;
	int32_t byte_rate;
	int16_t block_align;
	int16_t bits_per_sample;
	///<SUBCHUNK 2 - DATA
	char subchunk2_id[4];
	int32_t subchunk2_size;
}WaveHeader;

/** Pomocnicza struktura do ukrycia danych*/
typedef struct
{
	char* data_to_hide;
	unsigned int data_to_hide_length;
	unsigned int current_bit;

}DataToHide;

/** Funkcja zapisujaca naglowek pliku WAVE do wyjsciowego pliku WAVE
@param file przekazuje wskaznik do pliku wave, ktorego naglowek odczytujemy
@param header wskaznik na strukture przechowujaca naglowek pliku WAVE*/
void write_header(FILE* file, WaveHeader* header);
/** Funkcja odczytuj�ca naglowek pliku WAVE
@param file przekazuje wskaznik do pliku wave, ktorego naglowek odczytujemy
@param header wskaznik na strukture przechowujaca naglowek pliku WAVE
@return funkcja zwraca wskaznik na odczytana strukture plik WAVE*/
WaveHeader* read_header(FILE* file, WaveHeader* header);
/** Funkcja ukrywa dane bitowe w poszczeg�lnych bajtach
@param data_to_hide przekazuje dane, kt�re maja by� ukryte
@return funkcja zwraca ukryte dane*/
unsigned char get_next_bit_pack(DataToHide* data_to_hide, unsigned int bits_to_write);
/** Funkcja pobiera d�ugo�� pliku
@param file wska�nik na plik, kt�rego d�ugo�� szukamy
@return funkcja zwraca d�ugo�� pliku*/
int get_file_length(FILE* file);
/** Funkcja otwiera podane z linii polece� pliki  
@param path podaje �cie�k� do pliku wave
@param path2 podaje nazw� pliku wyj�ciowego
@param path3 podaje �cie�k� do pliku, w kt�rym znajduj� si� bity do ukrycia*/
int open_files(char path[],char path2[],char path3[], unsigned int bits_to_write);
/** Funkcja odczytuje plik wave oraz ukrywa podane bity
@param input wska�nik na plik wave
@param output wska�nik na plik, do kt�rego b�dziemy zapisywa� plik wave z ukrytymi bitami
@param file_to_hide wska�nik na plik z bitami do ukrycia*/
int read_wave(FILE* input, FILE* output, FILE* file_to_hide, unsigned int bits_to_write);
/** Funkcja odczytujaca zaszyfrowane dane z pliku wave
@param output wska�nik na zaszyforwany plik wave */
char read_output_bits(FILE* output, unsigned int bits_to_write);

#endif