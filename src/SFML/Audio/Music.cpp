////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2014 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/ALCheck.hpp>
#include <SFML/Audio/SoundFile.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <fstream>


namespace sf
{
////////////////////////////////////////////////////////////
Music::Music() :
m_file    (new priv::SoundFile),
m_sampleCount(0),
m_seekPos(0),
m_loopStart(0),
m_loopEnd(0)
{

}


////////////////////////////////////////////////////////////
Music::~Music()
{
    // We must stop before destroying the file
    stop();

    delete m_file;
}


////////////////////////////////////////////////////////////
bool Music::openFromFile(const std::string& filename)
{
    // First stop the music if it was already running
    stop();

    // Open the underlying sound file
    if (!m_file->openRead(filename))
        return false;

    // Perform common initializations
    initialize();

    return true;
}


////////////////////////////////////////////////////////////
bool Music::openFromMemory(const void* data, std::size_t sizeInBytes)
{
    // First stop the music if it was already running
    stop();

    // Open the underlying sound file
    if (!m_file->openRead(data, sizeInBytes))
        return false;

    // Perform common initializations
    initialize();

    return true;
}


////////////////////////////////////////////////////////////
bool Music::openFromStream(InputStream& stream)
{
    // First stop the music if it was already running
    stop();

    // Open the underlying sound file
    if (!m_file->openRead(stream))
        return false;

    // Perform common initializations
    initialize();

    return true;
}


////////////////////////////////////////////////////////////
std::size_t Music::getSampleCount() const
{
    return m_sampleCount;
}


////////////////////////////////////////////////////////////
Time Music::getDuration() const
{
    return seconds(static_cast<float>(m_sampleCount) / getSampleRate() / getChannelCount());
}


////////////////////////////////////////////////////////////
Time Music::getLoopStart() const
{
    return seconds(static_cast<float>(m_loopStart) / getSampleRate());
}


////////////////////////////////////////////////////////////
Time Music::getLoopEnd() const
{
    return seconds(static_cast<float>(m_loopEnd) / getSampleRate());
}

////////////////////////////////////////////////////////////
bool Music::setLoopPointsFromTime(Time start, Time end)
{
    return setLoopPointsFromSamples(static_cast<Uint64>(start.asSeconds() * getSampleRate()),
                                    static_cast<Uint64>(end.asSeconds() * getSampleRate()));
}


////////////////////////////////////////////////////////////
bool Music::setLoopPointsFromSamples(Uint64 start, Uint64 end)
{
    // Check our state. This averts a divide-by-zero and other potential problems
    if (!getChannelCount() || !m_sampleCount)
        return false;

    Uint64 realSamples = m_sampleCount/getChannelCount();
    if(start > realSamples)         // Clamp start to sample count
        start = realSamples;
    if(end > realSamples || !end)   // Clamp end to sample count, and 0 also means "reset"
        end = realSamples;

    // Any time start and end are the same, you can't form a closed loop
    // So we take that to mean "reset the loop to the whole audio"
    if (start == end)
    {
        start = 0;
        end = realSamples;
    }

    // When we apply this change, it's best to lock the mutex
    Lock lock(m_mutex);
    m_loopStart = start;
    m_loopEnd = end;

    return true;
}


////////////////////////////////////////////////////////////
bool Music::onGetData(SoundStream::Chunk& data)
{
    Lock lock(m_mutex);

    std::size_t toFill = m_samples.size();
    // If the loop end is enabled and imminent, request less data.
    // This will trip an "onLoop()" call from the underlying SoundStream,
    // and we can then take action.
    if (getLoop() && m_loopEnd != 0 && m_seekPos <= m_loopEnd && m_seekPos + (toFill / getChannelCount()) > m_loopEnd)
        toFill = (m_loopEnd - m_seekPos) * getChannelCount();

    // Fill the chunk parameters
    data.samples     = &m_samples[0];
    data.sampleCount = m_file->read(&m_samples[0], toFill);
    m_seekPos += (data.sampleCount / getChannelCount());

    // Check if we have reached the end of the audio file
    return data.sampleCount == m_samples.size();
}


////////////////////////////////////////////////////////////
void Music::onSeek(Time timeOffset)
{
    Lock lock(m_mutex);

    m_file->seek(timeOffset);
    m_seekPos = static_cast<Uint64>(timeOffset.asSeconds() * getSampleRate());
}


////////////////////////////////////////////////////////////
SoundStream::BufferEnd Music::onLoop()
{
    // Called by underlying SoundStream so we can determine where to loop.
    // Are we looping and at the right position?
    if (getLoop() && m_seekPos == m_loopEnd)
    {
        onSeek(getLoopStart());
        return SoundStream::LoopEnd;
    }
    else    // If not, reset to 0
    {
        onSeek(Time::Zero);
        return SoundStream::FileEnd;
    }
}


////////////////////////////////////////////////////////////
Uint64 Music::getLoopSampleOffset()
{
    // Return the channel-multiplied sample position
    // So SoundStream can track the play-position change upon loop.
    return m_loopStart * getChannelCount();
}


////////////////////////////////////////////////////////////
void Music::initialize()
{
    // Compute the music duration
    m_sampleCount = m_file->getSampleCount();
    m_seekPos = 0;
    m_loopStart = 0;
    m_loopEnd = m_sampleCount / m_file->getChannelCount();

    // Resize the internal buffer so that it can contain 1 second of audio samples
    m_samples.resize(m_file->getSampleRate() * m_file->getChannelCount());

    // Initialize the stream
    SoundStream::initialize(m_file->getChannelCount(), m_file->getSampleRate());
}

} // namespace sf
