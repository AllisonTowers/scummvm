/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

// PlayStation Stream demuxer and XA audio decoder based on FFmpeg/libav
// MDEC video emulation based on http://kenai.com/downloads/jpsxdec/Old/PlayStation1_STR_format1-00.txt

#include "audio/audiostream.h"
#include "audio/mixer.h"
#include "audio/decoders/raw.h"
#include "common/bitstream.h"
#include "common/huffman.h"
#include "common/memstream.h"
#include "common/stream.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "graphics/yuv_to_rgb.h"

#include "video/psx_decoder.h"

namespace Video {

PSXStreamDecoder::PSXStreamDecoder(Common::Rational frameRate) {
	assert(frameRate != 0);
	initCommon();
	_frameRate = frameRate;
	_frameCount = 0;
	_speed = kCDUnk;
}

PSXStreamDecoder::PSXStreamDecoder(CDSpeed speed, uint32 frameCount) {
	assert(speed != kCDUnk);
	assert(frameCount != 0);
	initCommon();
	_frameCount = frameCount;
	_speed = speed;
	// frame rate will be calculated in loadStream()
}

PSXStreamDecoder::~PSXStreamDecoder() {
	close();
	delete _surface;
	delete _huffman;
}

#define CODE_COUNT 113
#define HUFFVAL(z, a) ((z << 8) | a)
#define ESCAPE_CODE  ((uint32)-1) // arbitrary, just so we can tell what code it is
#define END_OF_BLOCK ((uint32)-2) // arbitrary, just so we can tell what code it is
#define GET_ZEROES(code) (code >> 8)
#define GET_AC(code) ((int)(code & 0xff))

static const uint32 s_huffmanCodes[CODE_COUNT] = {
	// Regular codes
	3, 3, 4, 5, 5, 6, 7, 4, 5, 6, 7, 4, 5, 6, 7,
	32, 33, 34, 35, 36, 37, 38, 39, 8, 9, 10, 11,
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
	23, 24, 25, 26, 27, 28, 29, 30, 31, 16, 17,
	18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
	29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23,
	24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18,
	19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31,

	// Escape code
	1,
	// End of block code
	2
};

static const byte s_huffmanLengths[CODE_COUNT] = {
	// Regular codes
	2, 3, 4, 4, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8, 10, 10, 10, 10, 10,
	10, 10, 10, 12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16,

	// Escape code
	6,
	// End of block code
	2
};

static const uint32 s_huffmanSymbols[CODE_COUNT] = {
	// Regular codes
	HUFFVAL(0, 1), HUFFVAL(1, 1), HUFFVAL(0, 2), HUFFVAL(2, 1), HUFFVAL(0, 3),
	HUFFVAL(4, 1), HUFFVAL(3, 1), HUFFVAL(7, 1), HUFFVAL(6, 1), HUFFVAL(1, 2),
	HUFFVAL(5, 1), HUFFVAL(2, 2), HUFFVAL(9, 1), HUFFVAL(0, 4), HUFFVAL(8, 1),
	HUFFVAL(13, 1), HUFFVAL(0, 6), HUFFVAL(12, 1), HUFFVAL(11, 1), HUFFVAL(3, 2),
	HUFFVAL(1, 3), HUFFVAL(0, 5), HUFFVAL(10, 1), HUFFVAL(16, 1), HUFFVAL(5, 2),
	HUFFVAL(0, 7), HUFFVAL(2, 3), HUFFVAL(1, 4), HUFFVAL(15, 1), HUFFVAL(14, 1),
	HUFFVAL(4, 2), HUFFVAL(0, 11), HUFFVAL(8, 2), HUFFVAL(4, 3), HUFFVAL(0, 10),
	HUFFVAL(2, 4), HUFFVAL(7, 2), HUFFVAL(21, 1), HUFFVAL(20, 1), HUFFVAL(0, 9),
	HUFFVAL(19, 1), HUFFVAL(18, 1), HUFFVAL(1, 5), HUFFVAL(3, 3), HUFFVAL(0, 8),
	HUFFVAL(6, 2), HUFFVAL(17, 1), HUFFVAL(10, 2), HUFFVAL(9, 2), HUFFVAL(5, 3),
	HUFFVAL(3, 4), HUFFVAL(2, 5), HUFFVAL(1, 7), HUFFVAL(1, 6), HUFFVAL(0, 15),
	HUFFVAL(0, 14), HUFFVAL(0, 13), HUFFVAL(0, 12), HUFFVAL(26, 1), HUFFVAL(25, 1),
	HUFFVAL(24, 1), HUFFVAL(23, 1), HUFFVAL(22, 1), HUFFVAL(0, 31), HUFFVAL(0, 30),
	HUFFVAL(0, 29), HUFFVAL(0, 28), HUFFVAL(0, 27), HUFFVAL(0, 26), HUFFVAL(0, 25),
	HUFFVAL(0, 24), HUFFVAL(0, 23), HUFFVAL(0, 22), HUFFVAL(0, 21), HUFFVAL(0, 20),
	HUFFVAL(0, 19), HUFFVAL(0, 18), HUFFVAL(0, 17), HUFFVAL(0, 16), HUFFVAL(0, 40),
	HUFFVAL(0, 39), HUFFVAL(0, 38), HUFFVAL(0, 37), HUFFVAL(0, 36), HUFFVAL(0, 35),
	HUFFVAL(0, 34), HUFFVAL(0, 33), HUFFVAL(0, 32), HUFFVAL(1, 14), HUFFVAL(1, 13),
	HUFFVAL(1, 12), HUFFVAL(1, 11), HUFFVAL(1, 10), HUFFVAL(1, 9), HUFFVAL(1, 8),
	HUFFVAL(1, 18), HUFFVAL(1, 17), HUFFVAL(1, 16), HUFFVAL(1, 15), HUFFVAL(6, 3),
	HUFFVAL(16, 2), HUFFVAL(15, 2), HUFFVAL(14, 2), HUFFVAL(13, 2), HUFFVAL(12, 2),
	HUFFVAL(11, 2), HUFFVAL(31, 1), HUFFVAL(30, 1), HUFFVAL(29, 1), HUFFVAL(28, 1),
	HUFFVAL(27, 1),

	// Escape code
	ESCAPE_CODE,
	// End of block code
	END_OF_BLOCK
};

void PSXStreamDecoder::initCommon() {
	_stream = 0;
	_audStream = 0;
	_surface = new Graphics::Surface();
	_yBuffer = _cbBuffer = _crBuffer = 0;
	_huffman = new Common::Huffman(0, CODE_COUNT, s_huffmanCodes, s_huffmanLengths, s_huffmanSymbols);
}

#define RAW_CD_SECTOR_SIZE 2352

#define CDXA_TYPE_MASK     0x0E
#define CDXA_TYPE_DATA     0x08
#define CDXA_TYPE_AUDIO    0x04
#define CDXA_TYPE_VIDEO    0x02

bool PSXStreamDecoder::loadStream(Common::SeekableReadStream *stream) {
	close();

	_stream = stream;

	Common::SeekableReadStream *sector = readSector();

	if (!sector) {
		close();
		return false;
	}

	// Rip out video info from the first frame
	sector->seek(18);
	byte sectorType = sector->readByte() & CDXA_TYPE_MASK;

	if (sectorType != CDXA_TYPE_VIDEO && sectorType != CDXA_TYPE_DATA) {
		close();
		return false;
	}

	sector->seek(40);

	uint16 width = sector->readUint16LE();
	uint16 height = sector->readUint16LE();
	_surface->create(width, height, g_system->getScreenFormat());

	_macroBlocksW = (width + 15) / 16;
	_macroBlocksH = (height + 15) / 16;
	_yBuffer = new byte[_macroBlocksW * _macroBlocksH * 16 * 16];
	_cbBuffer = new byte[_macroBlocksW * _macroBlocksH * 8 * 8];
	_crBuffer = new byte[_macroBlocksW * _macroBlocksH * 8 * 8];

	delete sector;
	_stream->seek(0);

	// Calculate frame rate based on CD speed
	if (_speed != kCDUnk) {
		// TODO: This algorithm is too basic and not accurate enough
		// TODO: Count the number of sectors per frame to get a better estimate
		_frameRate = Common::Rational(_speed * _frameCount, _stream->size() / RAW_CD_SECTOR_SIZE);
		_frameRate.debugPrint(0, "Approximate PSX Stream Frame Rate:");
	}

	return true;
}

void PSXStreamDecoder::close() {
	if (!_stream)
		return;

	delete _stream;
	_stream = 0;

	// Deinitialize sound
	g_system->getMixer()->stopHandle(_audHandle);
	_audStream = 0;

	_surface->free();

	memset(&_adpcmStatus, 0, sizeof(_adpcmStatus));

	_macroBlocksW = _macroBlocksH = 0;
	delete[] _yBuffer; _yBuffer = 0;
	delete[] _cbBuffer; _cbBuffer = 0;
	delete[] _crBuffer; _crBuffer = 0;

	reset();
}

uint32 PSXStreamDecoder::getElapsedTime() const {
	// TODO: Currently, the audio is always after the video so using this
	// can often lead to gaps in the audio...
	//if (_audStream)
	//	return _mixer->getSoundElapsedTime(_audHandle);

	return FixedRateVideoDecoder::getElapsedTime();
}

#define VIDEO_DATA_CHUNK_SIZE   2016
#define VIDEO_DATA_HEADER_SIZE  56

const Graphics::Surface *PSXStreamDecoder::decodeNextFrame() {
	Common::SeekableReadStream *sector = 0;
	byte *partialFrame = 0;

	while (!endOfVideo()) {
		sector = readSector();

		if (!sector)
			error("Corrupt PSX stream sector");

		sector->seek(0x11);
		byte track = sector->readByte();
		if (track >= 32)
			error("Bad PSX stream track");

		byte sectorType = sector->readByte() & CDXA_TYPE_MASK;

		switch (sectorType) {
		case CDXA_TYPE_DATA:
		case CDXA_TYPE_VIDEO:
			if (track == 1) {
				sector->seek(28);
				uint16 curSector = sector->readUint16LE();
				uint16 sectorCount = sector->readUint16LE();
				sector->readUint32LE();
				uint16 frameSize = sector->readUint32LE();

				if (curSector >= sectorCount)
					error("Bad sector");

				if (!partialFrame)
					partialFrame = (byte *)malloc(sectorCount * VIDEO_DATA_CHUNK_SIZE);

				sector->seek(VIDEO_DATA_HEADER_SIZE);
				sector->read(partialFrame + curSector * VIDEO_DATA_CHUNK_SIZE, VIDEO_DATA_CHUNK_SIZE);

				if (curSector == sectorCount - 1) {
					// Done assembling the frame
					Common::SeekableReadStream *frame = new Common::MemoryReadStream(partialFrame, frameSize, DisposeAfterUse::YES);

					decodeFrame(frame);
					
					delete frame;
					delete sector;

					_curFrame++;
					if (_curFrame == 0)
						_startTime = g_system->getMillis();

					return _surface;
				}
			} else
				error("Unhandled multi-track video");
			break;
		case CDXA_TYPE_AUDIO:
			// We only handle one audio channel so far
			if (track == 1)
				queueAudioFromSector(sector);
			else
				warning("Unhandled multi-track audio");
			break;
		default:
			// This shows up way too often, but the other sectors
			// are safe to ignore
			//warning("Unknown PSX sector type 0x%x", sectorType);
			break;
		}

		delete sector;
	}

	return 0;
}

static const byte s_syncHeader[12] = { 0x00, 0xff ,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00 };

Common::SeekableReadStream *PSXStreamDecoder::readSector() {
	assert(_stream);

	Common::SeekableReadStream *stream = _stream->readStream(RAW_CD_SECTOR_SIZE);

	byte syncHeader[12];
	stream->read(syncHeader, 12);
	if (!memcmp(s_syncHeader, syncHeader, 12))
		return stream;

	return 0;
}

// Ha! It's palindromic!
#define AUDIO_DATA_CHUNK_SIZE   2304
#define AUDIO_DATA_SAMPLE_COUNT 4032 

static const int s_xaTable[5][2] = {
   {   0,   0 },
   {  60,   0 },
   { 115, -52 },
   {  98, -55 },
   { 122, -60 }
};

void PSXStreamDecoder::queueAudioFromSector(Common::SeekableReadStream *sector) {
	assert(sector);

	if (!_audStream) {
		// Initialize audio stream
		sector->seek(19);
		byte format = sector->readByte();

		bool stereo = (format & (1 << 0)) != 0;
		uint rate = (format & (1 << 2)) ? 18900 : 37800;

		_audStream = Audio::makeQueuingAudioStream(rate, stereo);
		g_system->getMixer()->playStream(Audio::Mixer::kPlainSoundType, &_audHandle, _audStream);
	}

	sector->seek(24);

	// This XA audio is different (yet similar) from normal XA audio! Watch out!
	// TODO: It's probably similar enough to normal XA that we can merge it somehow...
	// TODO: RTZ PSX needs the same audio code in a regular AudioStream class. Probably
	// will do something similar to QuickTime and creating a base class 'ISOMode2Parser'
	// or something similar.
	byte *buf = new byte[AUDIO_DATA_CHUNK_SIZE];
	sector->read(buf, AUDIO_DATA_CHUNK_SIZE);

	int channels = _audStream->isStereo() ? 2 : 1;
	int16 *dst = new int16[AUDIO_DATA_SAMPLE_COUNT];
	int16 *leftChannel = dst;
	int16 *rightChannel = dst + 1;

	for (byte *src = buf; src < buf + AUDIO_DATA_CHUNK_SIZE; src += 128) {
		for (int i = 0; i < 4; i++) {
			int shift = 12 - (src[4 + i * 2] & 0xf);
			int filter = src[4 + i * 2] >> 4;
			int f0 = s_xaTable[filter][0];
			int f1 = s_xaTable[filter][1];
			int16 s_1 = _adpcmStatus[0].sample[0];
			int16 s_2 = _adpcmStatus[0].sample[1];

			for (int j = 0; j < 28; j++) {
				byte d = src[16 + i + j * 4];
				int t = (int8)(d << 4) >> 4;
				int s = (t << shift) + ((s_1 * f0 + s_2 * f1 + 32) >> 6);
				s_2 = s_1;
				s_1 = CLIP<int>(s, -32768, 32767);
				*leftChannel = s_1;
				leftChannel += channels;
			}

			if (channels == 2) {
				_adpcmStatus[0].sample[0] = s_1;
				_adpcmStatus[0].sample[1] = s_2;
				s_1 = _adpcmStatus[1].sample[0];
				s_2 = _adpcmStatus[1].sample[1];
			}

			shift = 12 - (src[5 + i * 2] & 0xf);
			filter = src[5 + i * 2] >> 4;
			f0 = s_xaTable[filter][0];
			f1 = s_xaTable[filter][1];

			for (int j = 0; j < 28; j++) {
				byte d = src[16 + i + j * 4];
				int t = (int8)d >> 4;
				int s = (t << shift) + ((s_1 * f0 + s_2 * f1 + 32) >> 6);
				s_2 = s_1;
				s_1 = CLIP<int>(s, -32768, 32767);

				if (channels == 2) {
					*rightChannel = s_1;
					rightChannel += 2;
				} else {
					*leftChannel++ = s_1;
				}
			}

			if (channels == 2) {
				_adpcmStatus[1].sample[0] = s_1;
				_adpcmStatus[1].sample[1] = s_2;
			} else {
				_adpcmStatus[0].sample[0] = s_1;
				_adpcmStatus[0].sample[1] = s_2;
			}
		}
	}

	int flags = Audio::FLAG_16BITS;

	if (_audStream->isStereo())
		flags |= Audio::FLAG_STEREO;

#ifdef SCUMM_LITTLE_ENDIAN
	flags |= Audio::FLAG_LITTLE_ENDIAN;
#endif

	_audStream->queueBuffer((byte *)dst, AUDIO_DATA_SAMPLE_COUNT * 2, DisposeAfterUse::YES, flags);
	delete[] buf;
}

void PSXStreamDecoder::decodeFrame(Common::SeekableReadStream *frame) {
	// A frame is essentially an MPEG-1 intra frame

	Common::BitStream16LEMSB bits(frame);

	bits.skip(16); // unknown
	bits.skip(16); // 0x3800
	uint16 scale = bits.getBits(16);
	uint16 version = bits.getBits(16);

	if (version != 2 && version != 3)
		error("Unknown PSX stream frame version");

	for (int mbX = 0; mbX < _macroBlocksW; mbX++)
		for (int mbY = 0; mbY < _macroBlocksH; mbY++)
			decodeMacroBlock(&bits, mbX, mbY, scale, version);

	// Output data onto the frame
	Graphics::convertYUV420ToRGB(_surface, _yBuffer, _cbBuffer, _crBuffer, _surface->w, _surface->h, _macroBlocksW * 16, _macroBlocksW * 8);
}

void PSXStreamDecoder::decodeMacroBlock(Common::BitStream *bits, int mbX, int mbY, uint16 scale, uint16 version) {
	int pitchY = _macroBlocksW * 16;
	int pitchC = _macroBlocksW * 8;

	// Note the strange order of red before blue
	decodeBlock(bits, _crBuffer + (mbY * pitchC + mbX) * 8, pitchC, scale, version);
	decodeBlock(bits, _cbBuffer + (mbY * pitchC + mbX) * 8, pitchC, scale, version);
	decodeBlock(bits, _yBuffer + (mbY * pitchY + mbX) * 16, pitchY, scale, version);
	decodeBlock(bits, _yBuffer + (mbY * pitchY + mbX) * 16 + 8, pitchY, scale, version);
	decodeBlock(bits, _yBuffer + (mbY * pitchY + mbX) * 16 + 8 * pitchY, pitchY, scale, version);
	decodeBlock(bits, _yBuffer + (mbY * pitchY + mbX) * 16 + 8 * pitchY + 8, pitchY, scale, version);
}

// Standard JPEG/MPEG zig zag table
static const byte s_zigZagTable[8 * 8] = {
	 0,  1,  5,  6, 14, 15, 27, 28,
	 2,  4,  7, 13, 16, 26, 29, 42,
	 3,  8, 12, 17, 25, 30, 41, 43,
	 9, 11, 18, 24, 31, 40, 44, 53,
	10, 19, 23, 32, 39, 45, 52, 54,
	20, 22, 33, 38, 46, 51, 55, 60,
	21, 34, 37, 47, 50, 56, 59, 61,
	35, 36, 48, 49, 57, 58, 62, 63
};

// One byte different from the standard MPEG-1 table
static const byte s_quantizationTable[8 * 8] = {
	 2, 16, 19, 22, 26, 27, 29, 34,
	16, 16, 22, 24, 27, 29, 34, 37,
	19, 22, 26, 27, 29, 34, 34, 38,
	22, 22, 26, 27, 29, 34, 37, 40,
	22, 26, 27, 29, 32, 35, 40, 48,
	26, 27, 29, 32, 35, 40, 48, 58,
	26, 27, 29, 34, 38, 46, 56, 69,
	27, 29, 35, 38, 46, 56, 69, 83
};

void PSXStreamDecoder::dequantizeBlock(int *coefficients, float *block, uint16 scale) {
	// Dequantize the data, un-zig-zagging as we go along
	for (int i = 0; i < 8 * 8; i++) {
		if (i == 0) // Special case for the DC coefficient
			block[i] = coefficients[i] * s_quantizationTable[i];
		else
			block[i] = (float)coefficients[s_zigZagTable[i]] * s_quantizationTable[i] * scale / 8;
	}
}

#define BLOCK_OVERFLOW_CHECK() \
	if (count > 63) \
		error("PSXStreamDecoder::readAC(): Too many coefficients")

void PSXStreamDecoder::readAC(Common::BitStream *bits, int *block) {
	// Clear the block first
	for (int i = 0; i < 63; i++)
		block[i] = 0;

	int count = 0;

	while (!bits->eos()) {
		uint32 symbol = _huffman->getSymbol(*bits);

		if (symbol == ESCAPE_CODE) {
			// The escape code!
			int zeroes = bits->getBits(6);
			count += zeroes + 1;
			BLOCK_OVERFLOW_CHECK();
			block += zeroes;
			*block++ = readSignedCoefficient(bits);
		} else if (symbol == END_OF_BLOCK) {
			// We're done
			break;
		} else {
			// Normal huffman code
			int zeroes = GET_ZEROES(symbol);
			count += zeroes + 1;
			BLOCK_OVERFLOW_CHECK();
			block += zeroes;

			if (bits->getBit())
				*block++ = -GET_AC(symbol);
			else
				*block++ = GET_AC(symbol);
		}
	}
}

int PSXStreamDecoder::readSignedCoefficient(Common::BitStream *bits) {
	uint val = bits->getBits(10);

	// extend the sign
	uint shift = 8 * sizeof(int) - 10;
	return (int)(val << shift) >> shift;
}

// IDCT table built with :
// _idct8x8[x][y] = cos(((2 * x + 1) * y) * (M_PI / 16.0)) * 0.5;
// _idct8x8[x][y] /= sqrt(2.0) if y == 0
static const double s_idct8x8[8][8] = {
	{ 0.353553390593274,  0.490392640201615,  0.461939766255643,  0.415734806151273,  0.353553390593274,  0.277785116509801,  0.191341716182545,  0.097545161008064 },
	{ 0.353553390593274,  0.415734806151273,  0.191341716182545, -0.097545161008064, -0.353553390593274, -0.490392640201615, -0.461939766255643, -0.277785116509801 },
	{ 0.353553390593274,  0.277785116509801, -0.191341716182545, -0.490392640201615, -0.353553390593274,  0.097545161008064,  0.461939766255643,  0.415734806151273 },
	{ 0.353553390593274,  0.097545161008064, -0.461939766255643, -0.277785116509801,  0.353553390593274,  0.415734806151273, -0.191341716182545, -0.490392640201615 },
	{ 0.353553390593274, -0.097545161008064, -0.461939766255643,  0.277785116509801,  0.353553390593274, -0.415734806151273, -0.191341716182545,  0.490392640201615 },
	{ 0.353553390593274, -0.277785116509801, -0.191341716182545,  0.490392640201615, -0.353553390593273, -0.097545161008064,  0.461939766255643, -0.415734806151273 },
	{ 0.353553390593274, -0.415734806151273,  0.191341716182545,  0.097545161008064, -0.353553390593274,  0.490392640201615, -0.461939766255643,  0.277785116509801 },
	{ 0.353553390593274, -0.490392640201615,  0.461939766255643, -0.415734806151273,  0.353553390593273, -0.277785116509801,  0.191341716182545, -0.097545161008064 }
};

void PSXStreamDecoder::idct(float *dequantData, float *result) {
	// IDCT code based on JPEG's IDCT code
	// TODO: Switch to the integer-based one mentioned in the docs
	// This is by far the costliest operation here

	float tmp[8 * 8];

	// Apply 1D IDCT to rows
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			tmp[y + x * 8] = dequantData[0] * s_idct8x8[x][0]
							+ dequantData[1] * s_idct8x8[x][1]
							+ dequantData[2] * s_idct8x8[x][2]
							+ dequantData[3] * s_idct8x8[x][3]
							+ dequantData[4] * s_idct8x8[x][4]
							+ dequantData[5] * s_idct8x8[x][5]
							+ dequantData[6] * s_idct8x8[x][6]
							+ dequantData[7] * s_idct8x8[x][7];
		}

