#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_FWDESC_ENTRY 20

int debug = 0;

struct t_fwdesc_entry {
	char fwpart[16];
	char typecode[128];
	char filename[128];
	uint64_t storage_addr;
	uint32_t actual_size;
	uint32_t zone_size;
};

struct t_fwdesc_entry fwimg[MAX_FWDESC_ENTRY];

uint64_t total_size;
uint32_t align_size;

uint32_t SIZE_ALIGN_BOUNDARY_MORE(uint32_t len, uint32_t size)
{
	return (((len - 1) & ~((size) - 1)) + size);
}

void print_fwimg_sort(unsigned int sort[MAX_FWDESC_ENTRY])
{
	int s, i;

	printf("=> Sorted\n");
	for (s = 0; s < MAX_FWDESC_ENTRY; s++) {
		i = sort[s];
		if (fwimg[i].zone_size == 0)
			continue;
		printf("%2d [%s] [%s] [%s] 0x%lx 0x%x 0x%x\n",
		       i, fwimg[i].fwpart, fwimg[i].typecode,
		       fwimg[i].filename, fwimg[i].storage_addr,
		       fwimg[i].actual_size, fwimg[i].zone_size);
	}
}

void print_fwimg_struct(void)
{
	int i;

	for (i = 0; i < MAX_FWDESC_ENTRY; i++) {
		if (fwimg[i].zone_size == 0)
			continue;
		printf("%2d [%s] [%s] [%s] 0x%lx 0x%x 0x%x\n",
		       i, fwimg[i].fwpart, fwimg[i].typecode,
		       fwimg[i].filename, fwimg[i].storage_addr,
		       fwimg[i].actual_size, fwimg[i].zone_size);
	}
}

int print_fwimg_layout(void)
{
	unsigned int sort[MAX_FWDESC_ENTRY], max, temp;
	int i, j;
	uint64_t flash_top_addr = 0, diff_size;
	int error = 0;
	uint64_t bootlogo_1st_addr = 0, bootlogo_2nd_addr = 0;
	int skip_bootlogo_check = 0;

	/* Set sort[] to the index of fwimg[] */
	for (i = 0; i < MAX_FWDESC_ENTRY; i++)
		sort[i] = i;

	/* Sort sort[] by start addres in decreasing order */
	for (i = 0; i < MAX_FWDESC_ENTRY; i++) {
		max = i;
		for (j = i + 1; j < MAX_FWDESC_ENTRY; j++) {
			if (fwimg[sort[max]].storage_addr <
			    fwimg[sort[j]].storage_addr) {
				max = j;
			}
		}

		temp = sort[i];
		sort[i] = sort[max];
		sort[max] = temp;
	}

	/* Skip BootLogo check if storage addr contained in both fw entries are the same */
	for (i = 0; i < MAX_FWDESC_ENTRY; i++) {
		if (strcmp(fwimg[sort[i]].typecode, "bootLogo") == 0) {
			if (strcmp(fwimg[sort[i]].fwpart, "1stfw") == 0) {
				bootlogo_1st_addr = fwimg[sort[i]].storage_addr;
			}
			else if (strcmp(fwimg[sort[i]].fwpart, "2ndfw") == 0) {
				bootlogo_2nd_addr = fwimg[sort[i]].storage_addr;
			}
		}
	}
	if ((bootlogo_1st_addr != 0) && (bootlogo_2nd_addr != 0)) {
		if ( bootlogo_1st_addr == bootlogo_2nd_addr ) {
			skip_bootlogo_check = 1;
		}
	}

	if (debug)
		print_fwimg_sort(sort);

	printf
	    ("          #fw  fwpart_name : actual_size(dec) [(hex)]      /   zone_size(dec) [(hex)]\n");
	for (i = 0; i < MAX_FWDESC_ENTRY; i++) {
		if (error)
			break;

		if (fwimg[sort[i]].zone_size == 0)
			continue;

		/* Print the top */
		if (flash_top_addr == 0) {
			printf
			    ("   +---------------------------------------------------------------------------------------+ 0x%012lx\n",
			     total_size);
			flash_top_addr = total_size;
		}

		/* Print free space if space exists between previous fwpart's start address and this fwpart's start address + zone */
		if (flash_top_addr >
		    fwimg[sort[i]].storage_addr + fwimg[sort[i]].zone_size) {
			diff_size =
			    flash_top_addr - (fwimg[sort[i]].storage_addr +
					      fwimg[sort[i]].zone_size);
			printf
			    ("   |            FREE SPACE :                                 %10ld bytes [0x%08lx] |\n",
			     diff_size, diff_size);
			printf
			    ("   +---------------------------------------------------------------------------------------+ 0x%012lx\n",
			     fwimg[sort[i]].storage_addr +
			     fwimg[sort[i]].zone_size);
		}

		/* Check overlap error if the above fwpart's start address < this fwpart's zone boundary */
		if (flash_top_addr <
		    fwimg[sort[i]].storage_addr + fwimg[sort[i]].zone_size) {
			if ( skip_bootlogo_check && strcmp(fwimg[sort[i]].typecode, "bootLogo") == 0 ) {
				// skip bootlogo check if storage addr contained in both fw entries are the same
			}
			else
			{
				diff_size =
					(fwimg[sort[i]].storage_addr +
					fwimg[sort[i]].zone_size) - flash_top_addr;
				printf
					("        !!! ERROR !!!        %10ld bytes [0x%08lx] zone overlapped\n",
					diff_size, diff_size);
				printf
					("   +---------------------------------------------------------------------------------------+ 0x%012lx\n",
					fwimg[sort[i]].storage_addr +
					fwimg[sort[i]].zone_size);
				error = 1;	
			}
		}

		/* Print this fwpart's actual size / zone size and filename */
		/* Special case: audioKernel
		 * To avoid verification fail in bootcode,
		 * the length of audioKernel in fwdesc entry is not aligned.
		 * But the audioKernel written into flash is still the aligned one.
		 */
		if (strcmp(fwimg[sort[i]].typecode, "audioKernel") == 0) {
			printf
			    ("   | %8s %12s : %10d bytes [0x%08x] / %10d bytes [0x%08x] |\n",
			     fwimg[sort[i]].fwpart, fwimg[sort[i]].typecode,
			     SIZE_ALIGN_BOUNDARY_MORE(fwimg[sort[i]].actual_size, align_size),
				 SIZE_ALIGN_BOUNDARY_MORE(fwimg[sort[i]].actual_size, align_size),
			     fwimg[sort[i]].zone_size, fwimg[sort[i]].zone_size);
		}
		else {
			printf
			    ("   | %8s %12s : %10d bytes [0x%08x] / %10d bytes [0x%08x] |\n",
			     fwimg[sort[i]].fwpart, fwimg[sort[i]].typecode,
			     fwimg[sort[i]].actual_size, fwimg[sort[i]].actual_size,
			     fwimg[sort[i]].zone_size, fwimg[sort[i]].zone_size);
		}
		printf
		    ("   |              %40s                                 |\n",
		     fwimg[sort[i]].filename);

		/* Check size error if the actual size > zone size */
		if (fwimg[sort[i]].actual_size > fwimg[sort[i]].zone_size) {
			printf
			    ("        !!! ERROR !!! actual_size %d > zone_size %d\n",
			     fwimg[sort[i]].actual_size,
			     fwimg[sort[i]].zone_size);
			error = 1;
		}

		/* Print this fwpart's start address */
		printf
		    ("   +---------------------------------------------------------------------------------------+ 0x%012lx\n",
		     fwimg[sort[i]].storage_addr);

		flash_top_addr = fwimg[sort[i]].storage_addr;
	}

	if (error)
		return -1;

	return 0;
}

