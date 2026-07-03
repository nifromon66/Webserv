#ifndef HTTPSYNTAX_MACRO_HPP
# define HTTPSYNTAX_MACRO_HPP

// WhiteSpace
# define	WS_CR				0x0D // Carriage Return
# define	WS_LF				0x0A // Line Feed
# define	WS_HT				0x09 // Horizontal Tab
# define	WS_SP				0x20 // Space
# define	WS_CRLF				"\r\n"
# define	WS_CRLF_CRLF		"\r\n\r\n"

// Control
# define	CTL_START			0x00 // NULL
# define	CTL_END				0x1F // Unit Separator
# define	CTL_DEL				0x7F // Delete

// Digit
# define	DIGIT_START			0x30 // 0
# define	DIGIT_END			0x39 // 9

// Alpha
# define	ALPHA_LOW_START		0x61 // a
# define	ALPHA_LOW_END		0x7A // z
# define	ALPHA_UPP_START		0x41 // A
# define	ALPHA_UPP_END		0x5A // Z

// Hexadecimal
# define	HEX_DIGIT_START		0x30 // 0
# define	HEX_DIGIT_END		0x39 // 9
# define	HEX_ALPHA_LOW_START	0x61 // a
# define	HEX_ALPHA_LOW_END	0x66 // f
# define	HEX_ALPHA_UPP_START	0x41 // A
# define	HEX_ALPHA_UPP_END	0x46 // F

// VCHAR
# define	VCHAR_START			0x21 // !
# define	VCHAR_END			0x7E // ~

// OCTET
# define	OCTET_START			0x00 // NULL
# define	OCTET_END			0xFF // ÿ

#endif