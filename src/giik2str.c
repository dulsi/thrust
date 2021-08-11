/* $Id: giik2str.c,v 1.1.1.1 2006/01/09 13:52:08 peda Exp $
******************************************************************************

   Conversion routine from GII sym/label to string.

   Copyright (C) 1999 Marcus Sundberg	[marcus@ggi-project.org]

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
   THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************
*/

#include <stdio.h>
#include <ctype.h>

#include <ggi/events.h>

/* Hey, this function never claimed to be threadsafe... */
static char retbuffer[32]; /* More than enough to hold an hex 32bit int */

const char *giik2str(uint32_t giik, int issym);

const char *
giik2str(uint32_t giik, int issym)
{
	switch (giik) {
	case GIIK_F0:	return "F0";
	case GIIK_F1:	return "F1";
	case GIIK_F2:	return "F2";
	case GIIK_F3:	return "F3";
	case GIIK_F4:	return "F4";
	case GIIK_F5:	return "F5";
	case GIIK_F6:	return "F6";
	case GIIK_F7:	return "F7";
	case GIIK_F8:	return "F8";
	case GIIK_F9:	return "F9";
	case GIIK_F10:	return "F10";
	case GIIK_F11:	return "F11";
	case GIIK_F12:	return "F12";
	case GIIK_F13:	return "F13";
	case GIIK_F14:	return "F14";
	case GIIK_F15:	return "F15";
	case GIIK_F16:	return "F16";
	case GIIK_F17:	return "F17";
	case GIIK_F18:	return "F18";
	case GIIK_F19:	return "F19";
	case GIIK_F20:	return "F20";
	case GIIK_F21:	return "F21";
	case GIIK_F22:	return "F22";
	case GIIK_F23:	return "F23";
	case GIIK_F24:	return "F24";
	case GIIK_F25:	return "F25";
	case GIIK_F26:	return "F26";
	case GIIK_F27:	return "F27";
	case GIIK_F28:	return "F28";
	case GIIK_F29:	return "F29";
	case GIIK_F30:	return "F30";
	case GIIK_F31:	return "F31";
	case GIIK_F32:	return "F32";
	case GIIK_F33:	return "F33";
	case GIIK_F34:	return "F34";
	case GIIK_F35:	return "F35";
	case GIIK_F36:	return "F36";
	case GIIK_F37:	return "F37";
	case GIIK_F38:	return "F38";
	case GIIK_F39:	return "F39";
	case GIIK_F40:	return "F40";
	case GIIK_F41:	return "F41";
	case GIIK_F42:	return "F42";
	case GIIK_F43:	return "F43";
	case GIIK_F44:	return "F44";
	case GIIK_F45:	return "F45";
	case GIIK_F46:	return "F46";
	case GIIK_F47:	return "F47";
	case GIIK_F48:	return "F48";
	case GIIK_F49:	return "F49";
	case GIIK_F50:	return "F50";
	case GIIK_F51:	return "F51";
	case GIIK_F52:	return "F52";
	case GIIK_F53:	return "F53";
	case GIIK_F54:	return "F54";
	case GIIK_F55:	return "F55";
	case GIIK_F56:	return "F56";
	case GIIK_F57:	return "F57";
	case GIIK_F58:	return "F58";
	case GIIK_F59:	return "F59";
	case GIIK_F60:	return "F60";
	case GIIK_F61:	return "F61";
	case GIIK_F62:	return "F62";
	case GIIK_F63:	return "F63";
	case GIIK_F64:	return "F64";

	case GIIK_VOID:	return "VOID";

	case GIIK_Enter:	return "Enter";
	case GIIK_Delete:	return "Delete";
	case GIIK_ScrollForw:	return "ScrollForw";
	case GIIK_ScrollBack:	return "ScrollBack";

	case GIIK_Break:	return "Break";
	case GIIK_Boot:		return "Boot";
	case GIIK_Compose:	return "Compose";
	case GIIK_SAK:		return "SAK";

	case GIIK_Undo:		return "Undo";
	case GIIK_Redo:		return "Redo";
	case GIIK_Menu:		return "Menu";
	case GIIK_Cancel:	return "Cancel";
	case GIIK_PrintScreen:	return "PrintScreen";
	case GIIK_Execute:	return "Execute";
	case GIIK_Find:		return "Find";
	case GIIK_Begin:	return "Begin";
	case GIIK_Clear:	return "Clear";
	case GIIK_Insert:	return "Insert";
	case GIIK_Select:	return "Select";
	case GIIK_Macro:	return "Macro";
	case GIIK_Help:		return "Help";
	case GIIK_Do:		return "Do";
	case GIIK_Pause:	return "Pause";
	case GIIK_SysRq:	return "SysRq";
	case GIIK_ModeSwitch:	return "ModeSwitch";

	case GIIK_Up:		return "Up";
	case GIIK_Down:		return "Down";
	case GIIK_Left:		return "Left";
	case GIIK_Right:	return "Right";
	case GIIK_PageUp:	return "PageUp";
	case GIIK_PageDown:	return "PageDown";
	case GIIK_Home:		return "Home";
	case GIIK_End:		return "End";

	case GIIK_P0:	return "KP_0";
	case GIIK_P1:	return "KP_1";
	case GIIK_P2:	return "KP_2";
	case GIIK_P3:	return "KP_3";
	case GIIK_P4:	return "KP_4";
	case GIIK_P5:	return "KP_5";
	case GIIK_P6:	return "KP_6";
	case GIIK_P7:	return "KP_7";
	case GIIK_P8:	return "KP_8";
	case GIIK_P9:	return "KP_9";
	case GIIK_PA:	return "KP_A";
	case GIIK_PB:	return "KP_B";
	case GIIK_PC:	return "KP_C";
	case GIIK_PD:	return "KP_D";
	case GIIK_PE:	return "KP_E";
	case GIIK_PF:	return "KP_F";

	case GIIK_PPlus:	return "KP_Plus";
	case GIIK_PMinus:	return "KP_Minus";
	case GIIK_PAsterisk:	return "KP_Asterisk";
	case GIIK_PSlash:	return "KP_Slash";
	case GIIK_PEnter:	return "KP_Enter";
	case GIIK_PPlusMinus:	return "KP_PlusMinus";
	case GIIK_PParenLeft:	return "KP_ParenLeft";
	case GIIK_PParenRight:	return "KP_ParenRight";
	case GIIK_PSpace:	return "KP_Space";
	case GIIK_PTab:		return "KP_Tab";
	case GIIK_PBegin:	return "KP_Begin";
	case GIIK_PEqual:	return "KP_Equal";
	case GIIK_PDecimal:	return "KP_Decimal";
	case GIIK_PSeparator:	return "KP_Separator";

	case GIIK_PF1:	return "KP_F1";
	case GIIK_PF2:	return "KP_F2";
	case GIIK_PF3:	return "KP_F3";
	case GIIK_PF4:	return "KP_F4";
	case GIIK_PF5:	return "KP_F5";
	case GIIK_PF6:	return "KP_F6";
	case GIIK_PF7:	return "KP_F7";
	case GIIK_PF8:	return "KP_F8";
	case GIIK_PF9:	return "KP_F9";

	case GIIK_Shift:
		if (issym)	return "Shift";
		else		return "ShiftL";
	case GIIK_Ctrl:
		if (issym)	return "Ctrl";
		else		return "CtrlL";
	case GIIK_Alt:
		if (issym)	return "Alt";
		else		return "AltL";
	case GIIK_Meta:
		if (issym)	return "Meta";
		else		return "MetaL";
	case GIIK_Super:
		if (issym)	return "Super";
		else		return "SuperL";
	case GIIK_Hyper:
		if (issym)	return "Hyper";
		else		return "HyperL";

	case GIIK_ShiftR:	return "ShiftR";
	case GIIK_CtrlR:	return "CtrlR";
	case GIIK_AltR:		return "AltR";
	case GIIK_MetaR:	return "MetaR";
	case GIIK_SuperR:	return "SuperR";
	case GIIK_HyperR:	return "HyperR";

	case GIIK_AltGr:	return "AltGr";
	case GIIK_Caps:		return "Caps";
	case GIIK_Num:		return "Num";
	case GIIK_Scroll:	return "Scroll";

	case GIIK_ShiftLock:	return "ShiftLock";
	case GIIK_CtrlLock:	return "CtrlLock";
	case GIIK_AltLock:	return "AltLock";
	case GIIK_MetaLock:	return "MetaLock";
	case GIIK_SuperLock:	return "SuperLock";
	case GIIK_HyperLock:	return "HyperLock";
	case GIIK_AltGrLock:	return "AltGrLock";
	case GIIK_CapsLock:	return "CapsLock";
	case GIIK_NumLock:	return "NumLock";
	case GIIK_ScrollLock:	return "ScrollLock";

	case GIIK_NIL:		return "NIL";

	case GIIUC_Nul:		return "Nul";
	case GIIUC_BackSpace:	return "BackSpace";
	case GIIUC_Tab:		return "Tab";
	case GIIUC_Linefeed:	return "Linefeed";
	case GIIUC_Escape:	return "Escape";

#ifdef GIIK_DeadRing
	case GIIK_DeadRing:		return "DeadRing";
	case GIIK_DeadCaron:		return "DeadCaron";
	case GIIK_DeadOgonek:		return "DeadOgonek";
	case GIIK_DeadIota:		return "DeadIota";
	case GIIK_DeadDoubleAcute:	return "DeadDoubleAcute";
	case GIIK_DeadBreve:		return "DeadBreve";
	case GIIK_DeadAboveDot:		return "DeadAboveDot";
	case GIIK_DeadBelowDot:		return "DeadBelowDot";
	case GIIK_DeadVoicedSound:	return "DeadVoicedSound";
	case GIIK_DeadSemiVoicedSound:	return "DeadSemiVoicedSound";
	case GIIK_DeadAcute:		return "DeadAcute";
	case GIIK_DeadCedilla:		return "DeadCedilla";
	case GIIK_DeadCircumflex:	return "DeadCircumflex";
	case GIIK_DeadDiaeresis:	return "DeadDiaeresis";
	case GIIK_DeadGrave:		return "DeadGrave";
	case GIIK_DeadTilde:		return "DeadTilde";
	case GIIK_DeadMacron:		return "DeadMacron";
#endif
		
	default:
		if (giik >= 32 && giik < 256) {
			/* Handle sym/label here */
			if (!issym) giik = toupper(giik);
			switch (giik) {
			case GIIUC_Space:	return "Space";
			case GIIUC_Exclamation:	return "Exclamation";
			case GIIUC_DoubleQuote:	return "DoubleQuote";
			case GIIUC_NumberSign:	return "NumberSign";
			case GIIUC_Dollar:	return "Dollar";
			case GIIUC_Percent:	return "Percent";
			case GIIUC_Ampersand:	return "Ampersand";
			case GIIUC_Apostrophe:	return "Apostrophe";
			case GIIUC_ParenLeft:	return "ParenLeft";
			case GIIUC_ParenRight:	return "ParenRight";
			case GIIUC_Asterisk:	return "Asterisk";
			case GIIUC_Plus:	return "Plus";
			case GIIUC_Comma:	return "Comma";
			case GIIUC_Minus:	return "Minus";
			case GIIUC_Period:	return "Period";
			case GIIUC_Slash:	return "Slash";
			case GIIUC_0:		return "0";
			case GIIUC_1:		return "1";
			case GIIUC_2:		return "2";
			case GIIUC_3:		return "3";
			case GIIUC_4:		return "4";
			case GIIUC_5:		return "5";
			case GIIUC_6:		return "6";
			case GIIUC_7:		return "7";
			case GIIUC_8:		return "8";
			case GIIUC_9:		return "9";
			case GIIUC_Colon:	return "Colon";
			case GIIUC_Semicolon:	return "Semicolon";
			case GIIUC_Less:	return "Less";
			case GIIUC_Equal:	return "Equal";
			case GIIUC_Greater:	return "Greater";
			case GIIUC_Question:	return "Question";
			case GIIUC_At:	return "At";
			case GIIUC_A:	return "A";
			case GIIUC_B:	return "B";
			case GIIUC_C:	return "C";
			case GIIUC_D:	return "D";
			case GIIUC_E:	return "E";
			case GIIUC_F:	return "F";
			case GIIUC_G:	return "G";
			case GIIUC_H:	return "H";
			case GIIUC_I:	return "I";
			case GIIUC_J:	return "J";
			case GIIUC_K:	return "K";
			case GIIUC_L:	return "L";
			case GIIUC_M:	return "M";
			case GIIUC_N:	return "N";
			case GIIUC_O:	return "O";
			case GIIUC_P:	return "P";
			case GIIUC_Q:	return "Q";
			case GIIUC_R:	return "R";
			case GIIUC_S:	return "S";
			case GIIUC_T:	return "T";
			case GIIUC_U:	return "U";
			case GIIUC_V:	return "V";
			case GIIUC_W:	return "W";
			case GIIUC_X:	return "X";
			case GIIUC_Y:	return "Y";
			case GIIUC_Z:	return "Z";
			case GIIUC_BracketLeft:	return "BracketLeft";
			case GIIUC_BackSlash:	return "BackSlash";
			case GIIUC_BracketRight:return "BracketRight";
			case GIIUC_Circumflex:	return "Circumflex";
			case GIIUC_Underscore:	return "Underscore";
			case GIIUC_Grave:	return "Grave";
			case GIIUC_a:	return "a";
			case GIIUC_b:	return "b";
			case GIIUC_c:	return "c";
			case GIIUC_d:	return "d";
			case GIIUC_e:	return "e";
			case GIIUC_f:	return "f";
			case GIIUC_g:	return "g";
			case GIIUC_h:	return "h";
			case GIIUC_i:	return "i";
			case GIIUC_j:	return "j";
			case GIIUC_k:	return "k";
			case GIIUC_l:	return "l";
			case GIIUC_m:	return "m";
			case GIIUC_n:	return "n";
			case GIIUC_o:	return "o";
			case GIIUC_p:	return "p";
			case GIIUC_q:	return "q";
			case GIIUC_r:	return "r";
			case GIIUC_s:	return "s";
			case GIIUC_t:	return "t";
			case GIIUC_u:	return "u";
			case GIIUC_v:	return "v";
			case GIIUC_w:	return "w";
			case GIIUC_x:	return "x";
			case GIIUC_y:	return "y";
			case GIIUC_z:	return "z";
			case GIIUC_BraceLeft:	return "BraceLeft";
			case GIIUC_Bar:		return "Bar";
			case GIIUC_BraceRight:	return "BraceRight";
			case GIIUC_Tilde:	return "Tilde";

			case GIIUC_NoBreakSpace:return "NoBreakSpace";
			case GIIUC_ExclamDown:	return "ExclamDown";
			case GIIUC_Cent:	return "Cent";
			case GIIUC_Sterling:	return "Sterling";
			case GIIUC_Currency:	return "Currency";
			case GIIUC_Yen:		return "Yen";
			case GIIUC_BrokenBar:	return "BrokenBar";
			case GIIUC_Section:	return "Section";
			case GIIUC_Diaeresis:	return "Diaeresis";
			case GIIUC_Copyright:	return "Copyright";
			case GIIUC_OrdFeminine:	return "OrdFeminine";
			case GIIUC_GuillemotLeft:return "GuillemotLeft";
			case GIIUC_NotSign:	return "NotSign";
			case GIIUC_SoftHyphen:	return "SoftHyphen";
			case GIIUC_Registered:	return "Registered";
			case GIIUC_Macron:	return "Macron";
			case GIIUC_Degree:	return "Degree";
			case GIIUC_PlusMinus:	return "PlusMinus";
			case GIIUC_TwoSuperior:	return "TwoSuperior";
			case GIIUC_ThreeSuperior:return "ThreeSuperior";
			case GIIUC_Acute:	return "Acute";
			case GIIUC_Mu:		return "Mu";
			case GIIUC_Paragraph:	return "Paragraph";
			case GIIUC_PeriodCentered:return "PeriodCentered";
			case GIIUC_Cedilla:	return "Cedilla";
			case GIIUC_OneSuperior:	return "OneSuperior";
			case GIIUC_mKuline:	return "mKuline";
			case GIIUC_GuillemotRight:return "GuillemotRight";
			case GIIUC_OneQuarter:	return "OneQuarter";
			case GIIUC_OneHalf:	return "OneHalf";
			case GIIUC_ThreeQuarters:return "ThreeQuarters";
			case GIIUC_QuestionDown:return "QuestionDown";
			case GIIUC_Agrave:	return "Agrave";
			case GIIUC_Aacute:	return "Aacute";
			case GIIUC_Acircumflex:	return "Acircumflex";
			case GIIUC_Atilde:	return "Atilde";
			case GIIUC_Adiaeresis:	return "Adiaeresis";
			case GIIUC_Aring:	return "Aring";
			case GIIUC_AE:		return "AE";
			case GIIUC_Ccedilla:	return "Ccedilla";
			case GIIUC_Egrave:	return "Egrave";
			case GIIUC_Eacute:	return "Eacute";
			case GIIUC_Ecircumflex:	return "Ecircumflex";
			case GIIUC_Ediaeresis:	return "Ediaeresis";
			case GIIUC_Igrave:	return "Igrave";
			case GIIUC_Iacute:	return "Iacute";
			case GIIUC_Icircumflex:	return "Icircumflex";
			case GIIUC_Idiaeresis:	return "Idiaeresis";
			case GIIUC_ETH:		return "ETH";
			case GIIUC_Ntilde:	return "Ntilde";
			case GIIUC_Ograve:	return "Ograve";
			case GIIUC_Oacute:	return "Oacute";
			case GIIUC_Ocircumflex:	return "Ocircumflex";
			case GIIUC_Otilde:	return "Otilde";
			case GIIUC_Odiaeresis:	return "Odiaeresis";
			case GIIUC_Multiply:	return "Multiply";
			case GIIUC_Ooblique:	return "Ooblique";
			case GIIUC_Ugrave:	return "Ugrave";
			case GIIUC_Uacute:	return "Uacute";
			case GIIUC_Ucircumflex:	return "Ucircumflex";
			case GIIUC_Udiaeresis:	return "Udiaeresis";
			case GIIUC_Yacute:	return "Yacute";
			case GIIUC_THORN:	return "THORN";
			case GIIUC_ssharp:	return "ssharp";
			case GIIUC_agrave:	return "agrave";
			case GIIUC_aacute:	return "aacute";
			case GIIUC_acircumflex:	return "acircumflex";
			case GIIUC_atilde:	return "atilde";
			case GIIUC_adiaeresis:	return "adiaeresis";
			case GIIUC_aring:	return "aring";
			case GIIUC_ae:		return "ae";
			case GIIUC_ccedilla:	return "ccedilla";
			case GIIUC_egrave:	return "egrave";
			case GIIUC_eacute:	return "eacute";
			case GIIUC_ecircumflex:	return "ecircumflex";
			case GIIUC_ediaeresis:	return "ediaeresis";
			case GIIUC_igrave:	return "igrave";
			case GIIUC_iacute:	return "iacute";
			case GIIUC_icircumflex:	return "icircumflex";
			case GIIUC_idiaeresis:	return "idiaeresis";
			case GIIUC_eth:		return "eth";
			case GIIUC_ntilde:	return "ntilde";
			case GIIUC_ograve:	return "ograve";
			case GIIUC_oacute:	return "oacute";
			case GIIUC_ocircumflex:	return "ocircumflex";
			case GIIUC_otilde:	return "otilde";
			case GIIUC_odiaeresis:	return "odiaeresis";
			case GIIUC_Division:	return "Division";
			case GIIUC_oslash:	return "oslash";
			case GIIUC_ugrave:	return "ugrave";
			case GIIUC_uacute:	return "uacute";
			case GIIUC_ucircumflex:	return "ucircumflex";
			case GIIUC_udiaeresis:	return "udiaeresis";
			case GIIUC_yacute:	return "yacute";
			case GIIUC_thorn:	return "thorn";
			case GIIUC_ydiaeresis:	return "ydiaeresis";
			}
		}

		/* Return hex integer string */
		sprintf(retbuffer, "%d", giik);
		return retbuffer;
	}
}
