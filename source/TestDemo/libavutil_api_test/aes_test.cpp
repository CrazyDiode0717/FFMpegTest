#include <string.h>

extern "C" {
#include "libavutil/aes.h"
#include "libavutil/lfg.h"
#include "libavutil/mem.h"
}

int TestAes() 
{
    const uint8_t key[16] = {'a', 'b', 'c', 'd', 'e', 'f', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    const uint8_t encData[16] = {'a', 'a', 'a', '1', '2', '3', 'b', 'b', 'b', 'c', 'd', 'd', 'h', 'e', 'l', 'l'};
    
    uint8_t encBuf[32]{0};
    uint8_t decBuf[32]{0};


    AVAES *hEnc = av_aes_alloc();
    AVAES *hDec = av_aes_alloc();

    av_aes_init(hEnc, key, 128, 0);
    av_aes_crypt(hEnc, encBuf, encData, 1, NULL, 0);
    
    av_aes_init(hDec, key, 128, 1);
    av_aes_crypt(hDec, decBuf, encBuf, 1, NULL, 1);

    printf("%s\n", decBuf);

    av_free(hEnc);
    av_free(hDec);

    return 0;
}
