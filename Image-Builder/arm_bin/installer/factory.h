#ifndef FACTORY_H
#define FACTORY_H

#define DEFAULT_FACTORY_DIR "/tmp/factory"
#define DEFAULT_FACTORY_DIR1 "/tmp/factory1"
#define DEFAULT_FACTORY_DIR2 "/tmp/factory2"
#define FACTORY_FILE_PATH "/tmp/factory.tar"
#define FACTORY_INSTALL_TEMP  "/tmp/install_factory"
#define IN_FACTORYTAR_NAMESIZE 100

int factory_find_latest_update(void);
unsigned int factory_tar_checksum(char *header);
void factory_tar_fill_checksum(char *in_header);
void factory_build_header(const char *path);
int factory_flush(unsigned int factory_start, unsigned int factory_part_size);
int factory_init(const char* dir);
int factory_load(void);
int factory_pingpong_flush(unsigned int factory_start, unsigned int factory_part_size);
int install_factory(void) ;

#endif /* RTK_FACTORYTAR_H */
