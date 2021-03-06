/*
 * Copyright 2002 Lite Speed Technologies Inc, All Rights Reserved.
 * LITE SPEED PROPRIETARY/CONFIDENTIAL.
 */


#include <util/rlimits.h>
#include <string.h>
RLimits::RLimits()
{
    reset();
}

RLimits::~RLimits()
{
}

void RLimits::reset()
{
    memset(this, 0, sizeof(RLimits));
}

void RLimits::setDataLimit(rlim_t cur, rlim_t max)
{
#if defined(RLIMIT_AS) || defined(RLIMIT_DATA) || defined(RLIMIT_VMEM)
    if (cur)
        m_data.rlim_cur = cur;
    if (max)
        m_data.rlim_max = max;
#endif


}
void RLimits::setProcLimit(rlim_t cur, rlim_t max)
{
#if defined(RLIMIT_NPROC)
    if (cur)
        m_nproc.rlim_cur = cur;
    if (max)
        m_nproc.rlim_max = max;
#endif
}
void RLimits::setCPULimit(rlim_t cur, rlim_t max)
{
#if defined(RLIMIT_CPU)
    if (cur)
        m_cpu.rlim_cur = cur;
    if (max)
        m_cpu.rlim_max = max;
#endif
}

int RLimits::applyMemoryLimit() const
{
#if defined(RLIMIT_AS) || defined(RLIMIT_DATA) || defined(RLIMIT_VMEM)
    if (m_data.rlim_cur)
    {
        return
#if defined(RLIMIT_AS)
            setrlimit(RLIMIT_AS, &m_data);
#elif defined(RLIMIT_DATA)
            setrlimit(RLIMIT_DATA, &m_data);
#elif defined(RLIMIT_VMEM)
            setrlimit(RLIMIT_VMEM, &m_data);
#endif
    }
#endif
    return 0;
}

int RLimits::applyProcLimit() const
{
#if defined(RLIMIT_NPROC)
    if (m_nproc.rlim_cur)
        setrlimit(RLIMIT_NPROC, &m_nproc);
#endif
    return 0;
}

int RLimits::apply() const
{
    applyMemoryLimit();
    applyProcLimit();
#if defined(RLIMIT_NPROC)
    if (m_nproc.rlim_cur)
        setrlimit(RLIMIT_NPROC, &m_nproc);
#endif

#if defined(RLIMIT_CPU)
    if (m_cpu.rlim_cur)
        setrlimit(RLIMIT_CPU, &m_cpu);
#endif
    return 0;
}

