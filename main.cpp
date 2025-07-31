#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;

void displayPlayer(const std::string& songName, const std::string& status) {
    system("clear || cls"); // Clear the terminal (works for both Linux/macOS and Windows)

    std::cout << "╔════════════════════════════╗\n";
    std::cout << "║     🎵 Terminal Player     ║\n";
    std::cout << "╠════════════════════════════╣\n";
    std::cout << "║ Now Playing: " << std::left << std::setw(14) << songName << " \n";
    std::cout << "║ Status     : " << std::left << std::setw(14) << status << " \n";
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
        // Trim if too long
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
        std::cout << "║   Gak ada lagu yang bisa   ║\n";
        std::cout << "║        diputar.           ║\n";
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

    // Display initial player
    displayPlayer(currentSong, "Playing");

    // Thread buat input kontrol
    std::thread inputThread([&]() {
        char cmd;
        while (!stop) {
            std::cin >> cmd;
            cmd = tolower(cmd);
            if (cmd == 'p') {
                if (!paused) {
                    music.pause();
                    displayPlayer(currentSong, "Paused");
                    paused = true;
                } else {
                    music.play();
                    displayPlayer(currentSong, "Playing");
                    paused = false;
                }
            } else if (cmd == 's') {
                music.stop();
                displayPlayer(currentSong, "Stopped");
                paused = false;
            } else if (cmd == 'q') {
                stop = true;
                music.stop();
                displayPlayer(currentSong, "Quitting");
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                break;
            }
        }
    });

    // Main thread muter lagu
    music.play();

    // Tunggu sampai lagu selesai atau user quit
    while (!stop) {
        auto status = music.getStatus();
        if (status == sf::SoundSource::Status::Stopped && !paused) {
            break; // lagu selesai dan gak dalam keadaan pause
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    stop = true;
    inputThread.join();

    return 0;
}
