#include "g5g.h"

uint32_t GetSampleRate(const char* file)
{
	ifstream ifs(file, ios::in|ios::binary);
	if(!ifs.is_open())
	{
		std::cerr << "Can't open file:" << file << endl;
		return 0;
	}
	WavHeader header;
    ifs.read(reinterpret_cast<char *>(&header), sizeof(WavHeader));
	ifs.close();
    if (std::string(header.chunkID, 4) != "RIFF" ||
        std::string(header.format, 4) != "WAVE" ||
        std::string(header.subchunk1ID, 4) != "fmt " ||
        (std::string(header.subchunk2ID, 4) != "data" &&
		std::string(header.subchunk2ID, 4) != "LIST")
		) {
        std::cerr << "Invalid WAV file" << std::endl;
        return 0;
    }
	return header.sampleRate;
	
}

bool yuyin()
{
    const char* file = "ans.wav";
	// 初始化OpenAL
	ALCdevice* device = alcOpenDevice(nullptr);
    ALCcontext* context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);
	
	const unsigned int False = 0;
	unsigned int nSampleRate = GetSampleRate(file);
	if(False == nSampleRate)
		return false;
	
	std::ifstream ifs(file, ios::binary|ios::in);
	std::ostringstream tmp;
	tmp << ifs.rdbuf();
	std::string str = tmp.str();
	ifs.close();
	
    ALuint source;
    alGenSources(1, &source);
    ALuint bufferId;
    alGenBuffers(1, &bufferId);
    alBufferData(bufferId, AL_FORMAT_MONO16, str.c_str(), str.size(), nSampleRate);
    alSourcei(source, AL_BUFFER, bufferId);
    alSourcePlay(source);

    // 等待音频播放完毕
    ALint state;
    do
    {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    // 清理
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &bufferId);
    alcDestroyContext(context);
    alcCloseDevice(device);
	return true;
}