
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Audio.hpp>
#include <iomanip>
#include <iostream>

////////////////////////////////////////////////////////////
/// Play a sound
///
////////////////////////////////////////////////////////////
void playSound()
{
    // Load a sound buffer from a wav file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("resources/canary.wav"))
        return;

    // Display sound informations
    std::cout << "canary.wav (Sound):" << std::endl;
    std::cout << " " << buffer.getDuration().asSeconds() << " seconds"       << std::endl;
    std::cout << " " << buffer.getSampleRate()           << " samples / sec" << std::endl;
    std::cout << " " << buffer.getChannelCount()         << " channels"      << std::endl;

    // Create a sound instance and play it
    sf::Sound sound(buffer);
    sound.play();

    // Loop while the sound is playing
    while (sound.getStatus() == sf::Sound::Playing)
    {
        // Leave some CPU time for other processes
        sf::sleep(sf::milliseconds(100));

        // Display the playing position
        std::cout << "\rPlaying... " << std::fixed << std::setprecision(2) << sound.getPlayingOffset().asSeconds() << " sec   ";
        std::cout << std::flush;
    }
    std::cout << std::endl << std::endl;
}


////////////////////////////////////////////////////////////
/// Play a music
///
////////////////////////////////////////////////////////////
void playMusic()
{
    // Load an ogg music file
    sf::Music music;
    if (!music.openFromFile("resources/orchestral.ogg"))
        return;

    // Display music informations
    std::cout << "orchestral.ogg (Music):" << std::endl;
    std::cout << " " << music.getDuration().asSeconds() << " seconds"       << std::endl;
    std::cout << " " << music.getSampleRate()           << " samples / sec" << std::endl;
    std::cout << " " << music.getChannelCount()         << " channels"      << std::endl;

    // Play it
    music.play();

    // Loop while the music is playing
    while (music.getStatus() == sf::Music::Playing)
    {
        // Leave some CPU time for other processes
        sf::sleep(sf::milliseconds(100));

        // Display the playing position
        std::cout << "\rPlaying... " << std::fixed << std::setprecision(2) << music.getPlayingOffset().asSeconds() << " sec   ";
        std::cout << std::flush;
    }
    std::cout << std::endl << std::endl;
}

void playSoundLoop() {
    // Load a sound buffer from a wav file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("resources/canary.wav"))
        return;

    if (!buffer.setLoopPointsFromTime(sf::seconds(2.9f), sf::seconds(4.5f)))
        return;

    // Display sound informations
    std::cout << "canary.wav (Sound, Loop):" << std::endl;
    std::cout << " " << buffer.getDuration().asSeconds() << " seconds"       << std::endl;
    std::cout << " " << buffer.getSampleRate()           << " samples / sec" << std::endl;
    std::cout << " " << buffer.getChannelCount()         << " channels"      << std::endl;
    std::cout << " Looping from " << buffer.getLoopStart().asSeconds() <<
            " sec to " << buffer.getLoopEnd().asSeconds() << " sec"          << std::endl;

    // Create a sound instance and play it
    sf::Sound sound(buffer);
    sound.setLoop(true);
    sound.play();

    // Loop while the sound is playing
    // Make sure it stops after a few loops.
    int loops = 5;
    float lastTime = -1.0f;
    while (sound.getStatus() == sf::Sound::Playing)
    {
        // Leave some CPU time for other processes
        sf::sleep(sf::milliseconds(100));
        float time = sound.getPlayingOffset().asSeconds();
        if (time < lastTime)
        {
            --loops;
            if (!loops) // Reset to full sound on the last loop.
            {
                // This will escape us from the loop points cleanly, with only a brief pause when calling this
                // Using "sound.setLoop(false);" by itself works even better
                // but I figure we can demonstrate that we handle this same-value reset-to-default case just fine.
                // It stops the sound too, as an intended side effect of resetting the source attachments
                buffer.setLoopPointsFromTime(sf::seconds(0.0f), sf::seconds(0.0f));
                sound.setPlayingOffset(sf::seconds(2.9f));
                sound.play();   // Restart this. It will have been stopped when the buffer made the changes.
            }
            else if (loops < 0)
            {
                loops = 0;
                sound.stop();
            }
        }
        lastTime = time;

        // Display the playing position
        std::cout << "\rPlaying... " << std::fixed << std::setprecision(2) << time << " sec with " << loops << " loops to go        ";
        std::cout << std::flush;
    }
    std::cout << std::endl << std::endl;
}

void playMusicLoop() {
    // Load an ogg music file
    sf::Music music;
    if (!music.openFromFile("resources/orchestral.ogg"))
        return;

    if (!music.setLoopPointsFromTime(sf::seconds(5.579f), sf::seconds(13.496f)))
        return;

    // Display sound informations
    std::cout << "orchestral.ogg (Music, Loop):" << std::endl;
    std::cout << " " << music.getDuration().asSeconds() << " seconds"       << std::endl;
    std::cout << " " << music.getSampleRate()           << " samples / sec" << std::endl;
    std::cout << " " << music.getChannelCount()         << " channels"      << std::endl;
    std::cout << " Looping from " << music.getLoopStart().asSeconds() <<
            " sec to " << music.getLoopEnd().asSeconds() << " sec"          << std::endl;

    music.setLoop(true);
    music.play();

    // Loop while the sound is playing
    // Make sure it stops after a few loops.
    int loops = 5;
    float lastTime = -1.0f;
    while (music.getStatus() == sf::Sound::Playing)
    {
        // Leave some CPU time for other processes
        sf::sleep(sf::milliseconds(100));
        float time = music.getPlayingOffset().asSeconds();
        if (time < lastTime)
        {
            --loops;
            if (loops == 3)         // Set to "outer" loop for a few iterations
            {
                // For music, all looping control happens within sf::Music itself,
                // by applying some cleverness to how we "feed" the underlying SoundStream.
                // The points are mutex-guarded, and can be safely changed mid-play.
                music.setLoopPointsFromTime(sf::seconds(13.496f), sf::seconds(5.579f));
            }
            else if (loops == 1)    // Reset to full sound on the last loop.
            {
                // Default: (0,0). An end of 0 defaults to "Sound end", effectively resetting the loop
                music.setLoopPointsFromSamples();
            }
            else if (!loops)
            {
                loops = 0;
                music.stop();
            }
        }
        lastTime = time;

        // Display the playing position
        std::cout << "\rPlaying... " << std::fixed << std::setprecision(2) << time << " sec with " << loops << " loops to go        ";
        std::cout << std::flush;
    }
    std::cout << std::endl << std::endl;
}


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
    // Play a sound
    playSound();

    // Play a music
    playMusic();

    // Play a looping sound with loop points
    playSoundLoop();

    // Play a looping music with loop points
    playMusicLoop();

    // Wait until the user presses 'enter' key
    std::cout << "Press enter to exit..." << std::endl;
    std::cin.ignore(10000, '\n');

    return EXIT_SUCCESS;
}
