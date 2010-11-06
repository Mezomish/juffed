// Utf8_16.cxx
// Copyright (C) 2002 Scott Kirkwood
//
// Permission to use, copy, modify, distribute and sell this code
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies or
// any derived copies.  Scott Kirkwood makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

#include "Utf8_16.h"

#include <stdio.h>

const Utf8_16::utf8 Utf8_16::k_Boms[][3] = {
	{0x00, 0x00, 0x00},  // Unknown
	{0xFE, 0xFF, 0x00},  // Big endian
	{0xFF, 0xFE, 0x00},  // Little endian
	{0xEF, 0xBB, 0xBF}, // UTF8
};

enum { SURROGATE_LEAD_FIRST = 0xD800 };
enum { SURROGATE_LEAD_LAST = 0xDBFF };
enum { SURROGATE_TRAIL_FIRST = 0xDC00 };
enum { SURROGATE_TRAIL_LAST = 0xDFFF };
enum { SURROGATE_FIRST_VALUE = 0x10000 };

// ==================================================================

Utf8_16_Read::Utf8_16_Read() {
	m_eEncoding = eUnknown;
	m_nBufSize = 0;
	m_pBuf = NULL;
	m_pNewBuf = NULL;
	m_bFirstRead = true;
	m_nLen = 0;
}

Utf8_16_Read::~Utf8_16_Read() {
	if ((m_eEncoding != eUnknown) && (m_eEncoding != eUtf8)) {
		delete [] m_pNewBuf;
		m_pNewBuf = NULL;
	}
}

size_t Utf8_16_Read::convert(char* buf, size_t len) {
	m_pBuf = reinterpret_cast<ubyte*>(buf);
	m_nLen = len;

	int nSkip = 0;
	if (m_bFirstRead) {
		nSkip = determineEncoding();
		m_bFirstRead = false;
	}

	if (m_eEncoding == eUnknown) {
		// Do nothing, pass through
		m_nBufSize = 0;
		m_pNewBuf = m_pBuf;
		return len;
	}

	if (m_eEncoding == eUtf8) {
		// Pass through after BOM
		m_nBufSize = 0;
		m_pNewBuf = m_pBuf + nSkip;
		return len - nSkip;
	}

	// Else...
	size_t newSize = len + len / 2 + 1;
	if (m_nBufSize != newSize) {
		delete [] m_pNewBuf;
		m_pNewBuf = NULL;
		m_pNewBuf = new ubyte[newSize];
		m_nBufSize = newSize;
	}

	ubyte* pCur = m_pNewBuf;

	m_Iter16.set(m_pBuf + nSkip, len - nSkip, m_eEncoding);

	for (; m_Iter16; ++m_Iter16) {
		*pCur++ = m_Iter16.get();
	}

	// Return number of bytes writen out
	return pCur - m_pNewBuf;
}

int Utf8_16_Read::determineEncoding() {
	m_eEncoding = eUnknown;

	int nRet = 0;

	if (m_nLen > 1) {
		if (m_pBuf[0] == k_Boms[eUtf16BigEndian][0] && m_pBuf[1] == k_Boms[eUtf16BigEndian][1]) {
			m_eEncoding = eUtf16BigEndian;
			nRet = 2;
		} else if (m_pBuf[0] == k_Boms[eUtf16LittleEndian][0] && m_pBuf[1] == k_Boms[eUtf16LittleEndian][1]) {
			m_eEncoding = eUtf16LittleEndian;
			nRet = 2;
		} else if (m_nLen > 2 && m_pBuf[0] == k_Boms[eUtf8][0] && m_pBuf[1] == k_Boms[eUtf8][1] && m_pBuf[2] == k_Boms[eUtf8][2]) {
			m_eEncoding = eUtf8;
			nRet = 3;
		}
	}

	return nRet;
}

// ==================================================================

Utf8_16_Write::Utf8_16_Write() {
	m_eEncoding = eUnknown;
	m_pFile = NULL;
	m_pBuf = NULL;
	m_bFirstWrite = true;
	m_nBufSize = 0;
}

