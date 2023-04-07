extern "C" {
#include "libavutil/uuid.h"
}

void TestUuid()
{
    int nRet = 0;
    const char* UUID_1 = "6021b21e-894e-43ff-8317-1ca891c1c49b";
    const char* UUID_1_UC = "6021B21E-894E-43FF-8317-1CA891C1C49B";
    const char* UUID_1_MIXED = "6021b21e-894E-43fF-8317-1CA891C1c49b";
    const char* UUID_1_URN = "urn:uuid:6021b21e-894e-43ff-8317-1ca891c1c49b";
    
    static const AVUUID UUID_1_BYTES = { 0x60, 0x21, 0xb2, 0x1e, 0x89, 0x4e,
                                        0x43, 0xff, 0x83, 0x17, 0x1c, 0xa8,
                                        0x91, 0xc1, 0xc4, 0x9b };

    static const AVUUID UUID_NIL = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00 };

                                    AVUUID uuid;
    AVUUID uuid2 = {0x32, 0xc7, 0x00, 0xc4, 0xd5, 0xd7, 0x42, 0x0,
                    0x93, 0xc0, 0x3b, 0x6d, 0xea, 0x1b, 0x20, 0x5b};

    /* test parsing */

    if (av_uuid_parse(UUID_1, uuid))
        return;

    if (!av_uuid_equal(uuid, UUID_1_BYTES))
        return;

    /* test nil */

    av_uuid_nil(uuid);

    if (!av_uuid_equal(uuid, UUID_NIL))
        return;

    /* test equality */

    if (av_uuid_equal(UUID_1_BYTES, uuid2))
        return;

    /* test copy */

    av_uuid_copy(uuid2, UUID_1_BYTES);

    if (!av_uuid_equal(uuid2, UUID_1_BYTES))
        return;

    /* test uppercase parsing */

    if (av_uuid_parse(UUID_1_UC, uuid))
        return;

    if (!av_uuid_equal(uuid, UUID_1_BYTES))
        return;

    /* test mixed-case parsing */

    if (av_uuid_parse(UUID_1_MIXED, uuid))
        return;

    if (!av_uuid_equal(uuid, UUID_1_BYTES))
        return;

    /* test URN uuid parse */

    if (av_uuid_urn_parse(UUID_1_URN, uuid))
        return;

    if (!av_uuid_equal(uuid, UUID_1_BYTES))
        return;

    /* test parse range */

    if (av_uuid_parse_range(UUID_1_URN + 9, UUID_1_URN + 45, uuid))
        return;

    if (!av_uuid_equal(uuid, UUID_1_BYTES))
        return;
}