/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CHEWY_ATDS_H
#define CHEWY_ATDS_H

#include "chewy/file.h"

namespace Chewy {

#define ATDS_VOC_OFFSET 20

#define AAD_MAX_PERSON 10

#define AAD_STR_START 0
#define AAD_STR_END 1
#define AAD_DATEI 0
#define ATS_DATEI 1
#define ADS_DATEI 2
#define ADH_DATEI 3
#define INV_USE_DATEI 4
#define INV_IDX_DATEI 5
#define INV_ATS_DATEI 6

#define MAX_HANDLE 8
#define AAD_HANDLE 0
#define ATS_HANDLE 1
#define ADS_HANDLE 2
#define ADH_HANDLE 3
#define INV_USE_HANDLE 4
#define INV_IDX_HANDLE 5
#define INV_ATS_HANDLE 6
#define ATDS_HANDLE 7

enum DisplayMode {
	DISPLAY_NONE = -1,
	DISPLAY_TXT = 0,
	DISPLAY_VOC = 1,
	DISPLAY_ALL = 2
};

#define MAX_STR_SPLIT 10

#define SPLIT_MITTE 1
#define SPLIT_LEFT 2

#define ATDS_END_ZEILE 0
#define ATDS_END_BLOCK 0x0b
#define ATDS_END_EINTRAG 0x0c
#define ATDS_END_TEXT 0x0d
#define ATDS_END 0x0e
#define BLOCKENDE 0x0f

#define MAX_ATS_ROOM 100

#define ATS_AKTIV_BIT 1
#define ATS_COUNT_BIT 2
#define ATS_RESET_BIT 4
#define ATS_ACTION_BIT 8

#define MAX_ATS_STATUS 3

#define TXT_MARK_NAME 0
#define TXT_MARK_LOOK 1
#define TXT_MARK_USE 2
#define TXT_MARK_WALK 3
#define TXT_MARK_TALK 4

#define STEUERBYTE 0xff

#define MAX_ADS_DIALOG 500
#define ADS_MAX_BL_EIN 6
#define ADS_STACK_SIZE 50

#define ADS_EXIT_BIT 1
#define ADS_SHOW_BIT 2
#define ADS_RESTART_BIT 4

struct KbdMouseInfo;

struct AdsDiaHeaders {
	int16 Anz;
};

struct AdsBlock {
	bool Show[ADS_MAX_BL_EIN];

	uint8 Next[ADS_MAX_BL_EIN];
	uint8 Steuer[ADS_MAX_BL_EIN];
};

struct AtdsVar {
	int16 Silent = 0;
	int16 *Delay = 0;
	int16 DiaNr = 0;

	DisplayMode Display = DISPLAY_TXT;
	bool _eventsEnabled = false;
	int16 VocNr = 0;

	void (*aad_str)(int16 dia_nr, int16 str_nr, int16 person_nr, int16 mode) = nullptr;
};

#define IUID_IIB 0
#define IUID_SIB 1
#define IUID_REC 2
#define IUID_AUTO 3
#define IUID_DET 4
#define IUID_SPIELER 5
#define INV_USE_DEF 16
#define INV_STRC_ANZ 30

struct InvUse {
	int16 ObjId;
	int16 ObjNr;
	int16 TxtNr;

	bool load(Common::SeekableReadStream *src);
	static constexpr int SIZE() { return 6; }
};

struct AadInfo {
	int16 X;
	int16 Y;
	int16 Color;

	bool load(const void *src);
	static constexpr int SIZE() { return 6; }
};

struct AadTxtHeader {
	int16 DiaNr;
	int16 PerAnz;
	int16 AMov;
	int16 CurNr;

	bool load(const void *src);
	static constexpr int SIZE() { return 8; }
};

struct AadStrHeader {
	int16 AkPerson;
	int16 VocNr;
};

struct AadVar {
	int16 Dialog;

	AadTxtHeader *TxtHeader;
	AadStrHeader *StrHeader;
	AadInfo *Person;
	char *Ptr;
	int16 StrNr;
	int16 _delayCount;
	int16 SilentCount;
};

struct AdsTxtHeader {
	int16 DiaNr;
	int16 PerAnz;
	int16 AMov;
	int16 CurNr;
};

struct AdsVar {
	int16 Dialog;
	int16 AutoDia;
	AdsTxtHeader *TxtHeader;
	AadInfo *Person;
	char *Ptr;
	char *BlkPtr;
	int16 StrNr;
	int16 DelayCount;
	int16 SilentCount;
};

struct AdsNextBlk {
	int16 BlkNr;
	int16 EndNr;
};

struct AtsStrHeader {
	uint16 VocNr = 0;

	bool load(Common::SeekableReadStream *src);
	static constexpr int SIZE() { return 2; }
};

struct AtsTxtHeader {
	uint16 TxtNr = 0;
	int16 AMov = 0;
	int16 CurNr = 0;

	bool load(Common::SeekableReadStream *src);
	static constexpr int SIZE() { return 8; }
};

struct AtsVar {
	DisplayMode _display = DISPLAY_NONE;
	AtsTxtHeader TxtHeader;
	AtsStrHeader StrHeader;
	char *Ptr;
	int16 DelayCount;
	int16 SilentCount;
	int16 TxtLen;
	int16 Color;
	int16 TxtMode;
};

struct SplitStringRet {
	char **StrPtr;
	int16 *X;
	int16 Y;
	short Anz;
	int16 Next;

};

struct SplitStringInit {
	char *Str;

