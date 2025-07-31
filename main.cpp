#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <string>

namespace fs = std::filesystem;

class MarqueeText {
private:
    std::string text;
    size_t position;
    size_t displayWidth;
    std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
    std::chrono::milliseconds delay;
    bool needsMarquee;

public:
    MarqueeText(const std::string& t, size_t width, std::chrono::milliseconds delay = std::chrono::milliseconds(200))
        : text(t), position(0), displayWidth(width), delay(delay) {
        needsMarquee = text.length() > displayWidth;
        lastUpdate = std::chrono::steady_clock::now();
    }

    std::string getDisplayText() {
        if (!needsMarquee) {
            return text;
        }

        auto now = std::chrono::steady_clock::now();
        if (now - lastUpdate > delay) {
            position = (position + 1) % (text.length() + 3); // +3 for spacing
            lastUpdate = now;
        }

        std::string display;
        for (size_t i = 0; i < displayWidth; ++i) {
            size_t textPos = (position + i) % (text.length() + 3);
            if (textPos < text.length()) {
                display += text[textPos];
            } else {
                display += ' '; // Add spaces after text
            }
        }

        return display;
    }
};

void displayPlayer(const std::string& songName, const std::string& status, MarqueeText& marquee) {
    system("clear || cls");

    std::cout << "╔════════════════════════════╗\n";
    std::cout << "║     🎵 Terminal Player     ║\n";
    std::cout << "╠════════════════════════════╣\n";
    std::cout << "║ Now Playing: " << std::left << std::setw(14) << marquee.getDisplayText() << "║\n";
    std::cout << "║ Status     : " << std::left << std::setw(14) << status << "║\n";
    std::cout << "╠════════════════════════════╣\n";
    std::cout << "║ Controls:                  ║\n";
    std::cout << "║ [P] Play/Pause             ║\n";
    std::cout << "║ [S] Stop                   ║\n";
    std::cout << "║ [Q] Quit                   ║\n";
    std::cout << "╚════════════════════════════╝\n";
}

void displaySongList(const std::vector<fs::path>& songs) {
    system("clear || cls");

    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║          🎼 Daftar Lagu                ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";

    for (size_t i = 0; i < songs.size(); ++i) {
        std::string songEntry = std::to_string(i + 1) + ". " + songs[i].filename().string();
        if (songEntry.length() > 36) {
            songEntry = songEntry.substr(0, 33) + "...";
        }
        std::cout << "║ " << std::left << std::setw(38) << songEntry << " ║\n";
    }

    std::cout << "╚════════════════════════════════════════╝\n";
}

int main() {
    std::string folderPath = "music";
    std::vector<fs::path> laguList;

    // Scan folder
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension().string();
            if (ext == ".wav" || ext == ".WAV" || ext == ".mp3" || ext == ".MP3" || ext == ".ogg" || ext == ".OGG") {
                laguList.push_back(entry.path());
            }
        }
    }

    if (laguList.empty()) {
        std::cout << "╔════════════════════════════╗\n";
        std::cout << "║   Gak ada lagu yang bisa    ║\n";
        std::cout << "║        diputar.            ║\n";
        std::cout << "╚════════════════════════════╝\n";
        return 1;
    }

    // Tampilkan daftar lagu dengan ASCII art
    displaySongList(laguList);

    // Pilih lagu
    std::cout << "\n╔════════════════════════════╗\n";
    std::cout << "║ Masukkan nomor lagu yang   ║\n";
    std::cout << "║ mau diputar:               ║\n";
    std::cout << "╚════════════════════════════╝\n";
    std::cout << "➤ ";
    int pilihan;
    std::cin >> pilihan;

    if (pilihan < 1 || pilihan > laguList.size()) {
        std::cout << "╔════════════════════════════╗\n";
        std::cout << "║     Nomor gak valid.       ║\n";
        std::cout << "╚════════════════════════════╝\n";
        return 1;
    }

    sf::Music music;
    if (!music.openFromFile(laguList[pilihan - 1].string())) {
        std::cout << "╔════════════════════════════╗\n";
        std::cout << "║     Gagal buka file.       ║\n";
        std::cout << "╚════════════════════════════╝\n";
        return 1;
    }

    std::atomic<bool> stop(false);
    std::atomic<bool> paused(false);
    std::string currentSong = laguList[pilihan - 1].filename().string();
    MarqueeText marquee(currentSong, 14); // 14 characters width for display

    // Display initial player
    displayPlayer(currentSong, "Playing", marquee);

    // Thread buat input kontrol
    std::thread inputThread([&]() {
        char cmd;
        while (!stop) {
            std::cin >> cmd;
            cmd = tolower(cmd);
            if (cmd == 'p') {
                if (!paused) {
                    music.pause();
                    displayPlayer(currentSong, "Paused", marquee);
                    paused = true;
                } else {
                    music.play();
                    displayPlayer(currentSong, "Playing", marquee);
                    paused = false;
                }
            } else if (cmd == 's') {
                music.stop();
                displayPlayer(currentSong, "Stopped", marquee);
                paused = false;
            } else if (cmd == 'q') {
                stop = true;
                music.stop();
                displayPlayer(currentSong, "Quitting", marquee);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                break;
            }
        }
    });

    // Main thread untuk update display dan musik
    music.play();
    while (!stop) {
        displayPlayer(currentSong,
                     paused ? "Paused" : (music.getStatus() == sf::SoundSource::Status::Playing ? "Playing" : "Stopped"),
                     marquee);

        auto status = music.getStatus();
        if (status == sf::SoundSource::Status::Stopped && !paused) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    stop = true;
    inputThread.join();

    return 0;
}
