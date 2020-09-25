#include "wave_parser.h"

int check_max_size_data_to_hide(DataToHide* data_to_hide, FILE* input, unsigned int bits_to_write, size_t probe_size)
{
	unsigned int pos = ftell(input);
	fseek(input, 0, SEEK_END);
	unsigned int rest_length = ftell(input) - pos;
	fseek(input, pos, SEEK_SET);
	if (((bits_to_write * rest_length) / probe_size ) < data_to_hide->data_to_hide_length * 8)
	{
		return 1;
	}
	else return 0;
}
void write_rest(FILE* input, FILE* output)
{
	unsigned int current_pos = ftell(input);
	fseek(input, 0, SEEK_END);
	unsigned int file_rest_length = ftell(input) - current_pos;
	fseek(input, current_pos, SEEK_SET);
	char* extra_buf = (char*)malloc(file_rest_length);
	fread(extra_buf, file_rest_length, 1, input);
	fwrite(extra_buf, file_rest_length, 1, output);

}
void write_header(FILE* file, WaveHeader* header)
{
	fwrite(header->chunk_id, sizeof(header->chunk_id), 1, file);
	fwrite(&header->chunk_size, sizeof(header->chunk_size), 1, file);
	fwrite(header->format, sizeof(header->format), 1, file);
	fwrite(header->subchunk1_id, sizeof(header->subchunk1_id), 1, file);
	fwrite(&header->subchunk1_size, sizeof(header->subchunk1_size), 1, file);
	fwrite(&header->audio_format, sizeof(header->audio_format), 1, file);
	fwrite(&header->num_channels, sizeof(header->num_channels), 1, file);
	fwrite(&header->sample_rate, sizeof(header->sample_rate), 1, file);
	fwrite(&header->byte_rate, sizeof(header->byte_rate), 1, file);
	fwrite(&header->block_align, sizeof(header->block_align), 1, file);
	fwrite(&header->bits_per_sample, sizeof(header->bits_per_sample), 1, file);
	fwrite(header->subchunk2_id, sizeof(header->subchunk2_id), 1, file);
	fwrite(&header->subchunk2_size, sizeof(header->subchunk2_size), 1, file);
}
WaveHeader* read_header(FILE* file, WaveHeader* header)
{
	int tmp;

	tmp = fread(header->chunk_id, sizeof(header->chunk_id), 1, file);
	READING(tmp);
	tmp = fread(&header->chunk_size, sizeof(header->chunk_size), 1, file);
	READING(tmp);
	tmp = fread(header->format, sizeof(header->format), 1, file);
	READING(tmp);
	tmp = fread(header->subchunk1_id, sizeof(header->subchunk1_id), 1, file);
	READING(tmp);
	tmp = fread(&header->subchunk1_size, sizeof(header->subchunk1_size), 1, file);
	READING(tmp);
	tmp = fread(&header->audio_format, sizeof(header->audio_format), 1, file);
	READING(tmp);
	tmp = fread(&header->num_channels, sizeof(header->num_channels), 1, file);
	READING(tmp);
	tmp = fread(&header->sample_rate, sizeof(header->sample_rate), 1, file);
	READING(tmp);
	tmp = fread(&header->byte_rate, sizeof(header->byte_rate), 1, file);
	READING(tmp);
	tmp = fread(&header->block_align, sizeof(header->block_align), 1, file);
	READING(tmp);
	tmp = fread(&header->bits_per_sample, sizeof(header->bits_per_sample), 1, file);
	READING(tmp);
	tmp = fread(header->subchunk2_id, sizeof(header->subchunk2_id), 1, file);
	READING(tmp);
	tmp = fread(&header->subchunk2_size, sizeof(header->subchunk2_size), 1, file);
	READING(tmp);

	return header;
}
char read_output_bits(FILE* output, unsigned int bits_to_write)
{
	WaveHeader* header = calloc(1, sizeof(WaveHeader));
	header = read_header(output, header);

	unsigned int probe_size = header->bits_per_sample / 8;
	unsigned char* buffer = (unsigned char*)calloc(probe_size, 10000);
	int lent;
	int j = 0;
	while ((lent = fread(buffer, probe_size, 1, output)) == 1 && j < 10000)
	{
		j++;
	}
	int len;
	unsigned char result_byte = 0;
	unsigned int how_many_bits_to_fill_byte = 0;
	
	FILE* read_data = fopen("output.txt", "wb");
	int pos = ftell(output);
	fseek(output, pos - probe_size, SEEK_SET);
	while ((len = fread(buffer, probe_size, 1, output)) == 1)
	{
		unsigned char byte = buffer[probe_size - 1];

		if (how_many_bits_to_fill_byte + bits_to_write < 8)
		{
			byte = byte << (8 - bits_to_write);
			byte = byte >> (8 - bits_to_write);
			result_byte = result_byte << bits_to_write;
			result_byte += byte;
			how_many_bits_to_fill_byte += bits_to_write;
		}
		else
		{
			byte = byte << (8 - bits_to_write);
			byte = byte >> (8 - bits_to_write);
			byte = byte >> (bits_to_write + how_many_bits_to_fill_byte - 8);
			result_byte = result_byte << (8 - how_many_bits_to_fill_byte);
			result_byte += byte;
			if (result_byte == 0)
			{
				break;
			}
			///<save result byte
			fwrite(&result_byte, sizeof(char), 1, read_data);
			result_byte = 0;
			byte = buffer[probe_size - 1];
			byte = byte << (8 - bits_to_write);
			byte = byte << (8 - how_many_bits_to_fill_byte);
			byte = byte >> (8 - bits_to_write);
			byte = byte >> (8 - how_many_bits_to_fill_byte);
			how_many_bits_to_fill_byte = (how_many_bits_to_fill_byte + bits_to_write) % 8;
			result_byte += byte;
		}
	}
}

