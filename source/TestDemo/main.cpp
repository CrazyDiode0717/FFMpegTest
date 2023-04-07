#include <cstdio>

extern int TestDecoder();
extern int TestEncode264();
extern int TestFilter();
extern int test_mov_writer();
extern void TestImgutils();
extern int TestAes();
extern void TestAudioFifo();
extern int TestBase64();
extern void TestAvstring();
extern void TestBprint();
extern void TestBuffer();
extern void TestDict();
extern void TestCpu();
extern void TestError();
extern int TestFifo();
extern void TestFrame();
extern void TestMacros();
extern void TestPixdesc();
extern void TestRational();
extern void TestParseutils();
extern void TestThreadmessage();
extern void TestUuid();
extern void TestAvio();
extern void TestAvformat();
extern void TestBsf();
extern void TestAvcodec();

extern "C"
{
extern int TestFilter1();
extern int TestFilter2();
extern int TestFilter3();
}

int main(int argc, char* argv[])
{
	TestDecoder();
	//TestEncode264();
	//TestFilter();
	//TestFilter2();
	//test_mov_writer();
	//TestImgutils();
	//TestAes();
	//TestBase64();

	//TestAvstring();
	//TestBprint();
	//TestBuffer();
	//TestDict();
	//TestCpu();
	//TestError();
	//TestFifo();
	//TestFrame();
	//TestMacros();
	//TestPixdesc();
	//TestRational();
	//TestParseutils();
	//TestThreadmessage();
	//TestUuid();
    //TestAvio();
    //TestAvformat();
    //TestBsf();
    //TestAvcodec();

    printf("end main\n");
	return 0;
} 