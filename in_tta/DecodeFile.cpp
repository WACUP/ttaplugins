/*
The ttaplugin-winamp project.
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

#include "DecodeFile.h"
#include "libtta.h"

TTAint32 CALLBACK read_callback(TTA_io_callback *io, TTAuint8 *buffer, TTAuint32 size) {
	TTA_io_callback_wrapper *iocb = (TTA_io_callback_wrapper *)io;
	TTAint32 result;

	if (::ReadFile(iocb->handle, buffer, size, (LPDWORD)&result, NULL)) {
		return result;
	}
	return 0;
} // read_callback

TTAint32 CALLBACK write_callback(TTA_io_callback *io, TTAuint8 *buffer, TTAuint32 size) {
	TTA_io_callback_wrapper *iocb = (TTA_io_callback_wrapper *)io;
	TTAint32 result;

	if (::WriteFile(iocb->handle, buffer, size, (LPDWORD)&result, NULL)) {
		return result;
	}
	return 0;
} // write_callback

TTAint64 CALLBACK seek_callback(TTA_io_callback *io, TTAint64 offset) {
	TTA_io_callback_wrapper *iocb = (TTA_io_callback_wrapper *)io;
	return ::SetFilePointer(iocb->handle, (LONG)offset, NULL, FILE_BEGIN);
} // seek_callback

CDecodeFile::CDecodeFile(void) : paused(0), seek_needed(1), decode_pos_ms(0), bitrate(0), Filesize(0),
st_state(0), decoderFileHANDLE(INVALID_HANDLE_VALUE), TTA(NULL), signature(sig_number)
{
	iocb_wrapper.handle = INVALID_HANDLE_VALUE;
	iocb_wrapper.iocb.read = NULL;
	iocb_wrapper.iocb.seek = NULL;
	iocb_wrapper.iocb.write = NULL;

	::InitializeCriticalSection(&CriticalSection);
}

CDecodeFile::~CDecodeFile(void)
{
	::EnterCriticalSection(&CriticalSection);

	if (INVALID_HANDLE_VALUE != decoderFileHANDLE) {
		::CloseHandle(decoderFileHANDLE);
		decoderFileHANDLE = INVALID_HANDLE_VALUE;
	}

	paused = 0;
	seek_needed = -1;
	decode_pos_ms = 0;
	bitrate = 0;
	Filesize = 0;
	st_state = 0;

	iocb_wrapper.handle = INVALID_HANDLE_VALUE;
	iocb_wrapper.iocb.read = NULL;
	iocb_wrapper.iocb.seek = NULL;
	iocb_wrapper.iocb.write = NULL;

	signature = -1;

	if (NULL != TTA) {
		TTA->~tta_decoder();
		TTA = NULL;
	}

	::LeaveCriticalSection(&CriticalSection);

	::DeleteCriticalSection(&CriticalSection);
}

int CDecodeFile::SetFileName(const wchar_t *filename)
{
	::EnterCriticalSection(&CriticalSection);

	// check for required data presented
	if (!filename) {
		::LeaveCriticalSection(&CriticalSection);
		throw CDecodeFile_exception(TTA_OPEN_ERROR);
	}


	FileName = filename;
	decoderFileHANDLE = CreateFileW(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (decoderFileHANDLE == INVALID_HANDLE_VALUE || decoderFileHANDLE == NULL) {
		::LeaveCriticalSection(&CriticalSection);
		throw CDecodeFile_exception(TTA_OPEN_ERROR);
	}

	Filesize = ::GetFileSize(decoderFileHANDLE, NULL);

	iocb_wrapper.handle = decoderFileHANDLE;
	iocb_wrapper.iocb.read = &read_callback;
	iocb_wrapper.iocb.seek = &seek_callback;

	if (NULL != TTA) {
		TTA->~tta_decoder();
		TTA = NULL;
	}
	else {
		// nothing todo
	}

	try {
		TTA = new tta::tta_decoder((TTA_io_callback *)&iocb_wrapper);
		if (NULL != TTA) {
		TTA->init_get_info(&tta_info, 0);
	}
	}
	catch (tta::tta_exception &ex) {
		if (NULL != TTA) {
			reinterpret_cast<tta::tta_decoder*>(TTA)->~tta_decoder();
			TTA = NULL;
		}

		::CloseHandle(decoderFileHANDLE);
		decoderFileHANDLE = INVALID_HANDLE_VALUE;
		::LeaveCriticalSection(&CriticalSection);
		throw CDecodeFile_exception(ex.code());
	}

	paused = 0;
	decode_pos_ms = 0;
	seek_needed = -1;

	// Filesize / (total samples * number of channel) = datasize per sample [byte/sample]
	// datasize per sample * 8 * samples per sec = bitrate [bit/sec]
	bitrate = (long)(Filesize / (tta_info.samples * tta_info.nch) * 8 * tta_info.sps / 1000);

	if ((NULL != TTA) && (TTA->seek_allowed)) {
		st_state = 1;
	}
	else {
		st_state = 0;
	}

	::LeaveCriticalSection(&CriticalSection);

	return TTA_NO_ERROR;
}

long double CDecodeFile::SeekPosition(int *done)
{

	::EnterCriticalSection(&CriticalSection);

	TTAuint32 new_pos;

	if (seek_needed >= GetLengthbymsec()) {
		decode_pos_ms = GetLengthbymsec();
		*done = 1;
	}
	else {
		decode_pos_ms = seek_needed;
		seek_needed = -1;
	}

	if (NULL == TTA) {
		::LeaveCriticalSection(&CriticalSection);
		return (double)0;
	}

	try {
		TTA->set_position((TTAuint32)(decode_pos_ms / 1000.), &new_pos);
	}

	catch (tta::tta_exception &ex) {
		::LeaveCriticalSection(&CriticalSection);
		throw CDecodeFile_exception(ex.code());
	}

	::LeaveCriticalSection(&CriticalSection);

	return decode_pos_ms;
}

int  CDecodeFile::GetSamples(BYTE *buffer, TTAuint32 buffersize)
{
	int skip_len = 0;
	int len = 0;


	if (INVALID_HANDLE_VALUE == decoderFileHANDLE || NULL == buffer || 0 == buffersize)
	{
		return 0; // no decode data
	}

	::EnterCriticalSection(&CriticalSection);

	if (NULL == TTA)
	{
		::LeaveCriticalSection(&CriticalSection);
		throw CDecodeFile_exception(TTA_MEMORY_ERROR);
	}

	try {
		len = TTA->process_stream(buffer, buffersize);
	}

	catch (tta::tta_exception &ex) {
		::LeaveCriticalSection(&CriticalSection);
		throw CDecodeFile_exception(ex.code());
	}

	if (len != 0) {
		skip_len += len;
		decode_pos_ms += (__int32)(skip_len * 1000. / tta_info.sps);
	}

	::LeaveCriticalSection(&CriticalSection);

	return len;

}

void CDecodeFile::SetOutputBPS(unsigned long bps)
{
	::EnterCriticalSection(&CriticalSection);

	if (NULL == TTA) {
		::LeaveCriticalSection(&CriticalSection);
		throw CDecodeFile_exception(TTA_MEMORY_ERROR);
	}

	try {
		tta_info.bps = bps;
		TTA->init_set_info(&tta_info);
	}

	catch (tta::tta_exception &ex) {
		::LeaveCriticalSection(&CriticalSection);
		throw CDecodeFile_exception(ex.code());
	}

	::LeaveCriticalSection(&CriticalSection);
}