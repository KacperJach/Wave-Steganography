/** @file */
#ifndef _WAVE_PARSER_H_
#define _WAVE_PARSER_H_
#define _CRT_SECURE_NO_WARNINGS
//#define DEBUG
#define READING(data) { if(data!=1){ printf("Couldn't read from file");  fclose(input); \
																			fclose(file_to_hide);\
																			fclose(output);\
																			free(data_to_hide);\
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

typedef struct
{
	char* data_to_hide;
	unsigned int data_to_hide_length;
	unsigned int current_bit;

}DataToHide;
/** Funkcja pobiera d�ugo�� pliku
@param file wska�nik na plik, kt�rego d�ugo�� szukamy
@return funkcja zwraca d�ugo�� pliku*/
int get_file_length(FILE* file);
/** Funkcja otwiera podane z linii polece� pliki  
@param path podaje �cie�k� do pliku wave
@param path2 podaje nazw� pliku wyj�ciowego
@param path3 podaje �cie�k� do pliku, w kt�rym znajduj� si� bity do ukrycia*/
int open_files(char path[],char path2[],char path3[]);
/** Funkcja odczytuje plik wave oraz ukrywa podane bity
@param file_to_hide wska�nik na plik z bitami do ukrycia
@param file_to_write wska�nik na plik, do kt�rego b�dziemy zapisywa� plik wave z ukrytymi bitami
@param input wska�nik na plik wave*/
int read_wave(FILE* input, FILE* output, FILE* file_to_hide);

#endif