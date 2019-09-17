#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtk_common.h"
#include "factory.h"
#include "tar.h"

static int factory_current_pp = -1;
static int factory_seq_num = 0;
char factory_dir[32];

extern long debug_level;
extern long factory_start_addr;
extern long factory_zone;
extern long align_size;
extern unsigned long mtd_erasesize;
extern long use_spi;
extern long use_emmc;
extern long use_nand;
extern char in_factorytar_file[IN_FACTORYTAR_NAMESIZE];
extern char storage_dev[64];

#define CMD_LEN 512
#define run(x...) \
	do { \
		char cmd[CMD_LEN]; \
		memset(cmd, 0, CMD_LEN); \
		snprintf(cmd, CMD_LEN, x); \
		install_log("Run... \n\"%s\"\n",cmd); \
		if (!(debug_level&INSTALL_FAKE_RUN)) \
			if (system(cmd)!=0) \
				install_fail("fail to execute \n%s\n",cmd); \
	} while(0)

int factory_find_latest_update(void)
{
	int factory_tarsize = 0;
	posix_header p0_start, p0_end;
	posix_header p1_start, p1_end;
	int pp_ok = 0;

	// get factory header from pp0 and pp1
	read_fileoffset_to_ptr(storage_dev, factory_start_addr, &p0_start, sizeof(p0_start));
	read_fileoffset_to_ptr(storage_dev, factory_start_addr + (factory_zone / 2), &p1_start, sizeof(p1_start));

	if ( strncmp(p0_start.name, "tmp/factory/", strlen("tmp/factory/")) == 0 )  {
		read_fileoffset_to_ptr(storage_dev,
		                       factory_start_addr + p0_start.rtk_tarsize,
		                       &p0_end,
		                       sizeof(p0_end));
		if ( memcmp(&p0_start, &p0_end, sizeof(p0_start)) == 0 ) {
			pp_ok = pp_ok | 1;
		}
	}
	if ( strncmp(p1_start.name, "tmp/factory/", strlen("tmp/factory/")) == 0 )  {
		read_fileoffset_to_ptr(storage_dev,
		                       factory_start_addr + (factory_zone/2) + p1_start.rtk_tarsize,
		                       &p1_end,
		                       sizeof(p1_end));
		if ( memcmp(&p1_start, &p1_end, sizeof(p1_start)) == 0 ) {
			pp_ok = pp_ok | 2;
		}
	}

	/* factory_current_pp:
	 *                   emmc addr	*
	 * 0 : factory1.tar (0x621000)	*
	 * 1 : factory2.tar (0x821000)	*/
	switch (pp_ok) {
		case 0:
			factory_current_pp = -1;
			break;
		case 1:
			factory_current_pp = 0;
			break;
		case 2:
			factory_current_pp = 1;
			break;
		case 3:
			if (p0_start.rtk_seqnum < p1_start.rtk_seqnum) {
				if ((p1_start.rtk_seqnum - p0_start.rtk_seqnum) > 0xFFFFFFF0) {
					factory_current_pp = 0;
				}
				else {
					factory_current_pp = 1;
				}
			}
			else {
				if ((p0_start.rtk_seqnum - p1_start.rtk_seqnum) > 0xFFFFFFF0) {
					factory_current_pp = 1;
				}
				else {
					factory_current_pp = 0;
				}
			}
			break;
	}

	if (factory_current_pp == 0) {
		factory_seq_num = p0_start.rtk_seqnum;
		factory_tarsize = p0_start.rtk_tarsize;
	}
	else if (factory_current_pp == 1) {
		factory_seq_num = p1_start.rtk_seqnum;
		factory_tarsize = p1_start.rtk_tarsize;
	}

	install_info("pp_ok = %x factory_current_pp = %d "
	             "factory_seq_num = %d factory_tarsize=%u\n", 
	             pp_ok,
	             factory_current_pp,
	             factory_seq_num,
	             factory_tarsize );
	return 0;
}

