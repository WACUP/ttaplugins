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

#include <cstdlib>
#include <memory>

#include <nsv/enc_if.h>

#include <libtta.h>

#include "AudioCoderTTA.h"
#include <tta_encoder_extend.h>

TTAint32 CALLBACK write_callback(TTA_io_callback* io, TTAuint8* buffer, TTAuint32 size)
{
	TTA_io_callback_wrapper* iocb = reinterpret_cast<TTA_io_callback_wrapper*>(io);

	if (iocb->remain_data_buffer.data_length > iocb->remain_data_buffer.current_end_pos + size)
	{
		memcpy_s(iocb->remain_data_buffer.buffer + iocb->remain_data_buffer.current_end_pos,
			iocb->remain_data_buffer.data_length - iocb->remain_data_buffer.current_end_pos, buffer, size);
		iocb->remain_data_buffer.current_end_pos += size;
		return static_cast<TTAint32>(size);
	}
	else
	{
		// Do nothing
	}
	return 0;
} // write_callback

TTAint64 CALLBACK seek_callback(TTA_io_callback* io, TTAint64 offset) {
	TTA_io_callback_wrapper* iocb = reinterpret_cast<TTA_io_callback_wrapper*>(io);

	if (iocb->remain_data_buffer.current_end_pos > (size_t)offset)
	{
		iocb->remain_data_buffer.current_pos = static_cast<size_t>(offset);
		return offset;
	}
	else
	{
		// Do nothing
	}
	return 0;
} // seek_callback

AudioCoderTTA::AudioCoderTTA() : AudioCoder()
{
}

AudioCoderTTA::AudioCoderTTA(int nch, int srate, int bps) : AudioCoder()
{

	m_iocb_wrapper.remain_data_buffer.buffer = nullptr;
	m_iocb_wrapper.remain_data_buffer.data_length = 0;
	m_iocb_wrapper.remain_data_buffer.current_pos = 0;
	m_iocb_wrapper.remain_data_buffer.current_end_pos = 0;

	m_iocb_wrapper.iocb.read = nullptr;
	m_iocb_wrapper.iocb.write = &write_callback;
	m_iocb_wrapper.iocb.seek = &seek_callback;

	//	m_smp_size = nch * bps >> 3;
	m_info.nch = static_cast<TTAuint32>(nch);
	m_info.bps = static_cast<TTAuint32>(bps);
	m_info.sps = static_cast<TTAuint32>(srate);
	m_info.format = TTA_FORMAT_SIMPLE;
	m_info.samples = 0;
	m_smp_size = nch * ((bps + 7) / 8);
	m_samplecount = 0;

	// check for supported formats
	if ((m_info.nch == 0) ||
		(m_info.nch > MAX_NCH) ||
		(m_info.bps == 0) ||
		(m_info.bps > MAX_BPS))
	{
		m_error = 1;
		//throw AudioCoderTTA_exception(TTA_FORMAT_ERROR);
	}
	else
	{
		// Do nothing
	}

	if (m_info.samples == 0)
	{
		m_info.samples = MAX_SAMPLES;
	}
	else
	{
		// Do nothing
	}

	m_iocb_wrapper.remain_data_buffer.data_length = (size_t)(PCM_BUFFER_LENGTH * m_smp_size + 4); // +4 for READ_BUFFER macro

	// allocate memory for PCM buffer
	m_iocb_wrapper.remain_data_buffer.buffer = static_cast<TTAuint8*>(_aligned_malloc(m_iocb_wrapper.remain_data_buffer.data_length, 16)); 
	if (m_iocb_wrapper.remain_data_buffer.buffer == nullptr)
	{
		throw AudioCoderTTA_exception(TTA_MEMORY_ERROR);
	}
	else
	{
		memset(m_iocb_wrapper.remain_data_buffer.buffer, 0, m_iocb_wrapper.remain_data_buffer.data_length);
	}

	// encoding unit size
	m_buffer_size = PCM_BUFFER_LENGTH * m_smp_size;

	try
	{
		m_TTA = new (&m_ttaenc_mem) tta::tta_encoder_extend(reinterpret_cast<TTA_io_callback*>(&m_iocb_wrapper));
	}

	catch (tta::tta_exception&/* ex*/)
	{
		if (nullptr != m_TTA)
		{
			reinterpret_cast<tta::tta_encoder_extend*>(m_TTA)->~tta_encoder_extend();
			m_TTA = nullptr;
			data_buf_free(&m_iocb_wrapper.remain_data_buffer);
			throw AudioCoderTTA_exception(TTA_MEMORY_ERROR);
		}
		else
		{
			// Do nothing
		}
	}
	m_TTA->init_set_info_for_memory(&m_info, 0);
}

