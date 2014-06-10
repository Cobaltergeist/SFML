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

#ifndef SFML_MUSIC_HPP
#define SFML_MUSIC_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Audio/Export.hpp>
#include <SFML/Audio/SoundStream.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <vector>


namespace sf
{
namespace priv
{
    class SoundFile;
}

class InputStream;

////////////////////////////////////////////////////////////
/// \brief Streamed music played from an audio file
///
////////////////////////////////////////////////////////////
class SFML_AUDIO_API Music : public SoundStream
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ////////////////////////////////////////////////////////////
    Music();

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~Music();

    ////////////////////////////////////////////////////////////
    /// \brief Open a music from an audio file
    ///
    /// This function doesn't start playing the music (call play()
    /// to do so).
    /// Here is a complete list of all the supported audio formats:
    /// ogg, wav, flac, aiff, au, raw, paf, svx, nist, voc, ircam,
    /// w64, mat4, mat5 pvf, htk, sds, avr, sd2, caf, wve, mpc2k, rf64.
    ///
    /// \param filename Path of the music file to open
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see openFromMemory, openFromStream
    ///
    ////////////////////////////////////////////////////////////
    bool openFromFile(const std::string& filename);

    ////////////////////////////////////////////////////////////
    /// \brief Open a music from an audio file in memory
    ///
    /// This function doesn't start playing the music (call play()
    /// to do so).
    /// Here is a complete list of all the supported audio formats:
    /// ogg, wav, flac, aiff, au, raw, paf, svx, nist, voc, ircam,
    /// w64, mat4, mat5 pvf, htk, sds, avr, sd2, caf, wve, mpc2k, rf64.
    /// Since the music is not loaded completely but rather streamed
    /// continuously, the \a data must remain available as long as the
    /// music is playing (ie. you can't deallocate it right after calling
    /// this function).
    ///
    /// \param data        Pointer to the file data in memory
    /// \param sizeInBytes Size of the data to load, in bytes
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see openFromFile, openFromStream
    ///
    ////////////////////////////////////////////////////////////
    bool openFromMemory(const void* data, std::size_t sizeInBytes);

    ////////////////////////////////////////////////////////////
    /// \brief Open a music from an audio file in a custom stream
    ///
    /// This function doesn't start playing the music (call play()
    /// to do so).
    /// Here is a complete list of all the supported audio formats:
    /// ogg, wav, flac, aiff, au, raw, paf, svx, nist, voc, ircam,
    /// w64, mat4, mat5 pvf, htk, sds, avr, sd2, caf, wve, mpc2k, rf64.
    /// Since the music is not loaded completely but rather streamed
    /// continuously, the \a stream must remain alive as long as the
    /// music is playing (ie. you can't destroy it right after calling
    /// this function).
    ///
    /// \param stream Source stream to read from
    ///
    /// \return True if loading succeeded, false if it failed
    ///
    /// \see openFromFile, openFromMemory
    ///
    ////////////////////////////////////////////////////////////
    bool openFromStream(InputStream& stream);

    ////////////////////////////////////////////////////////////
    /// \brief Get the number of samples in the file
    ///
    /// This is the total number of samples in the input that was
    /// opened by this music object
    ///
    /// \return Number of samples
    ///
    ////////////////////////////////////////////////////////////
    std::size_t getSampleCount() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the total duration of the music
    ///
    /// \return Music duration
    ///
    ////////////////////////////////////////////////////////////
    Time getDuration() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the position of the beginning of the sound's looping sequence
    ///
    /// \return Loop start position
    ///
    /// \see getLoopEnd, setLoopPointsFromTime, setLoopPointsFromSamples
    ///
    ////////////////////////////////////////////////////////////
    Time getLoopStart() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the position of the end of the sound's looping sequence
    ///
    /// \return Loop end position
    ///
    /// \see getLoopStart, setLoopPointsFromTime, setLoopPointsFromSamples
    ///
    ////////////////////////////////////////////////////////////
    Time getLoopEnd() const;

    ////////////////////////////////////////////////////////////
    /// \brief Sets the beginning and end of the sound's looping sequence using sf::Time
    ///
    /// Loop points allow one to specify a pair of positions such that, when the music
    /// is enabled for looping, it will seamlessly seek to start whenever it encounters end.
    /// The input values are clamped to the duration of the sound. If they are the same,
    /// then a closed loop cannot be formed, and this function will "reset" the loop to the
    /// full length of the sound. Note that the implicit "loop points" from the end to the
    /// beginning of the stream are still honored. Because of this, "reverse" loop ranges,
    /// where end comes before start, are allowed, and will cause the sound to loop everything
    /// "outside" of the specified range. This function can be safely called at any point
    /// after a stream is opened, and will be applied to a playing sound without affecting
    /// the current playing offset.
    ///
    /// \param start    The offset of the beginning of the loop. Can be any time point within the sound's length
    /// \param end      The offset of the end of the loop. If Time::Zero is passed, it defaults to the end of the sound
    ///
    /// \return True if the times were valid and the operation succeeded, false otherwise
    ///
    /// \see getLoopStart, getLoopEnd, setLoopPointsFromSamples
    ///
    ////////////////////////////////////////////////////////////
    bool setLoopPointsFromTime(Time start = Time::Zero, Time end = Time::Zero);