Utf8_16_Write::~Utf8_16_Write() {
	if (m_pFile) {
		fclose();
	}
}

void Utf8_16_Write::setfile(FILE *pFile) {
	m_pFile = pFile;

	m_bFirstWrite = true;
}

// Swap the two low order bytes of an integer value
static int swapped(int v) {
	return ((v & 0xFF) << 8) + (v >> 8);
}

size_t Utf8_16_Write::fwrite(const void* p, size_t _size) {
	if (!m_pFile) {
		return 0; // fail
	}

	if (m_eEncoding == eUnknown) {
		// Normal write
		return ::fwrite(p, _size, 1, m_pFile);
	}

	if (m_eEncoding == eUtf8) {
		if (m_bFirstWrite)
			::fwrite(k_Boms[m_eEncoding], 3, 1, m_pFile);
		m_bFirstWrite = false;
		return ::fwrite(p, _size, 1, m_pFile);
	}

	if (_size > m_nBufSize) {
		m_nBufSize = _size;
		delete [] m_pBuf;
		m_pBuf = NULL;
		m_pBuf = new utf16[_size + 1];
	}

	if (m_bFirstWrite) {
		if (m_eEncoding == eUtf16BigEndian || m_eEncoding == eUtf16LittleEndian) {
			// Write the BOM
			::fwrite(k_Boms[m_eEncoding], 2, 1, m_pFile);
		}

		m_bFirstWrite = false;
	}

	Utf8_Iter iter8;
	iter8.set(static_cast<const ubyte*>(p), _size, m_eEncoding);

	utf16* pCur = m_pBuf;

	for (; iter8; ++iter8) {
		if (iter8.canGet()) {
			int codePoint = iter8.get();
			if (codePoint >= SURROGATE_FIRST_VALUE) {
				codePoint -= SURROGATE_FIRST_VALUE;
				int lead = (codePoint >> 10) + SURROGATE_LEAD_FIRST;
				*pCur++ = static_cast<utf16>((m_eEncoding == eUtf16BigEndian) ?
					swapped(lead) : lead);
				int trail = (codePoint & 0x3ff) + SURROGATE_TRAIL_FIRST;
				*pCur++ = static_cast<utf16>((m_eEncoding == eUtf16BigEndian) ?
					swapped(trail) : trail);
			} else {
				*pCur++ = static_cast<utf16>((m_eEncoding == eUtf16BigEndian) ?
					swapped(codePoint) : codePoint);
			}
		}
	}

	size_t ret = ::fwrite(m_pBuf, (const char*)pCur - (const char*)m_pBuf, 1, m_pFile);

	return ret;
}

void Utf8_16_Write::fclose() {
	delete [] m_pBuf;
	m_pBuf = NULL;

	::fclose(m_pFile);
	m_pFile = NULL;
}

void Utf8_16_Write::setEncoding(Utf8_16::encodingType eType) {
	m_eEncoding = eType;
}

//=================================================================
Utf8_Iter::Utf8_Iter() {
	reset();
}

void Utf8_Iter::reset() {
	m_pBuf = NULL;
	m_pRead = NULL;
	m_pEnd = NULL;
	m_eState = eStart;
	m_nCur = 0;
	m_eEncoding = eUnknown;
}

void Utf8_Iter::set
	(const ubyte* pBuf, size_t nLen, encodingType eEncoding) {
	m_pBuf = pBuf;
	m_pRead = pBuf;
	m_pEnd = pBuf + nLen;
	m_eEncoding = eEncoding;
	operator++();
	// Note: m_eState, m_nCur not reset
}
// Go to the next byte.
void Utf8_Iter::operator++() {
	switch (m_eState) {
	case eStart:
		if ((0xF0 & *m_pRead) == 0xF0) {
			m_nCur = (0x7 & *m_pRead) << 18;
			m_eState = eSecondOf4Bytes;
		} else if ((0xE0 & *m_pRead) == 0xE0) {
			m_nCur = (~0xE0 & *m_pRead) << 12;
			m_eState = ePenultimate;
		} else if ((0xC0 & *m_pRead) == 0xC0) {
			m_nCur = (~0xC0 & *m_pRead) << 6;
			m_eState = eFinal;
		} else {
			m_nCur = *m_pRead;
			toStart();
		}
		break;
	case eSecondOf4Bytes:
		m_nCur |= (0x3F & *m_pRead) << 12;
		m_eState = ePenultimate;
		break;
	case ePenultimate:
		m_nCur |= (0x3F & *m_pRead) << 6;
		m_eState = eFinal;
		break;
	case eFinal:
		m_nCur |= static_cast<utf8>(0x3F & *m_pRead);
		toStart();
		break;
	}
	++m_pRead;
}

