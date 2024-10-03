#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>

class Song {
public:
    std::string title;
    std::string filepath;

    Song(const std::string& t, const std::string& p) : title(t), filepath(p) {}
};

class Playlist {
private:
    std::vector<Song> songs;

public:
    void addSong(const std::string& title, const std::string& filepath) {
        songs.emplace_back(title, filepath);
    }

    void playNext() {
        if (!songs.empty()) {
            if (Mix_PlayingMusic()) {
                Mix_HaltMusic(); // 停止当前音乐
            }
            Mix_Music* music = Mix_LoadMUS(songs.front().filepath.c_str());
            if (music) {
                Mix_PlayMusic(music, 1);
                std::cout << "Playing: " << songs.front().title << std::endl;
                Mix_FreeMusic(music);
            }
            else {
                std::cerr << "Error loading music: " << Mix_GetError() << std::endl;
            }
            songs.erase(songs.begin()); // 移除已播放的歌曲
        }
        else {
            std::cout << "No songs in playlist." << std::endl;
        }
    }

    void clear() {
        songs.clear();
    }
};

void initAudio() {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
}

void cleanupAudio() {
    Mix_CloseAudio();
    SDL_Quit();
}

void displayMenu(SDL_Renderer* renderer) {
    // 设置背景颜色为白色
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // 设置文本颜色为黑色
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // 在窗口中绘制简单的文本（替代真实的文本绘制方式，因为未实现TTF支持）
    // 绘制按钮文本
    // 这里只是代表文字部分，实际上需要用文本渲染库，例如SDL_ttf
    SDL_Rect rect1 = { 50, 50, 200, 50 };  // 播放下一首按钮
    SDL_RenderFillRect(renderer, &rect1);

    SDL_Rect rect2 = { 50, 120, 200, 50 }; // 添加歌曲按钮
    SDL_RenderFillRect(renderer, &rect2);

    SDL_Rect rect3 = { 50, 190, 200, 50 }; // 退出按钮
    SDL_RenderFillRect(renderer, &rect3);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    initAudio();

    Playlist playlist;
    playlist.addSong("First Song", "path_to_your_audio/song1.mp3"); // 替换为实际音频路径
    playlist.addSong("Second Song", "path_to_your_audio/song2.mp3"); // 替换为实际音频路径

    SDL_Window* window = SDL_CreateWindow("Music Player",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        cleanupAudio();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;
    bool running = true;

    while (running) {
        displayMenu(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; // 窗口关闭事件
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_RETURN: // 按下回车键播放下一首
                    playlist.playNext();
                    break;
                case SDLK_a: // 按下 'A' 键添加歌曲
                {
                    std::string title, filepath;
                    std::cout << "Enter song title: ";
                    std::cin.ignore(); // 清除前一次输入的换行符
                    std::getline(std::cin, title);
                    std::cout << "Enter song filepath: ";
                    std::getline(std::cin, filepath);
                    playlist.addSong(title, filepath);
                    break;
                }
                case SDLK_ESCAPE: // 按下 ESC 键退出
                    running = false;
                    break;
                }
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    cleanupAudio();
    return 0;
}