void AudioCoderTTA::data_buf_free(data_buf *databuf)
{
	if (databuf->buffer != nullptr)
	{
		_aligned_free(databuf->buffer);
		databuf->buffer = nullptr;
	}
	else
	{
		// Do nothing
	}
	databuf->current_pos = 0;
	databuf->data_length = 0;
	databuf->current_end_pos = 0;

}

__forceinline int AudioCoderTTA::write_output(TTAuint8 *out, int out_avail, int out_used_total)
{
	int out_used = 0;

	if (m_iocb_wrapper.remain_data_buffer.current_pos < m_iocb_wrapper.remain_data_buffer.current_end_pos) // write any header
	{
		int l = min(out_avail - out_used_total, static_cast<int>(m_iocb_wrapper.remain_data_buffer.current_end_pos - m_iocb_wrapper.remain_data_buffer.current_pos));
		memcpy_s(out + out_used_total, static_cast<rsize_t>(out_avail - out_used_total), m_iocb_wrapper.remain_data_buffer.buffer + m_iocb_wrapper.remain_data_buffer.current_pos, static_cast<rsize_t>(l));
		out_used += l;
		m_iocb_wrapper.remain_data_buffer.current_pos += l;

	}
	else if (m_iocb_wrapper.remain_data_buffer.current_pos == m_iocb_wrapper.remain_data_buffer.current_end_pos)
	{
		m_iocb_wrapper.remain_data_buffer.current_pos = 0;
		m_iocb_wrapper.remain_data_buffer.current_end_pos = 0;
	}
	else
	{
		// Do nothing
	}

	return out_used;
}

int AudioCoderTTA::Encode(int framepos, void *in0, int in_avail, int *in_used, void *out0, int out_avail)
{
	int out_used_total = 0;
	int out_used = 0;
	*in_used = 0;
	TTAuint8 * in = static_cast<TTAuint8*>(in0);
	TTAuint8 * out = static_cast<TTAuint8*>(out0);

	for (;;)
	{
		out_used = write_output(out, out_avail, out_used_total);
		if (out_used)
		{
			out_used_total += out_used;
			if (out_avail == out_used_total)
			{
				break;
			}
			else
			{
				// Do nothing
			}
		}
		else // encode more
		{
			int l = min(static_cast<int>(m_buffer_size), in_avail - *in_used);
			if (l > 0 || (m_lastblock == 1 && in_avail == *in_used))
			{
				m_samplecount += l / m_smp_size;
				m_TTA->process_stream(in + *in_used, static_cast<TTAuint32>(l));
				*in_used += l;

				if (m_lastblock)
				{
					m_TTA->preliminaryFinish();
					m_lastblock = 2;
				}
				else
				{
					// Do nothing
				}
			}
			else
			{
				break;
			}
		}
	}
	return out_used_total;
}

AudioCoderTTA::~AudioCoderTTA()
{

	data_buf_free(&m_iocb_wrapper.remain_data_buffer);

	//	m_smp_size = nch * bps >> 3;
	m_info.nch = 0;
	m_info.bps = 0;
	m_info.sps = 0;
	m_info.format = TTA_FORMAT_SIMPLE;
	m_info.samples = 0;
	m_samplecount = 0;

	if (nullptr != m_TTA)
	{
		reinterpret_cast<tta::tta_encoder_extend*>(m_TTA)->~tta_encoder_extend();
		m_TTA = nullptr;
	}
	else
	{
		// Do nothing
	}

} // ~AudioCoderTTA

void AudioCoderTTA::PrepareToFinish()
{
	m_lastblock = 1;
}

