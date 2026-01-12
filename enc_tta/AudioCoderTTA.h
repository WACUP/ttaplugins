/*
The ttaplugins-winamp project.
Copyright (C) 2005-2026 Yamagata Fumihiro

This file is part of enc_tta.

enc_tta is free software: you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation, either
version 3 of the License, or any later version.

enc_tta is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along with enc_tta.
If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef AUDIOCODERTTA_H_INCLUDED
#define AUDIOCODERTTA_H_INCLUDED


#include <nsv/enc_if.h>
#include <memory.h>
#include <malloc.h>
#include <windows.h>
#include <stdexcept>
#include <stdlib.h>
#include <libtta.h>

#include <tta_encoder_extend.h>

static const int MAX_PATHLEN = MAX_PATH/*/8192/**/;
static const int PCM_BUFFER_LENGTH = 5210;

struct data_buf
{
	size_t	data_length;
	size_t	current_pos;
	size_t	current_end_pos;
	TTAuint8* buffer;
};

struct TTA_io_callback_wrapper
{
	TTA_io_callback iocb{};
	data_buf remain_data_buffer{};
};

/////////////////////// TTA encoder functions /////////////////////////
class AudioCoderTTA : public AudioCoder
{
public:
	AudioCoderTTA();
	AudioCoderTTA(int nch, int srate, int bps);
	int Encode(int framepos, void *in0, int in_avail, int *in_used, void *out0, int out_avail) override; //returns bytes in out
	virtual ~AudioCoderTTA();

	/* internal public functions */
	void PrepareToFinish();
	void FinishAudio(const wchar_t *filename);
	//void FinishAudio(const char *filename);
	int GetLastError() { return m_error; }

protected:
	__forceinline int write_output(TTAuint8* out, int out_avail, int out_used_total);
	void data_buf_free(data_buf* databuf);

private:
	int m_buffer_size = 0;

protected:
	int m_error = 0;
	TTA_info m_info = {};

	int m_lastblock = 0;
	TTAuint32 m_samplecount = 0;
	int m_smp_size = 0;

private:
#ifndef _WIN64
	tta::tta_encoder_extend* m_TTA = nullptr;
#endif
	alignas(16) TTA_io_callback_wrapper m_iocb_wrapper ={};
#ifdef _WIN64	// memory hole after this under x86
	tta::tta_encoder_extend* m_TTA = nullptr;
#endif
	alignas(tta::tta_encoder_extend) std::byte m_ttaenc_mem[sizeof(tta::tta_encoder_extend)] = {};

}; // class AudioCoderTTA

//////////////////////// TTA exception class //////////////////////////
class AudioCoderTTA_exception : public std::exception
{
	tta_error m_err_code;

public:
	AudioCoderTTA_exception(tta_error code) : m_err_code(code) {}
	tta_error code() const { return m_err_code; }
}; // class AudioCoderTTA_exception

#endif // #ifndef AUDIOCODERTTA_H_INCLUDED

