/** @file */
#ifndef _WAVE_PARSER_H_
#define _WAVE_PARSER_H_
#define _CRT_SECURE_NO_WARNINGS
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

/** Funkcja sprawdzajaca czy plik do ukrycia nie jest wiekszy od maksymalnej mozliwej liczby bitow do ukrycia w pliku wave
@param data_to_hide przekazuje dlugosc pliku do ukrycia(w bajtach)
@param input przekazuje wskaznik na plik wave
@param bits_to_write przekazuje ilosc zamienianych bitow
@param probe_size przekazuje rozmiar probki */
int check_max_size_data_to_hide(DataToHide* data_to_hide, FILE* input, unsigned int bits_to_write, size_t probe_size);
/** Funkcja zapisujaca pozostale bajty pliku wave, ktore nie byly modyfikowane
@param input przekazuje wskaznik do pliku wave
@param output przekazuje wskaznik do pliku wyjsciowego */
void write_rest(FILE* input, FILE* output);
/** Funkcja zapisujaca naglowek pliku WAVE do wyjsciowego pliku WAVE
@param file przekazuje wskaznik do pliku wave, ktorego naglowek odczytujemy
@param header wskaznik na strukture przechowujaca naglowek pliku WAVE */
void write_header(FILE* file, WaveHeader* header);
/** Funkcja odczytujaca naglowek pliku WAVE
@param file przekazuje wskaznik do pliku wave, ktorego naglowek odczytujemy
@param header wskaznik na strukture przechowujaca naglowek pliku WAVE
@return funkcja zwraca wskaznik na odczytana strukture plik WAVE */
WaveHeader* read_header(FILE* file, WaveHeader* header);
/** Funkcja ukrywa dane bitowe w poszczegolnych bajtach
@param data_to_hide przekazuje dane, które maja byc ukryte
@param bits_to_write przekazuje ilosc zamienianych bitow
@return funkcja zwraca ukryte dane */
unsigned char get_next_bit_pack(DataToHide* data_to_hide, unsigned int bits_to_write);
/** Funkcja pobiera d³ugoœæ pliku
@param file wskaznik na plik, ktorego dlugosc szukamy
@return funkcja zwraca dlugosc pliku */
int get_file_length(FILE* file);
/** Funkcja otwiera podane z linii polecen pliki  
@param path podaje sciezke do pliku wave
@param path2 podaje nazwe pliku wyjsciowego
@param path3 podaje sciezke do pliku, w ktorym znajduja sie bity do ukrycia
@param bits_to_write przekazuje ilosc zamienianych bitow */
int open_files(char path[],char path2[],char path3[], unsigned int bits_to_write);
/** Funkcja odczytuje plik wave oraz ukrywa podane bity
@param input wskaznik na plik wave
@param output wskaznik na plik, do ktorego bedziemy zapisywac plik wave z ukrytymi bitami
@param file_to_hide wskaznik na plik z bitami do ukrycia
@param bits_to_write przekazuje ilosc zamienianych bitow */
int read_wave(FILE* input, FILE* output, FILE* file_to_hide, unsigned int bits_to_write);
/** Funkcja odczytujaca zaszyfrowane dane z pliku wave
@param output wskaznik na zaszyforwany plik wave
@param bits_to_write przekazuje ilosc zamienianych bitow */
char read_output_bits(FILE* output, unsigned int bits_to_write);

#endif