void AudioCoderTTA::FinishAudio(const wchar_t *filename)
{
	m_info.samples = m_samplecount;

	std::wstring lpTempPathBuffer;
	wchar_t szTempFileName[MAX_PATHLEN];

	DWORD dwBytesWritten = 0;
	DWORD dwBytesRead = 0;
	const size_t BUFSIZE = 65536;
	TTAuint8 *chBuffer;
	chBuffer = new TTAuint8[BUFSIZE];

	memset(chBuffer, 0, BUFSIZE);
#if 0
	if (wcsnlen(filename, MAX_PATHLEN) > MAX_PATH)
	{
		throw AudioCoderTTA_exception(TTA_WRITE_ERROR);
		return;
	}
	else
	{
		// Do nothing
	}
#endif
	std::wstring temppath = std::wstring(filename);
	size_t lastposofpath;
	lastposofpath = temppath.rfind(L'\\');

	if (lastposofpath != std::string::npos)
	{
		lpTempPathBuffer = temppath.substr(0, lastposofpath).c_str();
	}
	else
	{
		throw AudioCoderTTA_exception(TTA_WRITE_ERROR);
		return;
	}

	UINT uRetVal = GetTempFileNameW(lpTempPathBuffer.c_str(), // directory for tmp files
		L"enc",     // temp file name prefix 
		0,                // create unique name 
		szTempFileName);  // buffer for name 

	if (uRetVal == 0)
	{
		throw AudioCoderTTA_exception(TTA_WRITE_ERROR);
		return;
	}
	else
	{
		// Do nothing
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hTempFile = INVALID_HANDLE_VALUE;

	hFile = CreateFileW(filename,               // file name 
		GENERIC_READ,         // open for reading 
		0,                     // do not share 
		nullptr,				   // default security 
		OPEN_EXISTING,         // existing file only 
		FILE_ATTRIBUTE_NORMAL, // normal file 
		nullptr);                 // no template 

	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw AudioCoderTTA_exception(TTA_READ_ERROR);
		return;
	}
	else
	{
		// Do nothing
	}

	BOOL fSuccess = FALSE;

	// Write header
	hTempFile = CreateFileW(szTempFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	m_TTA->init_set_info_for_memory(&m_info, 0);
	m_TTA->flushFifo();

	fSuccess = WriteFile(hTempFile, (m_iocb_wrapper.remain_data_buffer.buffer + m_iocb_wrapper.remain_data_buffer.current_pos),
		static_cast<DWORD>(m_TTA->getHeaderOffset()), &dwBytesWritten, nullptr);

	if (!fSuccess)
	{
		throw AudioCoderTTA_exception(TTA_WRITE_ERROR);
		return;
	}
	else
	{
		// Do nothing
	}

	// Write seek table
	DWORD dwRetVal = SetFilePointer(hTempFile, static_cast<LONG>(m_TTA->getHeaderOffset()), nullptr, FILE_BEGIN);
	m_iocb_wrapper.remain_data_buffer.current_pos = 0;
	m_iocb_wrapper.remain_data_buffer.current_end_pos = 0;

	m_TTA->finalize();

	fSuccess = WriteFile(hTempFile, m_iocb_wrapper.remain_data_buffer.buffer + m_iocb_wrapper.remain_data_buffer.current_pos,
		static_cast<DWORD>(m_iocb_wrapper.remain_data_buffer.current_end_pos - m_iocb_wrapper.remain_data_buffer.current_pos), &dwBytesWritten, nullptr);

	if (!fSuccess)
	{
		throw AudioCoderTTA_exception(TTA_WRITE_ERROR);
		return;
	}
	else
	{
		// Do nothing
	}

	dwRetVal = SetFilePointer(hFile, static_cast<LONG>(m_TTA->getHeaderOffset()), nullptr, FILE_BEGIN);

	do
	{
		if (ReadFile(hFile, chBuffer, BUFSIZE, &dwBytesRead, nullptr))
		{
			fSuccess = WriteFile(hTempFile, chBuffer, dwBytesRead, &dwBytesWritten, nullptr);
			if (!fSuccess)
			{
				data_buf_free(&m_iocb_wrapper.remain_data_buffer);
				throw AudioCoderTTA_exception(TTA_WRITE_ERROR);
				return;
			}
			else
			{
				// Do nothing
			}
		}
		else
		{
			data_buf_free(&m_iocb_wrapper.remain_data_buffer);
			throw AudioCoderTTA_exception(TTA_READ_ERROR);
			return;
		}
		//  Continues until the whole file is processed.
	} while (dwBytesRead == BUFSIZE);

	if (!CloseHandle(hFile))
	{
		data_buf_free(&m_iocb_wrapper.remain_data_buffer);
		throw AudioCoderTTA_exception(TTA_FILE_ERROR);
		return;
	}
	else
	{
		// Do nothing
	}

	if (!CloseHandle(hTempFile))
	{
		data_buf_free(&m_iocb_wrapper.remain_data_buffer);
		throw AudioCoderTTA_exception(TTA_FILE_ERROR);
		return;
	}
	else
	{
		// Do nothing
	}

	CopyFileW(szTempFileName, filename, FALSE);
	DeleteFileW(szTempFileName);

	data_buf_free(&m_iocb_wrapper.remain_data_buffer);
	delete[] chBuffer;
	chBuffer = nullptr;
}

/*void AudioCoderTTA::FinishAudio(const char *filename)
{
	wchar_t *wfilename = new wchar_t[MAX_PATHLEN + 1];
	size_t converted = 0;
	mbstowcs_s(&converted, wfilename, MAX_PATHLEN, filename, MAX_PATHLEN);
	FinishAudio(wfilename);
}*/