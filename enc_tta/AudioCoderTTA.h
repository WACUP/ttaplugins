/*
The ttaplugins-winamp project.
Copyright (C) 2005-2015 Yamagata Fumihiro

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once
#include <nsv/enc_if.h>
#include <memory.h>
#include <malloc.h>
#include <windows.h>
#include <stdexcept>
#include <stdlib.h>

typedef __int8(TTAint8);
typedef __int16(TTAint16);
typedef __int32(TTAint32);
typedef __int64(TTAint64);
typedef unsigned __int8(TTAuint8);
typedef unsigned __int16(TTAuint16);
typedef unsigned __int32(TTAuint32);
typedef unsigned __int64(TTAuint64);

static const int MAX_PATHLEN = MAX_PATH/*/8192/**/;

static const int PCM_BUFFER_LENGTH = 5210;
static const TTAuint64 MAX_SAMPLES = 4294967295;
static const int TTA_FIFO_BUFFER_SIZE = 5120;
static const int MAX_DEPTH = 3;
static const int MAX_BPS = (MAX_DEPTH * 8);
static const int MIN_BPS = 16;
static const int MAX_NCH = 6;

// TTA audio format
static const int TTA_FORMAT_SIMPLE = 1;
static const int TTA_FORMAT_ENCRYPTED = 2;

#define TTA_ALIGNED(n) __declspec(align(n))

typedef enum tta_error {
	TTA_NO_ERROR,	// no known errors found
	TTA_OPEN_ERROR,	// can't open file
	TTA_FORMAT_ERROR,	// not compatible file format
	TTA_FILE_ERROR,	// file is corrupted
	TTA_READ_ERROR,	// can't read from input file
	TTA_WRITE_ERROR,	// can't write to output file
	TTA_SEEK_ERROR,	// file seek error
	TTA_MEMORY_ERROR,	// insufficient memory available
	TTA_PASSWORD_ERROR,	// password protected file
	TTA_NOT_SUPPORTED	// unsupported architecture
} TTA_CODEC_STATUS;


/////////////////////// TTA encoder functions /////////////////////////
class AudioCoderTTA : public AudioCoder
{
public:

	typedef enum {
		CPU_ARCH_UNDEFINED,
		CPU_ARCH_IX86_SSE2,
		CPU_ARCH_IX86_SSE3,
		CPU_ARCH_IX86_SSE4_1,
		CPU_ARCH_IX86_SSE4_2
	} CPU_ARCH_TYPE;

	typedef struct {
		TTAuint32 format;	// audio format
		TTAuint32 nch;	// number of channels
		TTAuint32 bps;	// bits per sample
		TTAuint32 sps;	// samplerate (sps)
		TTAuint32 samples;	// data length in samples
	} TTA_ALIGNED(16) TTA_info;

	typedef struct {
		TTAint32 index;
		TTAint32 error;
		TTAint32 round;
		TTAint32 shift;
		TTAint32 qm[8];
		TTAint32 dx[24];
		TTAint32 dl[24];
	} TTA_ALIGNED(16) TTA_fltst;

	typedef struct {
		TTAuint32 k0;
		TTAuint32 k1;
		TTAuint32 sum0;
		TTAuint32 sum1;
	} TTA_ALIGNED(16) TTA_adapt;

	typedef struct {
		TTA_fltst fst;
		TTA_adapt rice;
		TTAint32 prev;
	} TTA_ALIGNED(16) TTA_codec;

	typedef struct {
		TTAuint8 buffer[TTA_FIFO_BUFFER_SIZE];
		TTAuint8 end;
		TTAuint8 *pos;
		TTAuint32 bcount; // count of bits in cache
		TTAuint32 bcache; // bit cache
		TTAuint32 crc;
		TTAuint32 count;
	} TTA_ALIGNED(16) TTA_fifo;

	AudioCoderTTA(int nch, int srate, int bps);
	virtual int Encode(int framepos, void *in0, int in_avail, int *in_used, void *out0, int out_avail); //returns bytes in out
	virtual ~AudioCoderTTA();

	/* internal public functions */
	void PrepareToFinish();
	void FinishAudio(const wchar_t *filename);
	//void FinishAudio(const char *filename);
	int GetLastError() { return error; }

	void init_set_info(TTAuint64 pos);
	void set_password(void const *pstr, TTAuint32 len);
	void frame_reset(TTAuint32 frame);
	void process_stream();
	void process_frame(TTAuint8 *input, TTAuint32 in_bytes);
	TTAuint32 get_rate();

protected:
	TTA_codec encoder[MAX_NCH]; // encoder (1 per channel)
	TTAint8 data[8];	// encoder initialization data
	TTA_fifo fifo;
	TTA_codec *encoder_last;
	TTAuint64 *seek_table; // the playing position table
	TTAuint32 format;	// tta data format
	TTAuint32 rate;	// bitrate (kbps)
	TTAuint64 header_offset;	// data start position (header size, bytes)
	TTAuint64 header_and_seektable_offset;	// data start position (header and seektable size, bytes)
	TTAuint32 frames;	// total count of frames
	TTAuint32 depth;	// bytes per sample
	TTAuint32 flen_std;	// default frame length in samples
	TTAuint32 flen_last;	// last frame length in samples
	TTAuint32 flen;	// current frame length in samples
	TTAuint32 fnum;	// currently playing frame index
	TTAuint32 fpos;	// the current position in frame
	TTAuint32 shift_bits; // packing int to pcm
	TTA_info info;

	int error;
	int lastblock;
	int samplecount;
	int blockfilled;
	int smp_size;


	typedef struct _buffer {
		size_t	data_length;
		size_t	current_pos;
		size_t	current_end_pos;
		TTAuint8   *buffer;
	} data_buf;

	data_buf remain_data_buffer;
	data_buf input_data_buffer;

	void write_seek_table();
	void frame_init(TTAuint32 frame);
	void filter_init(TTA_fltst *fs, TTAint8 *data, TTAint32 shift);
	void writer_start();
	void writer_reset();
	void writer_done();
	__forceinline void write_byte(TTAuint32 value);
	__forceinline void write_uint16(TTAuint32 value);
	__forceinline void write_uint32(TTAuint32 value);
	__forceinline void write_crc32();
	void writer_skip_bytes(TTAuint32 size);
	TTAuint32 write_tta_header();
	__forceinline void put_value(TTA_adapt *rice, TTAint32 value);
	__forceinline void flush_bit_cache();
	__forceinline int write_output(TTAuint8 *out, int out_avail, int out_used_total);
	void data_buf_free(data_buf *databuf);

}; // class AudioCoderTTA

//////////////////////// TTA exception class //////////////////////////
class AudioCoderTTA_exception : public std::exception
{
	tta_error err_code;

public:
	AudioCoderTTA_exception(tta_error code) : err_code(code) {}
	tta_error code() const { return err_code; }
}; // class AudioCoderTTA_exception


