#ifndef __KAL_INTR_H__
#define __KAL_INTR_H__

/**
 * @file kal_intr.h
 */

/**
 * @defgroup intr Interrupt Locking
 * @{
 */

/**
 * Interrupt flags
 */
typedef unsigned long kal_iflags_t;

#if defined(_KAL_LINUX_KM)
static INLINE kal_iflags_t kal_hwi_lock()
{
    unsigned long iflags;

    local_irq_save(iflags);

    return iflags;
}

static INLINE void kal_hwi_unlock(kal_iflags_t iflags)
{
    local_irq_restore(iflags);
}

static INLINE void kal_swi_lock(void)
{
    local_bh_disable();
}

static INLINE void kal_swi_unlock(void)
{
    local_bh_enable();
}
#elif defined(_KAL_LINUX_UM) /* _KAL_LINUX_UM */
static INLINE kal_iflags_t kal_hwi_lock(void)
{
    return 0;
}

static INLINE void kal_hwi_unlock(kal_iflags_t iflags)
{
    iflags = iflags; /* TODO not used parameter */
}

static INLINE void kal_swi_lock(void)
{
}

static INLINE void kal_swi_unlock(void)
{
}
#endif

/**@}*/ /* End of @defgroup intr */

#endif /* !__KAL_INTR_H__ */