	int16 X;
	int16 Y;
	int16 Width;
	int16 Zeilen;
	int16 Mode;

	int16 Fvorx;
	int16 FHoehe;
};

class Atdsys {
public:
	Atdsys();
	~Atdsys();

	void set_delay(int16 *delay, int16 silent);
	void setHasSpeech(bool hasSpeech);
	void updateSoundSettings();
	void set_split_win(int16 nr, SplitStringInit *ssinit);
	SplitStringRet *split_string(SplitStringInit *ssi);
	void calc_txt_win(SplitStringInit *ssi);
	void str_null2leer(char *str_start, char *str_end);
	char *atds_adr(const char *fname, int16 chunk_start, int16 chunk_anz);
	void load_atds(int16 chunk_nr, int16 mode);
	void save_ads_header(int16 dia_nr);

	Common::Stream *pool_handle(const char *fname);
	void set_handle(const char *fname, int16 mode, Common::Stream *handle, int16 chunk_start, int16 chunk_anz);
	void open_handle(const char *fname, int16 mode);
	void close_handle(int16 mode);
	void crypt(char *txt, uint32 size);
	void init_ats_mode(int16 mode, uint8 *atsheader);
	DisplayMode start_ats(int16 txt_nr, int16 txt_mode, int16 color, int16 mode, int16 *voc_nr);
	void stop_ats();
	DisplayMode &ats_get_status();
	void print_ats(int16 x, int16 y, int16 scrx, int16 scry);
	int16 get_steuer_bit(int16 txt_nr, int16 bit_idx, int16 mode);
	void set_steuer_bit(int16 txt_nr, int16 bit_idx, int16 mode);
	void del_steuer_bit(int16 txt_nr, int16 bit_idx, int16 mode);
	void set_ats_str(int16 txt_nr, int16 txt_mode, int16 str_nr, int16 mode);
	void set_ats_str(int16 txt_nr, int16 str_nr, int16 mode);
	int16 get_ats_str(int16 txt_nr, int16 txt_mode, int16 mode);
	char *ats_get_txt(int16 txt_nr, int16 txt_mode, int16 *txt_anz, int16 mode);
	char *ats_search_block(int16 txt_mode, char *txt_adr);
	void ats_search_nr(int16 txt_nr, char **str);
	void ats_search_str(int16 *anz, uint8 *status, uint8 steuer, char **str);
	void set_ats_mem(int16 mode);
	int16 start_aad(int16 dia_nr);
	void stop_aad();
	void print_aad(int16 scrx, int16 scry);
	int16 aad_get_status();
	void set_string_end_func(void (*str_func)(int16 dia_nr, int16 str_nr, int16 person_nr, int16 mode));
	void aad_search_dia(int16 dia_nr, char **ptr);
	int16 aad_get_zeilen(char *str, int16 *txt_len);
	bool ads_start(int16 dia_nr);
	void stop_ads();
	char **ads_item_ptr(int16 block_nr, int16 *anzahl);
	AdsNextBlk *ads_item_choice(int16 blk_nr, int16 item_nr);
	AdsNextBlk *calc_next_block(int16 blk_nr, int16 item_nr);
	int16 ads_get_status();
	void hide_item(int16 dia_nr, int16 blk_nr, int16 item_nr);
	void show_item(int16 dia_nr, int16 blk_nr, int16 item_nr);
	int16 return_block(AdsBlock *ab);
	void ads_search_block(int16 blk_nr, char **ptr);
	void ads_search_item(int16 item_nr, char **blk_adr);
	int16 start_ads_auto_dia(char *item_adr);
	int16 check_item(int16 block_nr, int16 item_nr);
	int16 calc_inv_no_use(int16 cur_inv, int16 test_nr, int16 mode);
	int16 getStereoPos(int16 x);
	void enableEvents(bool nr) {
		_atdsv._eventsEnabled = nr;
	}
	int getAtdDisplay() const {
		return _atdsv.Display;	
	}

private:
	int16 get_delay(int16 txt_len);

	Common::Stream *_atdshandle[MAX_HANDLE] = { nullptr };
	char *_atdsmem[MAX_HANDLE] = { nullptr };
	int16 _atdspooloff[MAX_HANDLE] = { 0 };
	char *_atsmem = nullptr;
	uint8 *_ats_sheader = nullptr;
	uint8 *_ats_st_header[4];
	char *_atds_id_ptr = nullptr;
	AadVar _aadv;
	AtsVar _atsv;
	AdsVar _adsv;
	AtdsVar _atdsv;
	char *_ePtr[ADS_MAX_BL_EIN] = { nullptr };
	int16 _eNr[ADS_MAX_BL_EIN] = { 0 };
	AdsBlock *_adsBlock = nullptr;
	AdsNextBlk _adsnb;
	uint8 _adsStack[ADS_STACK_SIZE] = { 0 };
	int16 _adsStackPtr;
	SplitStringRet _ssret;
	SplitStringRet *_ssr = nullptr;
	SplitStringInit _ssi[AAD_MAX_PERSON];
	SplitStringInit _atsSsi;

	char *_splitPtr[MAX_STR_SPLIT] = { nullptr };
	int16 _splitX[MAX_STR_SPLIT] = { 0 };
	int16 _invBlockNr;
	char *_invUseMem = nullptr;
	int16 _tmpDelay;
	KbdMouseInfo *_inzeig = nullptr;
	bool _hasSpeech = false;
	int16 _mousePush = 0;
	int _printDelayCount1 = 0;
};

} // namespace Chewy

#endif
