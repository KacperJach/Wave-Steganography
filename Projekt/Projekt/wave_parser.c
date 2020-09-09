#include "wave_parser.h"






void print_byte_as_bits(char val) {
  for (int i = 7; 0 <= i; i--) {
    printf("%c", (val & (1 << i)) ? '1' : '0');
  }
}

void print_bits(char * ty, char * val, unsigned char * bytes, size_t num_bytes) {
  printf("(%*s) %*s = [ ", 15, ty, 16, val);
  for (size_t i = 0; i < num_bytes; i++) {
    print_byte_as_bits(bytes[i]);
    printf(" ");
  }
  printf("]\n");
}

#define SHOW(T,V) do { T x = V; print_bits(#T, #V, (unsigned char*) &x, sizeof(x)); } while(0)








unsigned int bits_to_write;
char read_output_bits(FILE* output)
{
	int tmp;
	WaveHeader* header = calloc(1, sizeof(WaveHeader));
	tmp = fread(header->chunk_id, sizeof(header->chunk_id), 1, output);
	
	tmp = fread(&header->chunk_size, sizeof(header->chunk_size), 1, output);
	
	tmp = fread(header->format, sizeof(header->format), 1, output);
	
	tmp = fread(header->subchunk1_id, sizeof(header->subchunk1_id), 1, output);
	
	tmp = fread(&header->subchunk1_size, sizeof(header->subchunk1_size), 1, output);
	
	tmp = fread(&header->audio_format, sizeof(header->audio_format), 1, output);
	
	tmp = fread(&header->num_channels, sizeof(header->num_channels), 1, output);
	
	tmp = fread(&header->sample_rate, sizeof(header->sample_rate), 1, output);
	
	tmp = fread(&header->byte_rate, sizeof(header->byte_rate), 1, output);
	
	tmp = fread(&header->block_align, sizeof(header->block_align), 1, output);
	
	tmp = fread(&header->bits_per_sample, sizeof(header->bits_per_sample), 1, output);
	
	tmp = fread(header->subchunk2_id, sizeof(header->subchunk2_id), 1, output);
	
	tmp = fread(&header->subchunk2_size, sizeof(header->subchunk2_size), 1, output);

	unsigned int probe_size = header->bits_per_sample / 8;
	unsigned char* buffer = (unsigned char*)calloc(probe_size, 10000);
	int lent;
	int j = 0;
	while ((lent = fread(buffer, probe_size, 1, output)) == 1 && j < 10000)
	{
		j++;
	}
	int len;
	unsigned char result_byte=0;
	unsigned int how_many_bits_to_fill_byte = 0;
	int i = 0;

	FILE* read_data = fopen("output.txt", "wb");
	int pos = ftell(output);
	fseek(output, pos - probe_size, SEEK_SET);
	while ((len = fread(buffer,probe_size, 1, output)) == 1)
	{
		
		unsigned char byte = buffer[probe_size-1];
		if (how_many_bits_to_fill_byte + bits_to_write < 8)
		{
			byte = byte << (8 - bits_to_write);
			byte = byte >> (8 - bits_to_write);
			//result_byte = result_byte << bits_to_write;
			byte = byte << how_many_bits_to_fill_byte;
			result_byte += byte;
			how_many_bits_to_fill_byte += bits_to_write;
		}
		else
		{
			byte = byte << how_many_bits_to_fill_byte;
			byte = byte >> how_many_bits_to_fill_byte;
			byte = byte << how_many_bits_to_fill_byte;
			result_byte += byte;
			if (result_byte == 0)
			{
				break;
			}
			//save result byte
			fwrite(&result_byte, sizeof(char), 1, read_data);
			result_byte = 0;
			byte = buffer[probe_size - 1];
			byte = byte >> (8 - how_many_bits_to_fill_byte);    
			byte = byte << (how_many_bits_to_fill_byte + 1);
			byte = byte >> (how_many_bits_to_fill_byte + 1);
			how_many_bits_to_fill_byte = (how_many_bits_to_fill_byte + bits_to_write) % 8;
			result_byte += byte;
		}
		

		SHOW(unsigned int, byte);

	}
		
		
	

	
}
unsigned char get_next_bit_pack(DataToHide* data_to_hide)
{
	unsigned char result = 0;
	unsigned int current_byte_no = data_to_hide->current_bit / 8;
	
	unsigned int rest = data_to_hide->current_bit % 8;

	bool is_everything_in_this_byte = rest + bits_to_write <= 8;

	if(is_everything_in_this_byte)
	{
		unsigned char current_byte = data_to_hide->data_to_hide[current_byte_no];
		unsigned char result1 = (unsigned char)(current_byte >> rest);
		unsigned char result2 = (unsigned char)(result1 << (8 - bits_to_write));
		result =  (unsigned char)  ( result2 >> (8 - bits_to_write));	
	}
	else
	{
		int16_t current_word = (int16_t) (data_to_hide->data_to_hide[current_byte_no + 1] << 8);
		current_word += (int16_t)data_to_hide->data_to_hide[current_byte_no];
		int16_t result1 = (int16_t)(current_word >> rest);
		printf("First: %d ", result1);
		int16_t result2 = (int16_t)(result1 << (16 - bits_to_write));
		printf("Second: %d ", result2);
		result = (unsigned char)(result2 >> (16 - bits_to_write));
		printf("Third: %d ", result);
	}

	data_to_hide->current_bit += bits_to_write;
	printf("%d \n", data_to_hide->current_bit);
	return result;
}

