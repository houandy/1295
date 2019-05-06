#ifndef _TAR_H_
#define _TAR_H_

typedef struct posix_header_t
{					/* byte offset */
	char name[100]; /* 0 */
	char mode[8]; /* 100 */
	char uid[8]; /* 108 */
	char gid[8]; /* 116 */
	char size[12]; /* 124 */
	char mtime[12]; /* 136 */
	char chksum[8]; /* 148 */
	char typeflag; /* 156 */
	char linkname[100]; /* 157 */
	char magic[6]; /* 257 */
	char version[2]; /* 263 */
	char uname[32]; /* 265 */
	char gname[32]; /* 297 */
	char devmajor[8]; /* 329 */
	char devminor[8]; /* 337 */
	char prefix[155]; /* 345 */
						/* 500 */
	unsigned int rtk_seqnum; /*500 */
	unsigned int rtk_tarsize; /*504 */
	char rtk_signature[4]; /*508 */
} posix_header;


/* GNU tar uses 2 512-byte block of zeros as EOF */
/* All flash type need to create an blank TAR header */
/* at the end so user space program won't take garbage data */
/* on storage as legal TAR data */
#define TAR_PADDING_SZ sizeof(posix_header)*2

#endif	/* _TAR_H_ */
