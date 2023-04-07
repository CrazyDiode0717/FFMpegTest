extern "C"
{
#include "libavutil/cpu.h"
}
#include <cstdio>

static const struct {
    int flag;
    const char *name;
} cpu_flag_tab[] = {
// #if   ARCH_AARCH64
//     { AV_CPU_FLAG_ARMV8,     "armv8"      },
//     { AV_CPU_FLAG_NEON,      "neon"       },
//     { AV_CPU_FLAG_VFP,       "vfp"        },
// #elif ARCH_ARM
//     { AV_CPU_FLAG_ARMV5TE,   "armv5te"    },
//     { AV_CPU_FLAG_ARMV6,     "armv6"      },
//     { AV_CPU_FLAG_ARMV6T2,   "armv6t2"    },
//     { AV_CPU_FLAG_VFP,       "vfp"        },
//     { AV_CPU_FLAG_VFP_VM,    "vfp_vm"     },
//     { AV_CPU_FLAG_VFPV3,     "vfpv3"      },
//     { AV_CPU_FLAG_NEON,      "neon"       },
//     { AV_CPU_FLAG_SETEND,    "setend"     },
// #elif ARCH_PPC
//     { AV_CPU_FLAG_ALTIVEC,   "altivec"    },
// #elif ARCH_MIPS
//     { AV_CPU_FLAG_MMI,       "mmi"        },
//     { AV_CPU_FLAG_MSA,       "msa"        },
// #elif ARCH_X86
    { AV_CPU_FLAG_MMX,       "mmx"        },
    { AV_CPU_FLAG_MMXEXT,    "mmxext"     },
    { AV_CPU_FLAG_SSE,       "sse"        },
    { AV_CPU_FLAG_SSE2,      "sse2"       },
    { AV_CPU_FLAG_SSE2SLOW,  "sse2slow"   },
    { AV_CPU_FLAG_SSE3,      "sse3"       },
    { AV_CPU_FLAG_SSE3SLOW,  "sse3slow"   },
    { AV_CPU_FLAG_SSSE3,     "ssse3"      },
    { AV_CPU_FLAG_ATOM,      "atom"       },
    { AV_CPU_FLAG_SSE4,      "sse4.1"     },
    { AV_CPU_FLAG_SSE42,     "sse4.2"     },
    { AV_CPU_FLAG_AVX,       "avx"        },
    { AV_CPU_FLAG_AVXSLOW,   "avxslow"    },
    { AV_CPU_FLAG_XOP,       "xop"        },
    { AV_CPU_FLAG_FMA3,      "fma3"       },
    { AV_CPU_FLAG_FMA4,      "fma4"       },
    { AV_CPU_FLAG_3DNOW,     "3dnow"      },
    { AV_CPU_FLAG_3DNOWEXT,  "3dnowext"   },
    { AV_CPU_FLAG_CMOV,      "cmov"       },
    { AV_CPU_FLAG_AVX2,      "avx2"       },
    { AV_CPU_FLAG_BMI1,      "bmi1"       },
    { AV_CPU_FLAG_BMI2,      "bmi2"       },
    { AV_CPU_FLAG_AESNI,     "aesni"      },
    { AV_CPU_FLAG_AVX512,    "avx512"     },
    { AV_CPU_FLAG_AVX512ICL, "avx512icl"  },
    { AV_CPU_FLAG_SLOW_GATHER, "slowgather" },
// #elif ARCH_LOONGARCH
//     { AV_CPU_FLAG_LSX,       "lsx"        },
//     { AV_CPU_FLAG_LASX,      "lasx"       },
// #endif
    { 0 }
};

static void print_cpu_flags(int cpu_flags, const char *type)
{
    int i;

    printf("cpu_flags(%s) = 0x%08X\n", type, cpu_flags);
    printf("cpu_flags_str(%s) =", type);
    for (i = 0; cpu_flag_tab[i].flag; i++)
        if (cpu_flags & cpu_flag_tab[i].flag)
            printf(" %s", cpu_flag_tab[i].name);
    printf("\n");
}

void TestCpu()
{
    int cpuCount = av_cpu_count();
    printf("cpu count:%d\n", cpuCount);

    size_t align = av_cpu_max_align();
    printf("max align:%d\n", align);

    int cpuFlags = av_get_cpu_flags();
    print_cpu_flags(cpuFlags, "raw");

    for (int i = 0; cpu_flag_tab[i].flag; i++) 
    {
        unsigned tmp = 0;
        if (av_parse_cpu_caps(&tmp, cpu_flag_tab[i].name) < 0) 
        {
            fprintf(stderr, "Table missing %s\n", cpu_flag_tab[i].name);
            return ;
        }
    }
}