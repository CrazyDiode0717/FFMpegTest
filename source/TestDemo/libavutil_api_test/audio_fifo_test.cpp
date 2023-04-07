
extern "C"
{
#include "libavutil/audio_fifo.h"
}

#include <cstdint>
#include <memory>
#include <mutex>

class AudioFifo
{
public:
	AudioFifo(AVSampleFormat sample_fmt, int channels, int max_samples);
	virtual ~AudioFifo();

	int GetSamples();
	int Write(void* data, int samples);
	int Read(void* data, int samples);

private:
	std::mutex m_mutex;
	std::shared_ptr<AVAudioFifo> m_audioFifo;
	AVSampleFormat m_sampleFmt;
	int m_channels;
	int m_maxSamples;
};


AudioFifo::AudioFifo(AVSampleFormat sampleFmt, int channels, int maxSamples)
	: m_sampleFmt(sampleFmt)
	, m_channels(channels)
	, m_maxSamples(maxSamples)
{
	m_audioFifo.reset(av_audio_fifo_alloc(m_sampleFmt, m_channels, m_maxSamples),
					  [](AVAudioFifo* af){ av_audio_fifo_free(af); });
}

AudioFifo::~AudioFifo()
{

}

int AudioFifo::GetSamples()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return av_audio_fifo_size(m_audioFifo.get());
}

int AudioFifo::Write(void* data, int samples)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return av_audio_fifo_write(m_audioFifo.get(), &data, samples );
}

int AudioFifo::Read(void* data, int samples)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return av_audio_fifo_read(m_audioFifo.get(), &data, samples );
}


void TestAudioFifo()
{

}