unsigned int factory_tar_checksum(char *header)
{
	unsigned char *data = (unsigned char *) header;
	unsigned int sum;
	int i;
	// check checksum;
	sum = 0;
	for (i=0; i<148; i++) 
		sum += data[i];

	for (i=156; i<512; i++) 
		sum += data[i];

	for (i=0; i<8; i++) 
		sum += 32;

	install_debug("tar checksum = %06o\n", sum);
	return sum;
}

void factory_tar_fill_checksum(char *in_header)
{
	posix_header* header = (posix_header*) in_header;
	sprintf(header->chksum, "%06o", factory_tar_checksum(in_header));

	header->chksum[6] = 0;
	header->chksum[7] = 0x20;
	install_debug("fill tar checksum  = %s\n", header->chksum);
}

void factory_build_header(const char *path)
{
	posix_header header;
	char buffer[2 * sizeof(header)];

	read_fileoffset_to_ptr(path, 0, &header, sizeof(header));
	header.rtk_signature[0] = 'R';
	header.rtk_signature[1] = 'T';
	header.rtk_signature[2] = 'K';
	header.rtk_signature[3] = 0;
	get_sizeof_file(path, &(header.rtk_tarsize));
	factory_seq_num = factory_seq_num + 1;
	header.rtk_seqnum = factory_seq_num;

	install_debug("header (%d bytes), rtk_tarsize %d\n", sizeof(header), header.rtk_tarsize);
	install_debug("seq_num = %u\n", header.rtk_seqnum);
	install_debug("%s\n", header.rtk_signature);
	factory_tar_fill_checksum((char *) &header);

	/* write factory.tar header to the begining of tar file */
	write_ptr_to_fileoffset(path, 0, &header, sizeof(header));
	/* write factory.tar header to the end of tar file (before padding)*/
	write_ptr_to_fileoffset(path, header.rtk_tarsize, &header, sizeof(header));
	/* GNU tar uses 2 512-byte block of zeros as EOF */
	memset(buffer, 0, sizeof(buffer));
	write_ptr_to_fileoffset(path, header.rtk_tarsize + sizeof(header), buffer, 2 * sizeof(header));
}

int factory_flush(unsigned int factory_start, unsigned int factory_part_size)
{
	unsigned int size;
	struct stat st = {0};
	long erase_size;
	// char tmp_recovery[32];

	if(factory_start == 0 || factory_part_size == 0) {
		factory_start = factory_start_addr;
		factory_part_size = factory_zone / 2;
	}

	// memset(tmp_recovery, 0, sizeof(tmp_recovery));
	// sprintf(tmp_recovery, "%s/recovery", factory_dir);
	// printf("[Installer_D]: recovery path=[%s]\n", tmp_recovery);
	// if(stat(tmp_recovery, &st) == 0) {
	// 	sprintf(cmd, "rm -rf %s", tmp_recovery);
	// 	rtk_command(cmd, __LINE__, __FILE__);
	// }

	run("ls -al /tmp/factory");

	if(stat(FACTORY_FILE_PATH, &st) == 0) {
		run("rm -rf %s", FACTORY_FILE_PATH);
	}
	run("tar cf %s %s; sync", FACTORY_FILE_PATH, factory_dir);

	install_debug("*************************\n");
	factory_build_header(FACTORY_FILE_PATH);
	install_debug("*************************\n");

	get_sizeof_file(FACTORY_FILE_PATH, &size);
	if (size > factory_part_size) {
		install_fail("factory.tar(%d) larger then default factory_part_size(%d)\n",
		             size,
		             factory_part_size);
		return -1;
	}

	if (factory_current_pp < 0) {
		factory_current_pp = 0;
	}
	else {
		factory_current_pp = factory_current_pp + 1;
		/* if pp value is 2 (0x10), pp value will back to 0 */
		factory_current_pp = factory_current_pp & 0x01;
	}
	install_info("save to current_pp = %d seq_num = %d\n",
	             factory_current_pp,
	             factory_seq_num);

	if (!use_spi && use_nand && !use_emmc)
		erase_size = (long) mtd_erasesize;
	else
		erase_size = align_size;

	if (!use_spi && !use_nand && use_emmc) {
		run("dd if=/dev/zero of=%s "
		    "bs=%ld count=%lu seek=%lu conv=notrunc,fsync",
		    storage_dev,
		    erase_size,
		    factory_part_size / erase_size,
		    (factory_start + factory_current_pp * factory_part_size) / erase_size);
	}
	else {
		run("/tmp/flash_erase %s 0x%x %ld",
		    storage_dev,
		    factory_start + factory_current_pp * factory_part_size,
		    factory_part_size / erase_size);
	}

	if (!use_spi && use_nand && !use_emmc) {
		/* nandwrite */
		run("/tmp/nandwrite -m -p -s 0x%x %s %s",
		    factory_start + factory_current_pp * factory_part_size,
		    storage_dev,
		    FACTORY_FILE_PATH);
	}
	else {
		run("dd if=%s of=%s "
		    "bs=%ld count=%lu seek=%lu",
		    FACTORY_FILE_PATH,
		    storage_dev,
		    align_size,
		    factory_part_size / align_size,
		    (factory_start + factory_current_pp * factory_part_size) / align_size );
	}

	return 0;
}