    ////////////////////////////////////////////////////////////
    /// \brief Sets the beginning and end of the sound's looping sequence using samples
    ///
    /// Used internally by setLoopPointsFromTime, this function can be used if one wants
    /// to set the loop points of the sound to a pair of exact sample positions.
    ///
    /// \param start    The offset of the beginning of the loop. Can be any sample point within the sound's length
    /// \param end      The offset of the end of the loop. If 0 is passed, it defaults to the end of the sound
    ///
    /// \return True if the times were valid and the operation succeeded, false otherwise
    ///
    /// \see getLoopStart, getLoopEnd, setLoopPointsFromTime
    ///
    ////////////////////////////////////////////////////////////
    bool setLoopPointsFromSamples(Uint64 start = 0, Uint64 end = 0);

protected :

    ////////////////////////////////////////////////////////////
    /// \brief Request a new chunk of audio samples from the stream source
    ///
    /// This function fills the chunk from the next samples
    /// to read from the audio file.
    ///
    /// \param data Chunk of data to fill
    ///
    /// \return True to continue playback, false to stop
    ///
    ////////////////////////////////////////////////////////////
    virtual bool onGetData(Chunk& data);

    ////////////////////////////////////////////////////////////
    /// \brief Change the current playing position in the stream source
    ///
    /// \param timeOffset New playing position, from the beginning of the music
    ///
    ////////////////////////////////////////////////////////////
    virtual void onSeek(Time timeOffset);

    ////////////////////////////////////////////////////////////
    /// \brief Change the current playing position in the stream source to the loop-start
    ///
    /// This is called by the underlying SoundStream whenever it needs us to
    /// reset the seek position for a loop. We then determine whether we are
    /// looping on a loop point or the end-of-file, perform the seek, and return the status.
    ///
    /// \return The end condition of this loop operation (file or loop)
    ///
    ////////////////////////////////////////////////////////////
    virtual SoundStream::BufferEnd onLoop();

    ////////////////////////////////////////////////////////////
    /// \brief Called to determine the "samples processed" count of the beginning of the loop
    ///
    /// This function is overriden to provide reliable implementation
    /// of custom loop points.  We pass the sample position of the
    /// loop start in order for getPlayingOffset() to track the
    /// correct in-file position between loop iterations.
    ///
    /// \return The sample position of the loop start
    ///
    ////////////////////////////////////////////////////////////
    virtual Uint64 getLoopSampleOffset();

private :

    ////////////////////////////////////////////////////////////
    /// \brief Initialize the internal state after loading a new music
    ///
    ////////////////////////////////////////////////////////////
    void initialize();

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    priv::SoundFile*   m_file;          ///< Sound file
    Uint64             m_sampleCount;   ///< Music duration
    Uint64             m_seekPos;       ///< Sample Seek Position
    Uint64             m_loopStart;     ///< Loop Start Point
    Uint64             m_loopEnd;       ///< Loop End Point
    std::vector<Int16> m_samples;       ///< Temporary buffer of samples
    Mutex              m_mutex;         ///< Mutex protecting the data
};

} // namespace sf


#endif // SFML_MUSIC_HPP


////////////////////////////////////////////////////////////
/// \class sf::Music
/// \ingroup audio
///
/// Musics are sounds that are streamed rather than completely
/// loaded in memory. This is especially useful for compressed
/// musics that usually take hundreds of MB when they are
/// uncompressed: by streaming it instead of loading it entirely,
/// you avoid saturating the memory and have almost no loading delay.
///
/// Apart from that, a sf::Music has almost the same features as
/// the sf::SoundBuffer / sf::Sound pair: you can play/pause/stop
/// it, request its parameters (channels, sample rate), change
/// the way it is played (pitch, volume, 3D position, ...), etc.
///
/// As a sound stream, a music is played in its own thread in order
/// not to block the rest of the program. This means that you can
/// leave the music alone after calling play(), it will manage itself
/// very well.
///
/// Usage example:
/// \code
/// // Declare a new music
/// sf::Music music;
///
/// // Open it from an audio file
/// if (!music.openFromFile("music.ogg"))
/// {
///     // error...
/// }
///
/// // Change some parameters
/// music.setPosition(0, 1, 10); // change its 3D position
/// music.setPitch(2);           // increase the pitch
/// music.setVolume(50);         // reduce the volume
/// music.setLoop(true);         // make it loop
///
/// // Play it
/// music.play();
/// \endcode
///
/// \see sf::Sound, sf::SoundStream
///
////////////////////////////////////////////////////////////