void Utf8_Iter::toStart() {
	m_eState = eStart;
}

//==================================================
Utf16_Iter::Utf16_Iter() {
	reset();
}

void Utf16_Iter::reset() {
	m_pBuf = NULL;
	m_pRead = NULL;
	m_pEnd = NULL;
	m_eState = eStart;
	m_nCur = 0;
	m_nCur16 = 0;
	m_eEncoding = eUnknown;
}

void Utf16_Iter::set
	(const ubyte* pBuf, size_t nLen, encodingType eEncoding) {
	m_pBuf = pBuf;
	m_pRead = pBuf;
	m_pEnd = pBuf + nLen;
	m_eEncoding = eEncoding;
	operator++();
	// Note: m_eState, m_nCur, m_nCur16 not reinitalized.
}

// Goes to the next byte.
// Not the next symbol which you might expect.
// This way we can continue from a partial buffer that doesn't align
void Utf16_Iter::operator++() {
	switch (m_eState) {
	case eStart:
		if (m_pRead >= m_pEnd) {
			++m_pRead;
			break;
		}
		if (m_eEncoding == eUtf16LittleEndian) {
			m_nCur16 = *m_pRead++;
			m_nCur16 |= static_cast<utf16>(*m_pRead << 8);
		} else {
			m_nCur16 = static_cast<utf16>(*m_pRead++ << 8);
			m_nCur16 |= *m_pRead;
		}
		if (m_nCur16 >= SURROGATE_LEAD_FIRST && m_nCur16 <= SURROGATE_LEAD_LAST) {
			++m_pRead;
			int trail;
			if (m_eEncoding == eUtf16LittleEndian) {
				trail = *m_pRead++;
				trail |= static_cast<utf16>(*m_pRead << 8);
			} else {
				trail = static_cast<utf16>(*m_pRead++ << 8);
				trail |= *m_pRead;
			}
			m_nCur16 = (((m_nCur16 & 0x3ff) << 10) | (trail & 0x3ff)) + SURROGATE_FIRST_VALUE;
		}
		++m_pRead;

		if (m_nCur16 < 0x80) {
			m_nCur = static_cast<ubyte>(m_nCur16 & 0xFF);
			m_eState = eStart;
		} else if (m_nCur16 < 0x800) {
			m_nCur = static_cast<ubyte>(0xC0 | m_nCur16 >> 6);
			m_eState = eFinal;
		} else if (m_nCur16 < SURROGATE_FIRST_VALUE) {
			m_nCur = static_cast<ubyte>(0xE0 | m_nCur16 >> 12);
			m_eState = ePenultimate;
		} else {
			m_nCur = static_cast<ubyte>(0xF0 | m_nCur16 >> 18);
			m_eState = eSecondOf4Bytes;
		}
		break;
	case eSecondOf4Bytes:
		m_nCur = static_cast<ubyte>(0x80 | ((m_nCur16 >> 12) & 0x3F));
		m_eState = ePenultimate;
		break;
	case ePenultimate:
		m_nCur = static_cast<ubyte>(0x80 | ((m_nCur16 >> 6) & 0x3F));
		m_eState = eFinal;
		break;
	case eFinal:
		m_nCur = static_cast<ubyte>(0x80 | (m_nCur16 & 0x3F));
		m_eState = eStart;
		break;
	}
}
