
#include "log.h"
#include "hook_elf.h"
#include "hook_util.h"
#include <elf.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifndef R_ARM_ABS32
#define R_ARM_ABS32 0x02
#endif
#ifndef R_ARM_GLOB_DAT
#define R_ARM_GLOB_DAT 0x15
#endif
#ifndef R_ARM_JUMP_SLOT
#define R_ARM_JUMP_SLOT 0x16
#endif

static int hook_at(int mem, int val) {
    int rc;
    int *dst, old;
    int ps, aligned;

    dst = (int *) mem;
    old = *dst;
    if (old == val) {
        LOGW("%s: already hooked\n", __func__);
        return val;
    }
    ps = getpagesize();
    aligned = (~(ps - 1)) & mem;
    // icache flush needs PROT_EXEC
    rc = mprotect((void *) aligned, ps, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (rc < 0) {
        LOGE("%s: mprotect failed\n", __func__);
        return 0;
    }
    // alter
    *dst = val;
    // icache flush
    rc = syscall(0xf0002, mem, mem + sizeof(int));

    return old;
}

int hook_elf(const char *soname, const char *symbol, int addr) {
    int rc = -1, i;
    uint32_t sobase;
    Elf32_Ehdr *ehdr = NULL;
    Elf32_Phdr *phdr = NULL;
    Elf32_Dyn *dyn = NULL;
    Elf32_Word dynsz = (Elf32_Word) -1;
    Elf32_Sym *sym = NULL;
    Elf32_Word symsz = (Elf32_Word) -1;
    Elf32_Rel *relplt = NULL;
    Elf32_Word relpltsz = (Elf32_Word) -1;
    Elf32_Rel *reldyn = NULL;
    Elf32_Word reldynsz = (Elf32_Word) - 1;
    const char *strtab = NULL;
    uint32_t nbucket = (uint32_t) -1;
    uint32_t nchain = (uint32_t) -1;
    uint32_t *bucket = NULL;
    uint32_t *chain = NULL;
    Elf32_Phdr *pt = NULL;
    Elf32_Dyn *d;
    Elf32_Sym *target = NULL;
    unsigned int hash;
    uint32_t index;
    Elf32_Rel *r;

    sobase = (uint32_t) get_module_base(-1, soname);
    LOGD("%s: @ = 0x%x\n", __func__, sobase);
    if (sobase == 0)
        return -1;
    // locate PT_DYNAMIC
    ehdr = (Elf32_Ehdr *) sobase;
    phdr = (Elf32_Phdr *)(sobase + ehdr->e_phoff);
    for (i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_DYNAMIC) {
            pt = &phdr[i];
            break;
        }
    }
    LOGD("%s: PT_DYNAMIC = %p\n", __func__, pt);
    if (pt == NULL)
        return -1;
    dyn = (Elf32_Dyn *)(sobase + (uint32_t) pt->p_vaddr);
    dynsz = pt->p_memsz / sizeof(Elf32_Dyn);
    // analyze PT_DYNAMIC
    for (i = 0; i < dynsz; i++) {
        d = dyn + i;
        switch (d->d_tag) {
        case DT_SYMTAB: {
            sym = (Elf32_Sym *)(sobase + d->d_un.d_val);
            break; 
        }
        case DT_STRTAB: {
            strtab = (const char *)(sobase + d->d_un.d_val);
            break;
        }
        case DT_HASH: {
            uint32_t *rawdata;

            rawdata = (uint32_t *)(sobase + d->d_un.d_val);
            nbucket = rawdata[0];
            nchain = rawdata[1];
            bucket = rawdata + 2;
            chain = bucket + nbucket;
            symsz = nchain;
            break;
        }
        case DT_REL: {
            reldyn = (Elf32_Rel *)(sobase + d->d_un.d_val);
            break;
        }
        case DT_RELSZ: {
            reldynsz = d->d_un.d_val / sizeof(Elf32_Rel);
            break;
        }
        case DT_JMPREL: {
            relplt = (Elf32_Rel *)(sobase + d->d_un.d_val);
            break;
        }
        case DT_PLTRELSZ: {
            relpltsz = d->d_un.d_val / sizeof(Elf32_Rel);
            break;
        }
        default:
            break;
        }
    }
    if (1) {
        LOGD("%s:\n"
            "\tDT_SYMTAB = %p size = %d\n"
            "\tDT_STRTAB = %p\n"
            "\tbucket = %p size = %d\n"
            "\tchain = %p size = %d\n"
            "\tDT_REL = %p size = %d\n"
            "\tDT_JMPREL = %p size = %d\n",
            __func__, sym, symsz, strtab, bucket, nbucket,
            chain, nchain, reldyn, reldynsz, relplt, relpltsz);
    }
    // locate the symbol
    hash = elf_hash((const unsigned char *) symbol);
    index = bucket[hash % nbucket];
    if (strcmp(strtab + sym[index].st_name, symbol)) {
        do {
            index = chain[index];
            if (!strcmp(strtab + sym[index].st_name, symbol)) {
                target = sym + index;
                break;
            }
        } while (index != 0);
    } else {
        target = sym + index;
    }
    if (target == NULL) {
        LOGE("%s: resolve `%s\' failed\n", __func__, symbol);
        return -1;
    }
    LOGD("%s: resolved `%s\' index = %d\n", __func__, symbol, index);
    // replace the symbol
    for (i = 0; i < reldynsz; i++) {
        r = &reldyn[i];
        if ((ELF_R_SYM(r->r_info) == index) &&
            ((ELF_R_TYPE(r->r_info) == R_ARM_ABS32) ||
            (ELF_R_TYPE(r->r_info) == R_ARM_GLOB_DAT))) {
            LOGD("%s: hook #1 hit: 0x%x\n", __func__, sobase + r->r_offset);
            rc = hook_at(sobase + r->r_offset, addr);
            if (rc == 0) {
                LOGE("%s: hook #1 failed\n", __func__);
                return -1;
            }
        }
    }
    for (i = 0; i < relpltsz ; i++) {
        r = &relplt[i];
        if ((ELF_R_SYM(r->r_info) == index) &&
            (ELF_R_TYPE(r->r_info) == R_ARM_JUMP_SLOT)) {
            LOGD("%s: hook #2 hit: 0x%x\n", __func__, sobase + r->r_offset);
            rc = hook_at(sobase + r->r_offset, addr);
            if (rc == 0) {
                LOGE("%s: hook #2 failed\n", __func__);
                return -1;
            }
            break;
        }
    }

    LOGD("%s: hook `%s\' done, old ptr = %p\n", __func__, symbol, (void *) rc);

    return rc;
}