		dequantData += 8;
	}

	// Apply 1D IDCT to columns
	for (int x = 0; x < 8; x++) {
		const float *u = tmp + x * 8;
		for (int y = 0; y < 8; y++) {
			result[y * 8 + x] = u[0] * s_idct8x8[y][0]
								+ u[1] * s_idct8x8[y][1]
								+ u[2] * s_idct8x8[y][2]
								+ u[3] * s_idct8x8[y][3]
								+ u[4] * s_idct8x8[y][4]
								+ u[5] * s_idct8x8[y][5]
								+ u[6] * s_idct8x8[y][6]
								+ u[7] * s_idct8x8[y][7];
		}
	}
}

void PSXStreamDecoder::decodeBlock(Common::BitStream *bits, byte *block, int pitch, uint16 scale, uint16 version) {
	int dc;

	if (version == 2) {
		dc = readSignedCoefficient(bits);
	} else {
		// TODO
		error("Unhandled PSX stream version 3 DC");
	}

	int coefficients[8 * 8];
	coefficients[0] = dc; // Start us off with the DC
	readAC(bits, &coefficients[1]); // Read in the AC

	// Dequantize
	float dequantData[8 * 8];
	dequantizeBlock(coefficients, dequantData, scale);

	// Perform IDCT
	float idctData[8 * 8];
	idct(dequantData, idctData);

	// Now output the data
	for (int y = 0; y < 8; y++) {
		byte *start = block + pitch * y;

		// Convert the result to be in the range [0, 255]
		for (int x = 0; x < 8; x++)
			*start++ = (int)CLIP<float>(idctData[y * 8 + x], -128.0f, 127.0f) + 128;
	}
}

} // End of namespace Video
