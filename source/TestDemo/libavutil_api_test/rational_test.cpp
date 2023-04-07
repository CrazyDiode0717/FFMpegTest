extern "C"
{
#include "libavutil/rational.h"
#include "libavutil/avutil.h"
}

void TestRational()
{
    int nRet = 0;

    AVRational r1 = av_make_q(1, 25);
    AVRational r2 = av_make_q(1, 50);

    /*
     * 0：a等于b
     * 1：a大于b
     * -1：a小于b
     * INT_MIN：a或b存在不合法的0/0
    */
    nRet = av_cmp_q(r1, r2);

    double d1 = av_q2d(r1);

    AVRational r3{24, 1001};
    AVRational r4{0, 0};

    //一般用于帧率计算。 求num/den的化简，输出结果保存在dst_num/dst_den，max用于限制dst_num和dst_den任何一个都不能大于它，一般用INT_MAX
    nRet = av_reduce(&r4.num, &r4.den, r3.num, r3.den, 25);

    AVRational  r5 = av_mul_q(r1, r2);
    r5 = av_div_q(r1, r2);
    r5= av_add_q(r1, r2);
    r5 = av_sub_q(r1, r2);

    //求有理数的倒数1/q
    r5 = av_inv_q(r1);

    r5 = av_d2q(d1, 50);

    r5 = av_gcd_q(r1, r2, AV_TIME_BASE / 2, AV_TIME_BASE_Q);
}