#include <SDL.h>
#include <array>
#include <vector>

class Graphics {
private:
    SDL_Window*   window;
    SDL_Renderer* renderer;

    std::array<std::array<bool, 64>, 32> pixels = {};

    std::vector<SDL_FRect> rects;

    int size_x, size_y;

    void prepare_imgui();
    void prepare_game_gfx();

public:
    Graphics();
    ~Graphics();

    void draw();
    void clear();

    // other overloads too cuz why not
    auto operator()(int y, int x) const { return pixels[y][x]; }

    auto& operator()(int y, int x) { return pixels[y][x]; }
};