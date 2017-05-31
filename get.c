#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>

#define THIS_MODULE_SECTION ".gnu.linkonce.this_module"

#if WIDTH == 32
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Shdr Elf32_Shdr
#define MODULE_NAME_OFFSET 12
#elif WIDTH == 64
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Shdr Elf64_Shdr
#define MODULE_NAME_OFFSET 24
#endif

int get_file_size(int fd) {
    int file_size = 0;

    if (fd < 0) {
        return -1;
    }

    file_size = lseek(fd, 0, SEEK_END);

    return file_size;
}

void *find_section(void *data, Elf_Ehdr *elf, Elf_Shdr *shdr, const char *target_section) {
    char *strtab = NULL;

    if (elf == NULL || shdr == NULL || target_section == NULL) {
        return NULL;
    }

    strtab = (char*) data + shdr[elf->e_shstrndx].sh_offset;

    for (int idx = 0; idx < elf->e_shnum; ++idx) {
        const char *iter_section = strtab + shdr[idx].sh_name;
        if (strcmp(iter_section, target_section) == 0) {
            return data + shdr[idx].sh_offset;
        }
    }

    return NULL;
}

int get_module_name(const char *path, char *module_name, size_t size) {
    int ret = -1;
    int fd = 0;
    int file_size = 0;
    void *data = NULL;
    void *section = NULL;
    Elf_Ehdr *elf = NULL;
    Elf_Shdr *shdr = NULL;

    if (path == NULL || module_name == NULL || size == 0) {
        fprintf(stderr, "bad parameters\n");
        goto END;
    }

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(stderr, "failed to open file: %s (%m)\n", path);
        goto END;
    }

    if ((file_size = get_file_size(fd)) < 0) {
        fprintf(stderr, "failed to get file size (%m)\n");
        goto END;
    }

    if ((data = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        fprintf(stderr, "failed to mmap (%m)\n");
        goto END;
    }

    elf = (Elf_Ehdr*) data;
    shdr = (Elf_Shdr*) (data + elf->e_shoff);

    if ((section = find_section(data, elf, shdr, THIS_MODULE_SECTION)) == NULL) {
        fprintf(stderr, "failed to find section: %s\n", THIS_MODULE_SECTION);
        goto END;
    }

    snprintf(module_name, size, "%s", (char*) section + MODULE_NAME_OFFSET);

    ret = 0;

END:
    munmap(data, file_size);

    if (fd >= 0) {
        close(fd);
    }

    return ret;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "    %s <kernel_module.ko>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    char module_name[BUFSIZ] = {'\0'};

    if (get_module_name(path, module_name, sizeof(module_name)) < 0) {
        fprintf(stderr, "failed to get module name\n");
        return 1;
    }

    printf("%s\n", module_name);

    return 0;
}