unsigned char get_next_bit_pack(DataToHide* data_to_hide, unsigned int bits_to_write)
{
	unsigned char result = 0;
	unsigned int current_byte_no = data_to_hide->current_bit / 8;

	unsigned int rest = data_to_hide->current_bit % 8;

	bool is_everything_in_this_byte = rest + bits_to_write <= 8;

	if (is_everything_in_this_byte)
	{
		unsigned int rest = data_to_hide->current_bit % 8;
		unsigned char current_byte = data_to_hide->data_to_hide[current_byte_no];
		unsigned char result1 = (unsigned char)(current_byte << (rest));
		unsigned char result2 = (unsigned char)(result1 >> (rest));
		result = (unsigned char)(result2 >> (8 - bits_to_write - rest));

	}
	else
	{
		unsigned char buf1;
		unsigned char buf2;
		buf1 = (data_to_hide->data_to_hide[current_byte_no] << (rest));
		buf1 = buf1 >> rest;
		buf2 = (data_to_hide->data_to_hide[current_byte_no + 1] >> (16 - bits_to_write - rest));
		buf1 = buf1 << (bits_to_write + rest - 8);

		result = (unsigned char)buf1 + buf2;
	}
	data_to_hide->current_bit += bits_to_write;

	return result;
}

int get_file_length(FILE* file)
{
	fseek(file, 0, SEEK_END);
	unsigned int len = ftell(file);
	fseek(file, 0, SEEK_SET);
	return len;
}

int open_files(char path[], char path2[], char path3[], unsigned int bits_to_write)
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
	read_wave(input, output, file_to_hide, bits_to_write);
}

int read_wave(FILE* input, FILE* output, FILE* file_to_hide, unsigned int bits_to_write)
{
	DataToHide* data_to_hide = (DataToHide*)malloc(sizeof(DataToHide));
	data_to_hide->current_bit = 0;

	if ((data_to_hide->data_to_hide_length = get_file_length(file_to_hide) + 1) == -1)
	{
		fprintf(stderr, "Cannot read file's length.");
	}
	size_t len;
	
	data_to_hide->data_to_hide = (char*)calloc(data_to_hide->data_to_hide_length, sizeof(char));

	if ((len = fread(data_to_hide->data_to_hide, data_to_hide->data_to_hide_length - 1, sizeof(char), file_to_hide)) != 1)
	{
		fprintf(stderr, "Couldn't read data from file to hide. %d\n", len);
		fclose(file_to_hide);
		fclose(output);
		fclose(input);
		free(data_to_hide->data_to_hide);
		return 1;
	}
	data_to_hide->data_to_hide[data_to_hide->data_to_hide_length - 1] = 0;

	WaveHeader* header = calloc(1, sizeof(WaveHeader));
	header = read_header(input, header);

	write_header(output, header);

	size_t probe_size = header->bits_per_sample / 8;
	unsigned char* buffer = (unsigned char*)calloc(probe_size, sizeof(unsigned char));
	int how_many_to_shift = bits_to_write;
	int j = 0;
	while ((len = fread(buffer, probe_size, 1, input)) == 1 && j < 10000)
	{
		fwrite(buffer, sizeof(char), probe_size, output);
		j++;
	}
	int tmp;
	if((tmp = check_max_size_data_to_hide(data_to_hide, input, bits_to_write, probe_size)) == 1)
	{
		fprintf(stderr, "Data to hide is too big to hide in this wave file");
		return 1;
	}

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

		buffer[probe_size - 1] = (buffer[probe_size - 1] >> bits_to_write) << bits_to_write;
		unsigned char extra_bits = get_next_bit_pack(data_to_hide, bits_to_write);
		buffer[probe_size - 1] += extra_bits;

		fwrite(buffer, sizeof(char), probe_size, output);
		buffer[probe_size - 1] = 0;
	}

	write_rest(input, output);

	fclose(file_to_hide);
	fclose(output);
	fclose(input);
	free(buffer);
	free(data_to_hide->data_to_hide);
	free(header);

	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 6)
	{
		fprintf(stderr, "Wrong usage of program. \n");
		fprintf(stderr, "Use following order: Wave_File_Name.wav, output_file_name.wav File_To_Hide_Name, Number of bits to hide, Opening mode(1 or 2)");
		fprintf(stderr, "If you want to hide data, opening mode should be 1");
		fprintf(stderr, "If you want to read hidden data, opening mode should be 2");
		return 1;
	}
	unsigned int bits_to_write = atoi(argv[4]);
	unsigned int mode = atoi(argv[5]);
	if (mode == 1)
	{
		open_files(argv[1], argv[2], argv[3], bits_to_write);
	}
	else
	{
		FILE* output = fopen(argv[2], "rb");
		read_output_bits(output, bits_to_write);
	}

	return 0;
}