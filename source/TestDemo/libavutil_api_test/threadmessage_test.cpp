extern "C"
{
#include "libavutil/threadmessage.h"
}
#include <cstdio>
#include <thread>

struct stMsg
{
    int nSeqNum;
};

bool g_bRunning = false;
AVThreadMessageQueue* g_pMq = nullptr;

void ReadFunc()
{
    int nRet = 0;
    do
    {
        stMsg msg{0};
        nRet = av_thread_message_queue_recv(g_pMq, &msg, 0);
        if(nRet < 0)
        {
            printf("receive message failed\n");
        }

        printf("get mesage %d\n", msg.nSeqNum);

        nRet = av_thread_message_queue_nb_elems(g_pMq);

    }while(g_bRunning || (nRet > 0));
}

void TestThreadmessage()
{
    int nRet = 0;
    nRet = av_thread_message_queue_alloc(&g_pMq, 5,  sizeof(stMsg));
    if(nRet < 0)
    {
        printf("alloc message queue failed\n");
        return;
    }

    g_bRunning = true;
    std::thread hThread(ReadFunc);
    for(int i = 0; i < 10; i++)
    {
        stMsg msg;
        msg.nSeqNum = i;
        nRet = av_thread_message_queue_send(g_pMq, &msg, 0);
        if(nRet < 0)
        {
            printf("send mesage %d failed\n", i);
        }
    }

    g_bRunning = false;
    if(hThread.joinable())
        hThread.join();

    av_thread_message_flush(g_pMq);
    av_thread_message_queue_free(&g_pMq);
}