int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	char newline[256], key[32], value[32];
	char fwpart[16], typecode[128], filename[128], compress[128];
	uint32_t ram_addr, actual_size, zone_size;
	uint64_t storage_addr;
	int i = 0, ret = 0;

	if (argc < 2) {
		printf("Usage: %s [config.txt]\n", argv[0]);
		return -1;
	}

	fp = fopen(argv[1], "r");
	if (!fp) {
		printf("layout error: no such file %s\n", argv[1]);
		return -1;
	}

	if (argc > 2)
		debug = atoi(argv[2]);

	memset(fwimg, 0, sizeof(fwimg));

	while (fgets(newline, sizeof(newline), fp)) {
		/* Skip useless line such as comments */
		if (strlen(newline) < 3)
			continue;

		/* Get line with "key=value" */
		sscanf(newline, "%[^=]=%s", key, value);
		if (strncmp(key, "total_size", strlen("total_size")) == 0)
			total_size = atol(value);
		if (strncmp(key, "align_size", strlen("align_size")) == 0)
			align_size = atol(value);	
		/* Get line with "xxxfw = ..." or "fw = " and "bootpart = ..." */
		if ((strncmp(newline + strlen("1st"), "fw = ", 5) == 0)
		    || (strncmp(newline, "fw = ", 5) == 0))
			sscanf(newline, "%s = %s %s %s %x %lx %x %x", fwpart,
			       typecode, filename, compress, &ram_addr,
			       &storage_addr, &actual_size, &zone_size);
		else if (strncmp(newline, "bootpart", strlen("bootpart")) == 0)
			sscanf(newline, "%s = %s %s %lx %x %x",
			       fwpart, typecode, filename, &storage_addr,
			       &actual_size, &zone_size);
		else if (strncmp(newline, "normalpart", strlen("normalpart")) == 0)
			sscanf(newline, "%s = %s %s %lx %x %x",
			       fwpart, typecode, filename, &storage_addr,
			       &actual_size, &zone_size);
		else
			continue;

		strcpy(fwimg[i].fwpart, fwpart);
		strcpy(fwimg[i].typecode, typecode);
		strcpy(fwimg[i].filename, filename + 1);	// FIXME: filename has prefix '_'
		fwimg[i].storage_addr = storage_addr;
		fwimg[i].actual_size = actual_size;
		fwimg[i].zone_size = zone_size;
		i++;
	}

	fclose(fp);

	if (debug)
		print_fwimg_struct();

	ret = print_fwimg_layout();

	return ret;
}