int factory_init(const char* dir)
{
	static int is_init = 0;
	if (is_init == 0) {
		install_debug("enter factory init\n");
		if (dir == NULL || strlen(dir) == 0) {
			sprintf(factory_dir, "%s", DEFAULT_FACTORY_DIR);
		}
		else {
			sprintf(factory_dir, "%s", dir);
		}
		install_debug("factory_start = 0x%x\n", factory_start_addr);
		install_debug("factory_zone = 0x%x\n", factory_zone / 2);

		run("mkdir -p %s", factory_dir);

		factory_find_latest_update();

		is_init = 1;

		return 0;
	}
	else {
		install_debug("factory has been initialized\n");
		return 1;
	}
	return -1;
}

int factory_load(void)
{
	if (factory_init(NULL) == 0)
		install_debug("factory initialization success\n");

	install_debug("entar,rm %s\r\n and mkdir %s", factory_dir , factory_dir);
	run("rm -rf %s;mkdir -p %s", factory_dir, factory_dir);

	if ( factory_current_pp != 0 && factory_current_pp != 1 ) {
		return -1;
	}

	/* untar the factory file in storage will extract directories "tmp/factory/..." */
	run("cd /;dd if=%s bs=%ld count=%ld skip=%ld | tar x", 
	    storage_dev,
	    align_size,
	    (factory_zone/2) / align_size,
	    (factory_start_addr + factory_current_pp * (factory_zone/2)) / align_size);

	return 0;
}

int factory_pingpong_flush(unsigned int factory_start, unsigned int factory_part_size) {
	int ret_val=0;

	if(factory_start == 0 || factory_part_size == 0) {
		factory_start = factory_start_addr;
		factory_part_size = factory_zone / 2;
	}

	/* save current content of "/tmp/factory.tar" to storage twice */
	if((ret_val = factory_flush(0, 0)) < 0) {
		install_fail("factory_flush fail\r\n");
		return -1;
	}
	if((ret_val = factory_flush(0, 0)) < 0) {
		install_fail("factory_flush fail\r\n");
		return -1;
	}
	/* if current_pp == 1, saving one more time to let current_pp == 0 */
	/* let the pp of newest factory file is 0 */
	install_debug("factory_current_pp = %d\r\n", factory_current_pp);
	if (factory_current_pp == 1) {
		if((ret_val = factory_flush(0, 0)) < 0) {
			install_fail("factory_flush fail\r\n");
			return -1;
		}
	}
	return 0;
}

int install_factory(void) {
	int ret_val = 0;

	run("rm -rf %s; mkdir -p %s; tar -x -f /tmp/%s -C %s/",
	    FACTORY_INSTALL_TEMP,
	    FACTORY_INSTALL_TEMP,
	    in_factorytar_file,
	    FACTORY_INSTALL_TEMP);

	if ((ret_val=factory_load()) < 0) {
		install_fail("factory load fail\n");
		return -1;
	}

	run("cp -rpf %s/* %s/", FACTORY_INSTALL_TEMP, factory_dir);

	factory_pingpong_flush(0, 0);

	return 0;
}
