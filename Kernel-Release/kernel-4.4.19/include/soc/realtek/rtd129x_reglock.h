#ifndef _RTD129x_REGLOCK_H_INCLUDED_
#define _RTD129x_REGLOCK_H_INCLUDED_

#include <linux/spinlock_types.h>

#ifdef CONFIG_COMMON_CLK_RTD129X

extern spinlock_t rtk_clk_en_lock;
#define rtk_clk_en_lock_irqsave(flags) \
    spin_lock_irqsave(&rtk_clk_en_lock, flags)
#define rtk_clk_en_unlock_irqrestore(flags) \
    spin_unlock_irqrestore(&rtk_clk_en_lock, flags)

#else

#define rtk_clk_en_lock_irqsave(flags) \
    WARN_ONCE(1, "clk_en_lock::CONFIG_COMMON_CLK_RTD129x not defined")
#define rtk_clk_en_unlock_irqrestore(flags) \
    WARN_ONCE(1, "clk_en_lock::CONFIG_COMMON_CLK_RTD129x not defined")

#endif /* CONFIG_COMMON_CLK_RTD129X */

#ifdef CONFIG_ARCH_RTD129X

extern spinlock_t rtk_rstn_lock;
#define rtk_rstn_lock_irqsave(flags) \
     spin_lock_irqsave(&rtk_rstn_lock, flags)
#define rtk_rstn_unlock_irqrestore(flags) \
    spin_unlock_irqrestore(&rtk_rstn_lock, flags)

#else

#define rtk_rstn_lock_irqsave(flags) \
    WARN_ONCE(1, "rstn_lock::CONFIG_ARCH_RTD129x not defined")
#define rtk_rstn_unlock_irqrestore(flags) \
    WARN_ONCE(1, "rstn_lock::CONFIG_ARCH_RTD129x not defined")

#endif /* CONFIG_ARCH_RTD129X */

#endif /* _RTD129x_REGLOCK_H_INCLUDED_ */