unsigned int get_file_length(FILE* file)
{
	fseek(file, 0, SEEK_END);
	unsigned int len = ftell(file);
	fseek(file, 0, SEEK_SET);
	return len;
}

int open_files(char path[], char path2[], char path3[])
{
	FILE* input = fopen(path, "rb");
	if (input == NULL)
	{
		fprintf(stderr, "Couldn't open input file: %s \n", path);
		return 1;
	}
	FILE* output = fopen(path2, "wb");
	if (output == NULL)
	{
		fprintf(stderr, "Cannot open file");
		fclose(input);
		return 1;
	}
	FILE* file_to_hide = fopen(path3, "rb"); ///<READ DATA FILE TO HIDE
	if (file_to_hide == NULL)
	{
		fprintf(stderr, "Couldn't open file");
		fclose(input);
		fclose(output);
		return 1;
	}
	read_wave(input, output, file_to_hide);
}

int read_wave(FILE* input, FILE* output, FILE* file_to_hide)
{	
	DataToHide* data_to_hide = (DataToHide*)malloc(sizeof(DataToHide));
	data_to_hide->current_bit = 0;
	
	if ((data_to_hide->data_to_hide_length = get_file_length(file_to_hide)+1) == -1)
	{
		fprintf(stderr, "Cannot read file's length.");
	}
	size_t len;
	printf("%d \n \n \n", data_to_hide->data_to_hide_length);
	data_to_hide->data_to_hide = (char*)calloc(data_to_hide->data_to_hide_length, sizeof(char)); ///<+1 because we need to take care of current_byte_no+1 in get_next_bits_pack()

	if ((len = fread(data_to_hide->data_to_hide, data_to_hide->data_to_hide_length-1, sizeof(char), file_to_hide)) != 1)
	{
		fprintf(stderr, "Couldn't read data from file to hide. %d\n", len);
		fclose(file_to_hide);
		fclose(output);
		fclose(input);
		free(data_to_hide->data_to_hide);
		return 1;
	}
	data_to_hide->data_to_hide[data_to_hide->data_to_hide_length-1] = 0;

	int tmp;
	WaveHeader* header = calloc(1, sizeof(WaveHeader));
	tmp = fread(header->chunk_id, sizeof(header->chunk_id), 1, input);
	READING(tmp);
	tmp = fread(&header->chunk_size, sizeof(header->chunk_size), 1, input);
	READING(tmp);
	tmp = fread(header->format, sizeof(header->format), 1, input);
	READING(tmp);
	tmp = fread(header->subchunk1_id, sizeof(header->subchunk1_id), 1, input);
	READING(tmp);
	tmp = fread(&header->subchunk1_size, sizeof(header->subchunk1_size), 1, input);
	READING(tmp);
	tmp = fread(&header->audio_format, sizeof(header->audio_format), 1, input);
	READING(tmp);
	tmp = fread(&header->num_channels, sizeof(header->num_channels), 1, input);
	READING(tmp);
	tmp = fread(&header->sample_rate, sizeof(header->sample_rate), 1, input);
	READING(tmp);
	tmp = fread(&header->byte_rate, sizeof(header->byte_rate), 1, input);
	READING(tmp);
	tmp = fread(&header->block_align, sizeof(header->block_align), 1, input);
	READING(tmp);
	tmp = fread(&header->bits_per_sample, sizeof(header->bits_per_sample), 1, input);
	READING(tmp);
	tmp = fread(header->subchunk2_id, sizeof(header->subchunk2_id), 1, input);
	READING(tmp);
	tmp = fread(&header->subchunk2_size, sizeof(header->subchunk2_size), 1, input);
	READING(tmp);

	fwrite(header->chunk_id, sizeof(header->chunk_id), 1, output);
	fwrite(&header->chunk_size, sizeof(header->chunk_size), 1, output);
	fwrite(header->format, sizeof(header->format), 1, output);
	fwrite(header->subchunk1_id, sizeof(header->subchunk1_id), 1, output);
	fwrite(&header->subchunk1_size, sizeof(header->subchunk1_size), 1, output);
	fwrite(&header->audio_format, sizeof(header->audio_format), 1, output);
	fwrite(&header->num_channels, sizeof(header->num_channels), 1, output);
	fwrite(&header->sample_rate, sizeof(header->sample_rate), 1, output);
	fwrite(&header->byte_rate, sizeof(header->byte_rate), 1, output);
	fwrite(&header->block_align, sizeof(header->block_align), 1, output);
	fwrite(&header->bits_per_sample, sizeof(header->bits_per_sample), 1, output);
	fwrite(header->subchunk2_id, sizeof(header->subchunk2_id), 1, output);
	fwrite(&header->subchunk2_size, sizeof(header->subchunk2_size), 1, output);

	size_t probe_size = header->bits_per_sample / 8;
	unsigned char* buffer = (unsigned char*)calloc(probe_size, sizeof(unsigned char));
	int64_t i = 0;
	int how_many_to_shift = bits_to_write;
	int j = 0;
	while ((len = fread(buffer, probe_size, 1, input)) == 1 && j<10000)
	{
		fwrite(buffer, sizeof(char), probe_size, output);
		j++;
	}
	printf("\n");
	while ((len = fread(buffer, probe_size, 1, input)) == 1 && data_to_hide->current_bit / 8 < data_to_hide->data_to_hide_length)
	{
		if (len < 0)
		{
			fprintf(stderr, "Couldn't read from input file. %d\n", len);
			fclose(input);
			fclose(file_to_hide);
			fclose(output);
			free(buffer);
			free(data_to_hide->data_to_hide);
			return 1;
		}

#ifdef DEBUG
		if (buffer[probe_size - 1] > 0)
		{
			if (i % 2 == 0)
			{
				printf("LEWY %d: ", i);
				for (int j = 0; j < probe_size; j++)
				{
					printf("%02x ", buffer[j]);
				}
			}
			else
			{
				printf("PRAWY %d: ", i);
				for (int j = 0; j < probe_size; j++)
				{
					printf("%02x ", buffer[j]);
				}
			}
			printf("\n");
		}
		i++;
#endif
		SHOW(unsigned char, data_to_hide->data_to_hide[data_to_hide->current_bit / 8]);
		//printf("%c ", data_to_hide->data_to_hide[data_to_hide->current_bit / 8]);
		char buf[50];
		_itoa(buffer[probe_size - 1], buf, 2);
		
		buffer[probe_size - 1] = (buffer[probe_size - 1] >> bits_to_write) << bits_to_write;
		unsigned char extra_bits = get_next_bit_pack(data_to_hide);
		buffer[probe_size - 1] += extra_bits;
		
		_itoa(extra_bits, buf, 2);
		SHOW(unsigned int, extra_bits);
		_itoa(buffer[probe_size - 1], buf, 2);

		
		fwrite(buffer, sizeof(char), probe_size, output);
		buffer[probe_size - 1] = 0;
	}
	

	while ((len = fread(buffer, probe_size, 1, input)) == 1)
	{
		fwrite(buffer, sizeof(char), probe_size, output); 
	}

	

	fclose(file_to_hide);
	fclose(output);
	fclose(input);
	free(buffer);
	free(data_to_hide->data_to_hide);

	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		fprintf(stderr, "Wrong usage of program. \n");
		fprintf(stderr, "Use following order: Wave_File_Name.wav, output_file_name.wav File_To_Hide_Name, Number of bits to hide ");
		return 1;
	}
	bits_to_write = atoi(argv[4]);
	open_files(argv[1], argv[2],argv[3]);
	FILE* output = fopen("output.wav", "rb");
	read_output_bits(output);
	return 0;
}