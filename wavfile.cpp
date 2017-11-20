#include "wavfile.h"
#include <cmath>
#include <fstream>
#include <iostream>
using namespace std;

//File based on code from this site
//http://www.cplusplus.com/forum/beginner/166954/

namespace little_endian_io
{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}
using namespace little_endian_io;

void WavFile::open(std::string filename, int channels) {
  file.open(filename);
  
  if (!file) {
    throw runtime_error("WavFile: failed to open file");
  }

    // Write the file headers
  file << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
  write_word( file,     16, 4 );  // no extension data
  write_word( file,      1, 2 );  // PCM - integer samples
  // write_word( file,      2, 2 );  // two channels (stereo file)
  write_word( file,      1 * channels, 2 );  // two channels (stereo file)
  write_word( file, 44100, 4 );  // samples per second (Hz)
  write_word( file, 176400, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
  write_word( file, 2 * channels, 2 );  // data block size (size of one integer samples, one for each channel, in bytes)
  write_word( file, 16 * channels, 2 );  // number of bits per sample (use a multiple of 8)

  // Write the data chunk header
  data_chunk_pos = file.tellp();
  file << "data----";  // (chunk size to be filled in later)
}

void WavFile::write(float *values, int size) {
  if (!file) {
    throw runtime_error("WavFile: file not open");
  }

  constexpr double max_amplitude = 32760;  // "volume"
  
  for (int i = 0; i < size; ++i) {
    write_word(file, (int) (values[i] * max_amplitude), 2);
  }
}

void WavFile::close() {
  if (!file.is_open()) {
    return;
  }
  
  // (We'll need the final file size to fix the chunk sizes above)
  size_t file_length = file.tellp();

  // Fix the data chunk header to contain the data size
  file.seekp( data_chunk_pos + 4 );
  write_word( file, file_length - data_chunk_pos + 8 );

  // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
  file.seekp( 0 + 4 );
  write_word( file, file_length - 8, 4 ); 
}