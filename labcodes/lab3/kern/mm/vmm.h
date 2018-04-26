#ifndef __KERN_MM_VMM_H__
#define __KERN_MM_VMM_H__

#include <defs.h>
#include <list.h>
#include <memlayout.h>
#include <sync.h>

//pre define
struct mm_struct;

// the virtual continuous memory area(vma), [vm_start, vm_end), 
// addr belong to a vma means  vma.vm_start<= addr <vma.vm_end 
struct vma_struct {
    struct mm_struct *vm_mm; // vma管理器指针 the set of vma using the same PDT
    uintptr_t vm_start;      // vma的起始地址 start addr of vma
    uintptr_t vm_end;        // vma的结束地址 end addr of vma, not include the vm_end itself
    uint32_t vm_flags;       // vma的状态 flags of vma
    list_entry_t list_link;  // 双向链表 linear list link which sorted by start addr of vma
};

#define le2vma(le, member)                  \
    to_struct((le), struct vma_struct, member)

#define VM_READ                 0x00000001
#define VM_WRITE                0x00000002
#define VM_EXEC                 0x00000004

// the control struct for a set of vma using the same PDT
struct mm_struct {
    list_entry_t mmap_list;        // 线性链表的头，指向按照地址排序的起始vma
    //linear list link which sorted by start addr of vma
    struct vma_struct *mmap_cache; // 当前vma 用于加速（缓存一致性）
    pde_t *pgdir;                  // vma集合的页表头
    int map_count;                 // vma集合的个数
    void *sm_priv;                   // the private data for swap manager
};

struct vma_struct *find_vma(struct mm_struct *mm, uintptr_t addr);
struct vma_struct *vma_create(uintptr_t vm_start, uintptr_t vm_end, uint32_t vm_flags);
void insert_vma_struct(struct mm_struct *mm, struct vma_struct *vma);

struct mm_struct *mm_create(void);
void mm_destroy(struct mm_struct *mm);

void vmm_init(void);

int do_pgfault(struct mm_struct *mm, uint32_t error_code, uintptr_t addr);

extern volatile unsigned int pgfault_num;
extern struct mm_struct *check_mm_struct;
#endif /* !__KERN_MM_VMM_H__ */

