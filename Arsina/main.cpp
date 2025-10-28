#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfx.h>

using namespace std;

constexpr int window_width = 1280, window_height = 800;
constexpr float PI = 3.141592653589;

default_random_engine random_generator;
uniform_int_distribution<int> random_distribution(0, 150000);

SDL_Renderer *renderer, *renderer_map, *renderer_pause;
SDL_Window *window, *window_map, *window_pause;
SDL_Cursor *cursor_hand, *cursor_ibeam, *cursor_crosshair, *cursor_arrow;
SDL_Texture *background1, *background2, *shooter_img, *spades_img, *spades2_img, *hearts_img, *hearts2_img, *clubs_img, *clubs2_img, *diamonds_img, *diamonds2_img, *stone_img, *bird_img, *joker_img, *leaderboard_img, *choosemap_img, *background_map, *titap_img, *questionmark_img, *pause_img, *backwards_img, *slowmotion_img, *filter_img, *bomb_all_img, *bomb_img, *thunder_img, *fire_img, *filter_img2, *bomb_img2, *fire_img2, *help_img1, *help_img2;
SDL_Rect background_rect, shooter_rect;
Mix_Music *music[3];
Mix_Chunk *sound[6];
int current_music;

bool sound_on = true, music_on = true;

int user_line, score;

int power_quantity[5];

bool click_wait = true, key_wait = true, exit_pressed = false;

int map_size[4] = {3324, 4215, 2467, 0};

void first_menu(bool just_draw);
void game_menu(bool just_draw);

void delay(long long int amount) {
    long long int start_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    SDL_Event *e = new SDL_Event;
    while (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - start_time < amount) {
        if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_CLOSE) {
            exit_pressed = true;
            return;
        }
        SDL_PollEvent(e);
    }
}

void render() {
    SDL_RenderPresent(renderer);
}

void clean() {
    SDL_RenderClear(renderer);
}

void change_music() {
    current_music = (current_music + 1) % 3;
    Mix_PlayMusic(music[current_music], 0);
    music_on = true;
}

void put_image(SDL_Texture *img, int w, int h, int x, int y) {
    SDL_Rect img_rect;
    img_rect.w = w;
    img_rect.h = h;
    img_rect.x = x - w / 2;
    img_rect.y = y - h / 2;
    SDL_RenderCopy(renderer, img, NULL, &img_rect);
}

void put_text(const char * font_file, int font_size, const char *text, int x, int y, int r, int g, int b, int which = 0) {
    TTF_Font *font = TTF_OpenFont(font_file, font_size);
    int text_w, text_h;
    TTF_SizeText(font, text, &text_w, &text_h);
    SDL_Surface* surfaceMessage = TTF_RenderUTF8_Blended(font, text, {r, g, b});
    SDL_Texture* Message = SDL_CreateTextureFromSurface(which == 1? renderer_map : (which == 2? renderer_pause : renderer), surfaceMessage);
    SDL_Rect Message_rect;
    Message_rect.w = text_w;
    Message_rect.h = text_h;
    Message_rect.x = x - text_w / 2;
    Message_rect.y = y - text_h / 2;
    SDL_RenderCopy(which == 1? renderer_map : (which == 2? renderer_pause : renderer), Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
    TTF_CloseFont(font);
}

void update_powers() {
    vector<string> file_data;
    ifstream file_1;
    file_1.open("accounts.txt");
    string previous, s;
    while (true) {
        file_1 >> s;
        if (s == previous)
            break;
        file_data.push_back(s);
        for (int i = 0; i < 7; i++) {
            file_1 >> s;
            file_data.push_back(s);
        }
        previous = s;
    }
    file_1.close();
    for (int i = 0; i < 5; i++)
        file_data[user_line + 2 + i] = to_string(power_quantity[i]);
    ofstream file_2;
    file_2.open("accounts.txt");
    for (int i = 0; i < file_data.size(); i++)
        file_2 << (i == 0? "" : "\n") << file_data[i];
    file_2.close();
}

void get_powers() {
    ifstream file;
    file.open("accounts.txt");
    string s;
    int current = 0;
    while (current++ < user_line + 2)
        file >> s;
    for (int i = 0; i < 5; i++) {
        file >> s;
        power_quantity[i] = stoi(s);
    }
    file.close();
}

int check_username(string username) {
    ifstream file;
    file.open("accounts.txt");
    string previous, s;
    int current = 1;
    while (true) {
        file >> s;
        if (s == previous)
            break;
        else if (s == username) {
            file.close();
            return current;
        }
        previous = s;
        file >> s >> s >> s >> s >> s >> s >> s;
        current += 8;
    }
    file.close();
    return 0;
}

bool check_password(int line, string password) {
    ifstream file;
    file.open("accounts.txt");
    string s;
    int current = 0;
    while (current++ < line)
        file >> s;
    file.close();
    return password == s;
}

int add_account(string username, string password) {
    ofstream file_1;
    file_1.open("accounts.txt", std::ios_base::app);
    file_1 << '\n' << username << '\n' << password << '\n' << 0 << '\n' << 0 << '\n' << 0 << '\n' << 0 << '\n' << 0 << '\n' << 0;
    file_1.close();
    ifstream file_2;
    file_2.open("accounts.txt");
    string previous, s;
    int current = 1;
    while (true) {
        file_2 >> s;
        if (s == previous)
            break;
        previous = s;
        file_2 >> s >> s >> s >> s >> s >> s >> s;
        current += 8;
    }
    file_2.close();
    return current - 8;
}

bool comparison(pair<int, string> x, pair<int, string> y) {
    return (x.first == y.first? x.second < y.second : x.first > y.first);
}

vector<int> path_box;
vector<pair<float, float>> path_point, path;

bitset<32> full;

void make_path(int current) {
    full[current] = true;
    path_box.push_back(current);
    vector<int> available;
    if (current % 8 != 0 && !full[current - 1])
        available.push_back(current - 1);
    if (current % 8 != 7 && !full[current + 1])
        available.push_back(current + 1);
    if (current > 7 && !full[current - 8])
        available.push_back(current - 8);
    if (current < 24 && !full[current + 8])
        available.push_back(current + 8);
    if (!available.empty())
        make_path(available[rand() % available.size()]);
}

pair<float, float> f(int which_map, float t) {
    if (t < 0)
        return make_pair(-100, -100);
    float theta;
    if (which_map == 0) {
        if (t <= 300)
            return make_pair(50 + t, 750);
        else if (t <= 535) {
            theta = PI / 470.0 * (t - 300.0) + PI * 1.5;
            return make_pair(350.0 + 150.0 * cos(theta), 600.0 - 150.0 * sin(theta));
        }
        else if (t <= 635)
            return make_pair(500, 1135 - t);
        else if (t <= 1295) {
            theta = -4.0 * PI / (140.0 * 2.0 * PI * 2.0 / 3.0 * 3.0) * (t - 635.0) + 11.0 * PI / 6.0;
            return make_pair(500.0 - 70.0 * sqrt(3) + 140.0 * cos(theta), 430 - 140.0 * sin(theta));
        }
        else if (t <= 2029) {
            theta = (-0.5636 - PI) / 734.0 * (t - 1295) + PI + 0.2818;
            return make_pair(640 + 197.8 * cos(theta), 255 - 197.8 * sin(theta));
        }
        else if (t <= 2689) {
            theta = -4.0 * PI / (140.0 * 2.0 * PI * 2.0 / 3.0 * 3.0) * (t - 2029.0) + 16.0 * PI / 6.0;
            return make_pair(780.0 + 70.0 * sqrt(3) + 140.0 * cos(theta), 430 - 140.0 * sin(theta));
        }
        else if (t <= 2789)
            return make_pair(779, t - 2189);
        else if (t <= 3024) {
            theta = PI / 470.0 * (t - 2789.0) + PI;
            return make_pair(930.0 + 150.0 * cos(theta), 600.0 - 150.0 * sin(theta));
        }
        else if (t <= 3324)
            return make_pair(t - 2094, 750);
    }
    else if (which_map == 1) {
        if (t <= 100)
            return make_pair(250, 750 - t);
        else if (t <= 300)
            return make_pair(150 + t, 650);
        else if (t <= 625)
            return make_pair(450, 950 - t);
        else if (t <= 825)
            return make_pair(1075 - t, 325);
        else if (t <= 1000)
            return make_pair(250, t - 500);
        else if (t <= 1150)
            return make_pair(1250 - t, 500);
        else if (t <= 1225)
            return make_pair(100, 1650 - t);
        else if (t <= 1300)
            return make_pair(t - 1125, 425);
        else if (t <= 1475)
            return make_pair(175, 1725 - t);
        else if (t <= 1850)
            return make_pair(t - 1300, 250);
        else if (t <= 1975)
            return make_pair(550, 2100 - t);
        else if (t <= 2230)
            return make_pair(t - 1425, 125);
        else if (t <= 2365)
            return make_pair(2044 - t * 75 / 135, t * 125 / 135 - 1939);
        else if (t <= 2665)
            return make_pair(t - 1635, 250);
        else if (t <= 2840)
            return make_pair(1030, 2915 - t);
        else if (t <= 2990)
            return make_pair(t - 1810, 75);
        else if (t <= 3065)
            return make_pair(1180, t - 2915);
        else if (t <= 3140)
            return make_pair(4245 - t, 150);
        else if (t <= 3315)
            return make_pair(1105, t - 2990);
        else if (t <= 3590)
            return make_pair(4420 - t, 325);
        else if (t <= 3815)
            return make_pair(830, t - 3265);
        else if (t <= 3915)
            return make_pair(t - 2985, 550);
        else if (t <= 4115)
            return make_pair(930, t - 3365);
        else if (t <= 4215)
            return make_pair(t - 3185, 750);
    }
    else if (which_map == 2) {
        if (t <= 100)
            return make_pair(510.0 - t / 2.0, 174.8 - t * sqrt(3) / 2.0);
        else if (t <= 477) {
            theta = PI / 1131.0 * (t - 100.0) + 2.0 * PI / 3.0;
            return make_pair(640.0 + 360.0 * cos(theta), 400.0 - 360.0 * sin(theta));
        }
        else if (t <= 677)
            return make_pair(t - 197, 400);
        else if (t <= 845) {
            theta = PI / 504.0 * (t - 677.0) + PI;
            return make_pair(640.0 + 160.0 * cos(theta), 400.0 - 160.0 * sin(theta));
        }
        else if (t <= 1045)
            return make_pair(560.0 - (t - 845.0) / 2.0, 539.0 + (t - 845.0) * sqrt(3) / 2.0);
        else if (t <= 1422) {
            theta = PI / 1131.0 * (t - 1045.0) + 4.0 * PI / 3.0;
            return make_pair(640.0 + 360.0 * cos(theta), 400.0 - 360.0 * sin(theta));
        }
        else if (t <= 1622)
            return make_pair(820.0 - (t - 1422.0) / 2.0, 712.0 - (t - 1422.0) * sqrt(3) / 2.0);
        else if (t <= 1790) {
            theta = PI / 504.0 * (t - 1622.0) + 5.0 * PI / 3.0;
            return make_pair(640.0 + 160.0 * cos(theta), 400.0 - 160.0 * sin(theta));
        }
        else if (t <= 1990)
            return make_pair(t - 990, 400);
        else if (t <= 2367) {
            theta = PI / 1131.0 * (t - 1990.0);
            return make_pair(640.0 + 360.0 * cos(theta), 400.0 - 360.0 * sin(theta));
        }
        else if (t <= 2467)
            return make_pair(820.0 - (t - 2367.0) / 2.0, 88.0 + (t - 2367.0) * sqrt(3) / 2.0);
    }
    else
        return path[t];
}

struct ball {
    int type;
    float t;
    bool stopped = false, frozen = false, pause = false, slow_motion = false, backwards = false;
    time_t start_effect;
};

float distance_of(pair<int, int> x, pair<int, int> y) {
    return sqrt((x.first - y.first) * (x.first - y.first) + (x.second - y.second) * (x.second - y.second));
}

void update_score() {
    vector<string> file_data;
    ifstream file_1;
    file_1.open("accounts.txt");
    string previous, s;
    while (true) {
        file_1 >> s;
        if (s == previous)
            break;
        file_data.push_back(s);
        for (int i = 0; i < 7; i++) {
            file_1 >> s;
            file_data.push_back(s);
        }
        previous = s;
    }
    file_1.close();
    file_data[user_line + 1] = to_string(stoi(file_data[user_line + 1]) + score);
    ofstream file_2;
    file_2.open("accounts.txt");
    for (int i = 0; i < file_data.size(); i++)
        file_2 << (i == 0? "" : "\n") << file_data[i];
    file_2.close();
}

void won() {
    boxRGBA(renderer, 0, 0, window_width, window_height, 15, 80, 15, 255);
    put_text("dollarbill.ttf", 48, ("You won with " + to_string(score) + " points").c_str(), window_width / 2, window_height / 2, 255, 255, 255);
    render();
    delay(5000);
    int which_power = rand() % 100;
    which_power = (which_power < 2? 4 : (which_power < 11? 3 : (which_power < 30? 2 : (which_power < 60? 1 : 0))));
    power_quantity[which_power]++;
    for (double i = 0; i < 150; i = min(150.0, i + sqrt(sqrt(sqrt(15000 - i * 100))))) {
        if (exit_pressed)
            return;
        clean();
        boxRGBA(renderer, 0, 0, window_width, window_height, 15, 80, 15, 255);
        boxRGBA(renderer, window_width / 2 - 205, window_height / 2 - 205, window_width / 2 + 205, window_height / 2 + 205, 7, 40, 7, 255);
        put_image(which_power == 0? fire_img2 : (which_power == 1? bomb_img2 : (which_power == 2? thunder_img : (which_power == 3? filter_img2 : bomb_all_img))), 80, 80, window_width / 2, window_height / 2 - 50);
        put_text("dollarbill.ttf", 40, which_power == 0? "Fire" : (which_power == 1? "Bomb" : (which_power == 2? "Thunder" : (which_power == 3? "Filter" : "Jackpot"))), window_width / 2, window_height / 2 + 50, 255, 255, 255);
        filledTrigonRGBA(renderer, window_width / 2, window_height / 2 - i, window_width / 2 - 200, window_height / 2 - 200, window_width / 2 + 200, window_height / 2 - 200, 170, 180, 100, 255);
        filledTrigonRGBA(renderer, window_width / 2, window_height / 2 + i, window_width / 2 - 200, window_height / 2 + 200, window_width / 2 + 200, window_height / 2 + 200, 170, 180, 100, 255);
        filledTrigonRGBA(renderer, window_width / 2 - i, window_height / 2, window_width / 2 - 200, window_height / 2 - 200, window_width / 2 - 200, window_height / 2 + 200, 170, 180, 100, 255);
        filledTrigonRGBA(renderer, window_width / 2 + i, window_height / 2, window_width / 2 + 200, window_height / 2 - 200, window_width / 2 + 200, window_height / 2 + 200, 170, 180, 100, 255);
        render();
        if (i == 0)
            delay(2000);
        else
            delay(25);
    }
    delay(5000);
}

void lost() {
    boxRGBA(renderer, 0, 0, window_width, window_height, 45, 7, 7, 255);
    put_image(titap_img, 623, 500, window_width / 2, window_height / 2);
    render();
    delay(5000);
}

int current_ball_type, current_ball_state, next_ball_type;

bitset<4> available_balls;

vector<ball> balls;

void change_ball() {
    available_balls.reset();
    for (int j = 0; j < balls.size(); j++)
        available_balls[balls[j].type] = true;
    if (available_balls.none())
        available_balls[0] = true;
    current_ball_type = next_ball_type;
    next_ball_type = rand() % 4;
    while (!available_balls[next_ball_type])
        next_ball_type = rand() % 4;
    current_ball_state = 0;
}

void destroy_ball(int index, int mode, time_t start_time, time_t &start_effect, float &speed) {
    if (mode == 1 || mode == 2)
        score += 100 / max(5.0, sqrt(difftime(time(NULL), start_time)));
    else if (balls[index].type == 4 || balls[index].type == 5)
        score += (balls[index].type == 4? 5000 : 2000) / max(5.0, sqrt(difftime(time(NULL), start_time)));
    if (sound_on) {
        if (balls[index].type == 5)
            Mix_PlayChannel(0, sound[0], 0);
        else if (balls[index].type == 6)
            Mix_PlayChannel(3, sound[3], 0);
    }
    speed = (balls[index].pause? 0 : (balls[index].slow_motion? 0.2 : (balls[index].backwards? -0.8 : speed)));
    if (balls[index].pause || balls[index].slow_motion || balls[index].backwards)
        start_effect = time(NULL);
    balls.erase(balls.begin() + index);
}

void game(int which_map, int mode) {
    SDL_SetCursor(cursor_crosshair);
    SDL_Event *e = new SDL_Event();
    int mouse_x, mouse_y;
    SDL_RenderCopy(renderer, background1, NULL, &background_rect);
    for (int i = 0; i < map_size[which_map]; i++)
        filledCircleRGBA(renderer, f(which_map, i).first, f(which_map, i).second, 35, (which_map == 3? 170 : 7), (which_map == 3? 180 : 30), (which_map == 3? 50 : 40), 255);
    for (int i = 0; i < map_size[which_map]; i++)
        filledCircleRGBA(renderer, f(which_map, i).first, f(which_map, i).second, 30, (which_map == 0? 90 : (which_map == 1? 175 : which_map == 2? 110 : 7)), (which_map == 0? 15 : (which_map == 1? 150 : which_map == 2? 100 : 40)), (which_map == 0? 15 : (which_map == 1? 0 : (which_map == 2? 40 : 7))), 255);
    for (int i = 0; i < map_size[which_map]; i++)
        filledCircleRGBA(renderer, f(which_map, i).first, f(which_map, i).second, 10, (which_map == 0? 150 : (which_map == 1? 50 : which_map == 2? 220 : 15)), (which_map == 0? 150 : (which_map == 1? 50 : which_map == 2? 220 : 80)), (which_map == 0? 150 :(which_map == 1? 50 : (which_map == 2? 0 : 15))), 255);
    for (int i = 0; i < map_size[which_map]; i++)
        filledCircleRGBA(renderer, f(which_map, i).first, f(which_map, i).second, 5, (which_map == 0? 90 : (which_map == 1? 175 : which_map == 2? 110 : 7)), (which_map == 0? 15 : (which_map == 1? 150 : which_map == 2? 100 : 40)), (which_map == 1? 0 : (which_map == 2? 40 : 7)), 255);
    SDL_Surface *sshot = SDL_CreateRGBSurface(0, window_width, window_height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
    background_map = SDL_CreateTextureFromSurface(renderer, sshot);
    SDL_FreeSurface(sshot);
    int stones, remaining_stones, birds, remaining_birds, power_bar_x = 0, power_bar_speed = -14, fires_left, bomb_x, bomb_y, bomb_radius = 0;
    time_t start_time = time(NULL), start_effect;
    score = 0;
    float send_theta, current_ball_x, current_ball_y, speed = 0.8, light_opacity = 0;
    balls.clear();
    ball new_ball, temporary;
    for (int i = 0; i < map_size[which_map] * 6 / 10; i += 60) {
        new_ball.type = (rand() % 4 == 0 && i != 0? balls.back().type : rand() % 4);
        new_ball.t = i;
        balls.push_back(new_ball);
        available_balls[new_ball.type] = true;
    }
    if (mode == 3) {
        stones = remaining_stones = map_size[which_map] / 60 / 10;
        for (int i = 0; i < map_size[which_map] * 6 / 10 / 60; i += map_size[which_map] * 6 / 10 / 60 / 3) {
            balls[min((int)balls.size() - 2, i + rand() % 8 + 1)].type = 4;
            remaining_stones--;
        }
    }
    else if (mode == 4) {
        birds = remaining_birds = map_size[which_map] / 60 / 10;
        for (int i = 0; i < map_size[which_map] * 6 / 10 / 60; i += map_size[which_map] * 6 / 10 / 60 / 3) {
            balls[min((int)balls.size() - 2, i + rand() % 8 + 1)].type = 5;
            remaining_birds--;
        }
    }
    for (ball &b : balls)
        if (b.type <= 4) {
            if (rand() % 90 == 0)
                b.type = 6;
            else if (rand() % 90 == 0)
                b.type = 7;
            else if (rand() % 90 == 0)
                b.frozen = true;
        }
    current_ball_type = rand() % 4;
    while (!available_balls[current_ball_type])
        current_ball_type = rand() % 4;
    next_ball_type = rand() % 4;
    while (!available_balls[next_ball_type])
        next_ball_type = rand() % 4;
    reverse(balls.begin(), balls.end());
    current_ball_state = 0;
    while (!exit_pressed) {
        clean();
        light_opacity = max(0.0, light_opacity - 0.4);
        bomb_radius -= 2;
        SDL_RenderCopy(renderer, background_map, NULL, &background_rect);
        if (current_ball_state > 1)
            speed = 0;
        else if (difftime(time(NULL), start_effect) >= 7)
            speed = 0.8;
        if (which_map == 1) {
            boxRGBA(renderer, (mode == 1? 15 : 150), 15, (mode == 1? 315 : 450), 145, (which_map == 3? 170 : 7), (which_map == 3? 180 : 30), (which_map == 3? 50 : 40), 255);
            boxRGBA(renderer, (mode == 1? 20 : 155), 20, (mode == 1? 310 : 445), 140, (which_map == 0? 90 : (which_map == 1? 175 : which_map == 2? 110 : 7)), (which_map == 0? 15 : (which_map == 1? 150 : which_map == 2? 100 : 40)), (which_map == 0? 15 : (which_map == 1? 0 : (which_map == 2? 40 : 7))), 255);
            put_text("dollarbill.ttf", 48, to_string(score).c_str(), (mode == 1? 165 : 300), 80, 255, 255, 255);
        }
        else {
            boxRGBA(renderer, 965, 15, 1265, 145, (which_map == 3? 170 : 7), (which_map == 3? 180 : 30), (which_map == 3? 50 : 40), 255);
            boxRGBA(renderer, 970, 20, 1260, 140, (which_map == 0? 90 : (which_map == 1? 175 : which_map == 2? 110 : 7)), (which_map == 0? 15 : (which_map == 1? 150 : which_map == 2? 100 : 40)), (which_map == 0? 15 : (which_map == 1? 0 : (which_map == 2? 40 : 7))), 255);
            put_text("dollarbill.ttf", 48, to_string(score).c_str(), 1115, 80, 255, 255, 255);
        }
        if (mode != 1) {
            boxRGBA(renderer, 15, 15, 145, 145, (which_map == 3? 170 : 7), (which_map == 3? 180 : 30), (which_map == 3? 50 : 40), 255);
            boxRGBA(renderer, 20, 20, 140, 140, (which_map == 0? 90 : (which_map == 1? 175 : which_map == 2? 110 : 7)), (which_map == 0? 15 : (which_map == 1? 150 : which_map == 2? 100 : 40)), (which_map == 0? 15 : (which_map == 1? 0 : (which_map == 2? 40 : 7))), 255);
            put_text("dollarbill.ttf", 48, to_string(mode == 2? int(80 - difftime(time(NULL), start_time)) : (mode == 3? stones : birds)).c_str(), 80, 80, 255, 255, 255);
        }
        if (current_ball_state == 0 || current_ball_state == 2) {
            current_ball_x = 640 + cos(atan2(mouse_y - window_height / 2, mouse_x - window_width / 2)) * 60;
            current_ball_y = 400 + sin(atan2(mouse_y - window_height / 2, mouse_x - window_width / 2)) * 60;
        }
        else if (current_ball_state == 1 || current_ball_state == 3) {
            current_ball_x += cos(send_theta) * 10;
            current_ball_y += sin(send_theta) * 10;
            if (current_ball_type == 8)
                for (int i = 0; i < 50; i++)
                    filledCircleRGBA(renderer, current_ball_x - i * cos(send_theta), current_ball_y - i * sin(send_theta), 30 - i / 5, 255, 2 * i, 0, 3 * i);
            if (0 <= current_ball_x && current_ball_x < 1280 && 0 <= current_ball_y && current_ball_y < 800) {
                int from, to;
                for (int i = 0; i < balls.size(); i++)
                    if (distance_of(f(which_map, balls[i].t), make_pair(current_ball_x, current_ball_y)) <= 60) {
                        if (current_ball_type == 10) {
                            if (balls[i].type >= 4)
                                break;
                            int to_delete = balls[i].type;
                            for (int i = 0; i < balls.size(); i++)
                                if (balls[i].type == to_delete)
                                    destroy_ball(i--, mode, start_time, start_effect, speed);
                            change_ball();
                            break;
                        }
                        else if (current_ball_type == 9) {
                            bomb_x = f(which_map, balls[i].t).first;
                            bomb_y = f(which_map, balls[i].t).second;
                            bomb_radius = 120;
                            for (int j = min((int)balls.size() - 1, i + 2); j >= max(0, i - 2); j--)
                                if (balls[j].type != 4) {
                                    if (balls[j].type == 5)
                                        birds--;
                                    destroy_ball(j, mode, start_time, start_effect, speed);
                                }
                            change_ball();
                            break;
                        }
                        else if (current_ball_type == 8) {
                            if (balls[i].type == 4) {
                                if (fires_left > 0)
                                    current_ball_state = 2;
                                else if (!balls.empty())
                                    change_ball();
                                break;
                            }
                            else if (balls[i].type == 5)
                                birds--;
                            destroy_ball(i, mode, start_time, start_effect, speed);
                            if (fires_left > 0)
                                current_ball_state = 2;
                            else if (!balls.empty())
                                change_ball();
                            break;
                        }
                        if (balls[i].type == 6) {
                            destroy_ball(i, mode, start_time, start_effect, speed);
                            change_ball();
                            break;
                        }
                        if (balls[i].frozen) {
                            balls[i].frozen = false;
                            change_ball();
                            break;
                        }
                        temporary.type = current_ball_type;
                        if (balls[i].t + 60 >= map_size[which_map] || balls[i].t - 60 >= 0 && distance_of(f(which_map, balls[i].t - 60), make_pair(current_ball_x, current_ball_y)) <= distance_of(f(which_map, balls[i].t + 60), make_pair(current_ball_x, current_ball_y))) {
                            temporary.t = balls[i].t - 60;
                            i++;
                        }
                        else
                            temporary.t = balls[i].t + 60;
                        balls.insert(balls.begin() + i, temporary);
                        from = i;
                        to = i;
                        while (from > 0 && !balls[from - 1].frozen && balls[from - 1].type == balls[from].type && balls[from - 1].t - balls[from].t <= 60 + abs(speed))
                            from--;
                        while (to < balls.size() - 1 && !balls[to + 1].frozen && balls[to + 1].type == balls[to].type && balls[to].t - balls[to + 1].t <= 60 + abs(speed))
                            to++;
                        if (to - from > 1) {
                            if (to + 1 < balls.size() && balls[to + 1].type == 5 && balls[to].t - balls[to + 1].t <= 60 + abs(speed)) {
                                destroy_ball(to + 1, mode, start_time, start_effect, speed);
                                birds--;
                            }
                            if (to + 1 < balls.size() && balls[to].t - balls[to + 1].t <= 60 + abs(speed) && balls[to + 1].type == 7)
                                balls[to + 1].type = balls[to].type;
                            if (from - 1 >= 0 && balls[from - 1].t - balls[from].t <= 60 + abs(speed) && balls[from - 1].type == 7)
                                balls[from - 1].type = balls[from].type;
                            if (to + 1 < balls.size() && balls[to].t - balls[to + 1].t <= 60 + abs(speed) && balls[to + 1].frozen)
                                balls[to + 1].frozen = false;
                            if (from - 1 >= 0 && balls[from - 1].t - balls[from].t <= 60 + abs(speed) && balls[from - 1].frozen)
                                balls[from - 1].frozen = false;
                            float keep = balls[from].t;
                            for (int i = to; i >= from; i--)
                                destroy_ball(i, mode, start_time, start_effect, speed);
                            if (from - 1 >= 0 && balls[from - 1].type == 5 && balls[from - 1].t - keep <= 60 + abs(speed)) {
                                destroy_ball(from - 1, mode, start_time, start_effect, speed);
                                birds--;
                            }
                        }
                        if (balls.size() > 1)
                            for (int j = balls.size() - 2; j >= 0; j--)
                                balls[j].t = max(balls[j].t, balls[j + 1].t + 60);
                        if (!balls.empty()) {
                            change_ball();
                            break;
                        }
                    }
            }
            else {
                if (mode == 1 && balls.empty()) {
                    update_score();
                    won();
                    return;
                }
                change_ball();
            }
        }
        else if (current_ball_state == 4) {
            float theta = atan2(mouse_y - window_height / 2, mouse_x - window_width / 2), alpha;
            for (float i = theta - PI / 12.0; i <= theta + PI / 12.0; i += 0.01) {
                alpha = (i > 0? (i > PI? -2.0 * PI : 0) + i : (i < -PI? 2.0 * PI : 0) + i);
                if (PI / 4.0 <= abs(theta) && abs(theta) <= 3.0 * PI / 4.0)
                    lineRGBA(renderer, window_width / 2, window_height / 2, ((alpha > 0.0? 800 : 0) + tan(alpha) * window_width / 2.0 - window_height / 2.0) / tan(alpha), (alpha > 0.0? 800 : 0), 170, 180, 50, 255);
                else
                    lineRGBA(renderer, window_width / 2, window_height / 2, (abs(theta) > PI / 2.0? 0 : 1280), (abs(theta) > PI / 2.0? 800 : 0) + (abs(theta) > PI / 2.0? -1.0 : 1.0) * (tan(alpha) * 640 + window_height / 2), 170, 180, 50, 255);
            }
        }
        if (balls.empty()) {
            if (mode == 1) {
                update_score();
                won();
                return;
            }
            else {
                if (mode == 2)
                    new_ball.type = rand() % 4;
                else if (mode == 3) {
                    if (rand() % 10 == 0 && remaining_stones > 0) {
                        new_ball.type = 4;
                        remaining_stones--;
                    }
                    else
                        new_ball.type = rand() % 4;
                }
                else if (mode == 4) {
                    if (rand() % 10 == 0 && remaining_birds > 0) {
                        new_ball.type = 5;
                        remaining_birds--;
                    }
                    else
                        new_ball.type = rand() % 4;
                }
                new_ball.t = 0;
                balls.push_back(new_ball);
            }
        }
        if (mode == 2 && int(80 - difftime(time(NULL), start_time)) <= 0) {
            update_score();
            won();
            return;
        }
        if (current_ball_state != 4)
            put_image(current_ball_type == 0? spades_img : (current_ball_type == 1? hearts_img : (current_ball_type == 2? clubs_img : (current_ball_type == 3? diamonds_img : (current_ball_type == 8? fire_img : (current_ball_type == 9? bomb_img : filter_img))))), 60, 60, current_ball_x, current_ball_y);
        SDL_RenderCopyEx(renderer, shooter_img, NULL, &shooter_rect, atan2(mouse_y - window_height / 2, mouse_x - window_width / 2) * 180 / 3.141592653589 + 270, NULL, SDL_FLIP_NONE);
        put_image(next_ball_type == 0? spades_img : (next_ball_type == 1? hearts_img : (next_ball_type == 2? clubs_img : diamonds_img)), 20, 20, window_width / 2 - 60 * cos(atan2(mouse_y - window_height / 2, mouse_x - window_width / 2)), window_height / 2 - 60 * sin(atan2(mouse_y - window_height / 2, mouse_x - window_width / 2)));
        for (int i = (speed > 0? balls.size() - 1 : 0); (speed > 0? i >= 0 : i < balls.size()); i += (speed > 0? -1 : 1)) {
            if (balls[i].type < 4 && !balls[i].pause && !balls[i].slow_motion && !balls[i].backwards) {
                if (random_distribution(random_generator) == 0)
                    balls[i].pause = true;
                else if (random_distribution(random_generator) == 0)
                    balls[i].slow_motion = true;
                else if (random_distribution(random_generator) == 0)
                    balls[i].backwards = true;
                balls[i].start_effect = time(NULL);
            }
            if (difftime(time(NULL), balls[i].start_effect) >= 5)
                balls[i].pause = balls[i].slow_motion = balls[i].backwards = false;
            if (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() % 500 < 150 && (balls[i].pause || balls[i].slow_motion || balls[i].backwards))
                put_image(balls[i].pause? pause_img : (balls[i].slow_motion? slowmotion_img : backwards_img), 60, 60, f(which_map, balls[i].t).first, f(which_map, balls[i].t).second);
            else
                put_image(balls[i].frozen? (balls[i].type == 0? spades2_img : (balls[i].type == 1? hearts2_img : (balls[i].type == 2? clubs2_img : diamonds2_img))) : (balls[i].type == 0? spades_img : (balls[i].type == 1? hearts_img : (balls[i].type == 2? clubs_img : (balls[i].type == 3? diamonds_img : (balls[i].type == 4? stone_img : (balls[i].type == 5? bird_img : (balls[i].type == 6? joker_img : questionmark_img))))))), 60, 60, f(which_map, balls[i].t).first, f(which_map, balls[i].t).second);
            if (((speed > 0 && i == balls.size() - 1) || (speed < 0 && i == 0)) || ((speed > 0 && balls[i].t - balls[i + 1].t <= 60 + abs(speed)) || (speed < 0 && balls[i - 1].t - balls[i].t <= 60 + abs(speed)))) {
                balls[i].t = (((speed > 0 && i == balls.size() - 1) || (speed < 0 && i == 0))? balls[i].t + speed : balls[speed > 0? i + 1 : i - 1].t + (speed > 0? 60 : -60));
                if (balls[i].stopped && ((speed > 0 && balls[i + 1].type == balls[i].type) || (speed < 0 && balls[i - 1].type == balls[i].type)) && balls[i].type != 4) {
                    int from, to;
                    from = (speed > 0? i : i - 1);
                    to = (speed > 0? i + 1 : i);
                    while (from > 0 && !balls[from - 1].frozen && balls[from - 1].type == balls[from].type && balls[from - 1].t - balls[from].t <= 60 + abs(speed))
                        from--;
                    while (to < balls.size() - 1 && !balls[to + 1].frozen && balls[to + 1].type == balls[to].type && balls[to].t - balls[to + 1].t <= 60 + abs(speed))
                        to++;
                    if (to - from > 1) {
                        if (to + 1 < balls.size() && balls[to + 1].type == 5 && balls[to].t - balls[to + 1].t <= 60 + abs(speed)) {
                            destroy_ball(to + 1, mode, start_time, start_effect, speed);
                            birds--;
                        }
                        if (to + 1 < balls.size() && balls[to + 1].type == 7)
                            balls[to + 1].type = balls[to].type;
                        if (from - 1 >= 0 && balls[from - 1].type == 7)
                            balls[from - 1].type = balls[from].type;
                        if (to + 1 < balls.size() && balls[to + 1].frozen)
                            balls[to + 1].frozen = false;
                        if (from - 1 >= 0 && balls[from - 1].frozen)
                            balls[from - 1].frozen = false;
                        float keep = balls[from].t;
                        for (int i = to; i >= from; i--)
                            destroy_ball(i, mode, start_time, start_effect, speed);
                        if (from - 1 >= 0 && balls[from - 1].type == 5 && balls[from - 1].t - keep <= 60 + abs(speed)) {
                            destroy_ball(from - 1, mode, start_time, start_effect, speed);
                            birds--;
                            i = from - 1;
                        }
                        else
                            i = from;
                        if (sound_on)
                            Mix_PlayChannel(1, sound[1], 0);
                    }
                }
                balls[i].stopped = false;
            }
            else
                balls[i].stopped = true;
        }
        if (mode != 1 && balls.back().t >= 60) {
            if (mode == 2)
                new_ball.type = rand() % 4;
            else if (mode == 3) {
                if (rand() % 10 == 0 && remaining_stones > 0) {
                    new_ball.type = 4;
                    remaining_stones--;
                }
                else
                    new_ball.type = rand() % 4;
            }
            else if (mode == 4) {
                if (rand() % 10 == 0 && remaining_birds > 0) {
                    new_ball.type = 5;
                    remaining_birds--;
                }
                else
                    new_ball.type = rand() % 4;
            }
            new_ball.t = 0;
            balls.push_back(new_ball);
        }
        if (balls.front().t >= map_size[which_map]) {
            if (mode != 3 || balls.front().type != 4) {
                lost();
                return;
            }
            else {
                destroy_ball(0, mode, start_time, start_effect, speed);
                stones--;
            }
        }
        if (balls.front().type == 4)
            balls.front().t += abs(speed * 15);
        if (mode == 3 && stones == 0) {
            update_score();
            won();
            return;
        }
        else if (mode == 4 && birds == 0) {
            update_score();
            won();
            return;
        }
        power_bar_x += power_bar_speed;
        power_bar_x = max(power_bar_x, 0);
        power_bar_x = min(power_bar_x, 150);
        boxRGBA(renderer, power_bar_x - 155, 195, power_bar_x + 5, 605, (which_map == 3? 170 : 7), (which_map == 3? 180 : 30), (which_map == 3? 50 : 40), 255);
        boxRGBA(renderer, power_bar_x - 150, 200, power_bar_x, 600, (which_map == 0? 90 : (which_map == 1? 175 : which_map == 2? 110 : 7)), (which_map == 0? 15 : (which_map == 1? 150 : which_map == 2? 100 : 40)), (which_map == 0? 15 : (which_map == 1? 0 : (which_map == 2? 40 : 7))), 255);
        put_image(fire_img2, 60, 60, power_bar_x - 35, 230 + 100 / 6 + (60 + 100 / 6) * 0);
        put_image(bomb_img2, 60, 60, power_bar_x - 35, 230 + 100 / 6 + (60 + 100 / 6) * 1);
        put_image(thunder_img, 60, 60, power_bar_x - 35, 230 + 100 / 6 + (60 + 100 / 6) * 2);
        put_image(filter_img2, 60, 60, power_bar_x - 35, 230 + 100 / 6 + (60 + 100 / 6) * 3);
        put_image(bomb_all_img, 60, 60, power_bar_x - 35, 230 + 100 / 6 + (60 + 100 / 6) * 4);
        for (int i = 0; i < 5; i++)
            put_text("dollarbill.ttf", 18, (to_string(power_quantity[i]) + " x").c_str(), power_bar_x - 110, 230 + 100 / 6 + (60 + 100 / 6) * i, 255, 255, 255);
        for (int i = bomb_radius; i >= 0; i -= 20)
            circleRGBA(renderer, bomb_x, bomb_y, i, 255, 0, 0, 255);
        boxRGBA(renderer, 0, 0, window_width, window_height, 255, 200, 150, light_opacity);
        if (e->type == SDL_MOUSEMOTION) {
            SDL_GetMouseState(&mouse_x, &mouse_y);
            if (power_bar_x - 155 <= e->motion.x && e->motion.x <= power_bar_x + 5 && 195 <= e->motion.y && e->motion.y <= 605) {
                power_bar_speed = 14;
                if (power_bar_x - 150 <= e->motion.x && e->motion.x <= power_bar_x - 5 && ((200 + 100 / 6 + (60 + 100 / 6) * 0 <= e->motion.y && e->motion.y <= 260 + 100 / 6 + (60 + 100 / 6) * 0) ||
                    (200 + 100 / 6 + (60 + 100 / 6) * 1 <= e->motion.y && e->motion.y <= 260 + 100 / 6 + (60 + 100 / 6) * 1) || (200 + 100 / 6 + (60 + 100 / 6) * 2 <= e->motion.y && e->motion.y <= 260 + 100 / 6 + (60 + 100 / 6) * 2) ||
                    (200 + 100 / 6 + (60 + 100 / 6) * 3 <= e->motion.y && e->motion.y <= 260 + 100 / 6 + (60 + 100 / 6) * 3) || (200 + 100 / 6 + (60 + 100 / 6) * 4 <= e->motion.y && e->motion.y <= 260 + 100 / 6 + (60 + 100 / 6) * 4)))
                    SDL_SetCursor(cursor_hand);
                else
                    SDL_SetCursor(cursor_arrow);
            }
            else {
                power_bar_speed = -14;
                SDL_SetCursor(cursor_crosshair);
            }
        }
        else if (click_wait && e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
            click_wait = false;
            if (power_bar_x - 150 <= e->motion.x && e->motion.x <= power_bar_x - 5 && 195 <= e->motion.y && e->motion.y <= 605) {
                for (int i = 0; i < 5; i++)
                    if (200 + 100 / 6 + (60 + 100 / 6) * i <= e->motion.y && e->motion.y <= 260 + 100 / 6 + (60 + 100 / 6) * i && power_quantity[i] > 0) {
                        power_quantity[i]--;
                        if (i == 4) {
                            light_opacity = 255;
                            if (sound_on)
                                Mix_PlayChannel(5, sound[5], 0);
                            for (int i = 0; i < balls.size(); i++)
                                if (balls[i].type != 4) {
                                    if (balls[i].type == 5)
                                        birds--;
                                    destroy_ball(i--, mode, start_time, start_effect, speed);
                                }
                            break;
                        }
                        current_ball_state = (i == 2? 4 : 2);
                        current_ball_type = (i == 0? 8 : (i == 1? 9 : 10));
                        fires_left = 3;
                    }
            }
            else if (current_ball_state == 0 || current_ball_state == 2) {
                if (sound_on)
                    Mix_PlayChannel(2, sound[2], 0);
                send_theta = atan2(mouse_y - window_height / 2, mouse_x - window_width / 2);
                current_ball_state = (current_ball_state == 0? 1 : 3);
                fires_left--;
            }
            else if (current_ball_state == 4) {
                if (sound_on)
                    Mix_PlayChannel(4, sound[4], 0);
                float theta = atan2(mouse_y - window_height / 2, mouse_x - window_width / 2), alpha, from = theta - PI / 12.0, to = theta + PI / 12.0, from2, to2;
                from2 = from = (from < 0? 2.0 * PI : 0) + from;
                to2 = to = (to < 0? 2.0 * PI : 0) + to;
                if (from > to) {
                    from2 = 0;
                    to = 2.0 * PI;
                }
                for (int i = 0; i < balls.size(); i++) {
                    alpha = atan2(f(which_map, balls[i].t).second - window_height / 2, f(which_map, balls[i].t).first - window_width / 2);
                    alpha = (alpha < 0? 2.0 * PI : 0) + alpha;
                    if ((from <= alpha && alpha <= to) || (from2 <= alpha && alpha <= to2)) {
                        if (balls[i].type == 4)
                            continue;
                        if (balls[i].type == 5)
                            birds--;
                        destroy_ball(i--, mode, start_time, start_effect, speed);
                    }
                }
                current_ball_state = 0;
                change_ball();
            }
        }
        else if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_ESCAPE) {
                boxRGBA(renderer, 0, 0, window_width, window_height, 0, 0, 0, 128);
                render();
                time_t start_pause = time(NULL);
                window_pause = SDL_CreateWindow("Pause", 760, 200, 400, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
                renderer_pause = SDL_CreateRenderer(window_pause, 1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                SDL_SetCursor(cursor_arrow);
                SDL_RaiseWindow(window_pause);
                boxRGBA(renderer_pause, 0, 0, 400, 600, 0, 35, 29, 255);
                boxRGBA(renderer_pause, 50, 25, 350, 115, 170, 180, 100, 255);
                boxRGBA(renderer_pause, 60, 35, 340, 105, 15, 60, 80, 255);
                put_text("arial.ttf", 32, "Resume", 200, 70, 255, 255, 255, 2);
                boxRGBA(renderer_pause, 50, 140, 350, 230, 170, 180, 100, 255);
                boxRGBA(renderer_pause, 60, 150, 340, 220, 15, 60, 80, 255);
                put_text("arial.ttf", 32, "Sound", 200, 185, 255, 255, 255, 2);
                boxRGBA(renderer_pause, 50, 255, 350, 345, 170, 180, 100, 255);
                boxRGBA(renderer_pause, 60, 265, 340, 335, 15, 60, 80, 255);
                put_text("arial.ttf", 32, "Music", 200, 300, 255, 255, 255, 2);
                boxRGBA(renderer_pause, 50, 370, 350, 460, 170, 180, 100, 255);
                boxRGBA(renderer_pause, 60, 380, 340, 450, 15, 60, 80, 255);
                put_text("arial.ttf", 32, "Change Music", 200, 415, 255, 255, 255, 2);
                boxRGBA(renderer_pause, 50, 485, 350, 575, 170, 180, 100, 255);
                boxRGBA(renderer_pause, 60, 495, 340, 565, 90, 15, 15, 255);
                put_text("arial.ttf", 32, "Exit", 200, 530, 255, 255, 255, 2);
                SDL_RenderPresent(renderer_pause);
                SDL_Event *ee = new SDL_Event();
                while (true) {
                    if (ee->type == SDL_MOUSEMOTION) {
                        if (50 <= ee->motion.x && ee->motion.x <= 350 && (
                            25 <= ee->motion.y && ee->motion.y <= 115 || 140 <= ee->motion.y && ee->motion.y <= 230 || 255 <= ee->motion.y && ee->motion.y <= 345 ||
                            370 <= ee->motion.y && ee->motion.y <= 460 || 485 <= ee->motion.y && ee->motion.y <= 575))
                            SDL_SetCursor(cursor_hand);
                        else
                            SDL_SetCursor(cursor_arrow);
                    }
                    else if (click_wait && ee->type == SDL_MOUSEBUTTONDOWN && ee->button.button == SDL_BUTTON_LEFT) {
                        click_wait = false;
                        if (50 <= ee->motion.x && ee->motion.x <= 350) {
                            if (25 <= ee->motion.y && ee->motion.y <= 115)
                                break;
                            else if (140 <= ee->motion.y && ee->motion.y <= 230)
                                sound_on = !sound_on;
                            else if (255 <= ee->motion.y && ee->motion.y <= 345) {
                                music_on = !music_on;
                                if (music_on)
                                    Mix_ResumeMusic();
                                else
                                    Mix_PauseMusic();
                            }
                            else if (370 <= ee->motion.y && ee->motion.y <= 460)
                                change_music();
                            else if (485 <= ee->motion.y && ee->motion.y <= 575) {
                                SDL_DestroyRenderer(renderer_pause);
                                SDL_DestroyWindow(window_pause);
                                return;
                            }
                        }
                    }
                    else if (ee->type == SDL_MOUSEBUTTONUP && ee->button.button == SDL_BUTTON_LEFT)
                        click_wait = true;
                    else if (ee->type == SDL_WINDOWEVENT && ee->window.event == SDL_WINDOWEVENT_CLOSE)
                        break;
                    SDL_PollEvent(ee);
                }
                start_time += difftime(time(NULL), start_pause);
                SDL_DestroyRenderer(renderer_pause);
                SDL_DestroyWindow(window_pause);
                SDL_SetCursor(cursor_crosshair);
            }
            else if (e->key.keysym.sym == SDLK_SPACE && current_ball_state == 0)
                swap(current_ball_type, next_ball_type);
        }
        else if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT)
            click_wait = true;
        else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_CLOSE)
            exit_pressed = true;
        render();
        SDL_PollEvent(e);
    }
}

void start_menu(bool just_draw = false, int focus = 0) {
    clean();
    boxRGBA(renderer, 0, 0, window_width, window_height, 0, 35, 29, 255);
    put_text("arial.ttf", 36, "←", 150, window_height / 10, 255, 255, 255);
    put_text("arial.ttf", 36, "Choose Mode", window_width / 2, window_height / 10, 255, 255, 255);
    for (int i = 1; i <= 4; i++) {
        if (focus == i) {
            rectangleRGBA(renderer, window_width / 25 * i + window_width / 5 * (i - 1), window_height * 2 / 10, window_width / 25 * i + window_width / 5 * i, window_height * 3 / 10 + 20, 210, 170, 100, 255);
            rectangleRGBA(renderer, window_width / 25 * i + window_width / 5 * (i - 1) + 5, window_height * 2 / 10 + 5, window_width / 25 * i + window_width / 5 * i - 5, window_height * 3 / 10 + 15, 210, 170, 100, 255);
            rectangleRGBA(renderer, window_width / 25 * i + window_width / 5 * (i - 1) + 10, window_height * 2 / 10 + 10, window_width / 25 * i + window_width / 5 * i - 10, window_height * 3 / 10 + 10, 210, 170, 100, 255);
        }
        else
            rectangleRGBA(renderer, window_width / 25 * i + window_width / 5 * (i - 1), window_height * 2 / 10, window_width / 25 * i + window_width / 5 * i, window_height * 3 / 10 + 20, 170, 180, 100, 255);
    }
    put_text("arial.ttf", 28, "Erasure", window_width / 25 * 1 + window_width / 5 * 1 / 2, window_height * 5 / 20 + 10, (focus == 1? 210 : 170), (focus == 1? 170 : 180), 100);
    put_text("arial.ttf", 28, "Timer", window_width / 25 * 2 + window_width / 5 * 3 / 2, window_height * 5 / 20 + 10, (focus == 2? 210 : 170), (focus == 2? 170 : 180), 100);
    put_text("arial.ttf", 28, "Stones", window_width / 25 * 3 + window_width / 5 * 5 / 2, window_height * 5 / 20 + 10, (focus == 3? 210 : 170), (focus == 3? 170 : 180), 100);
    put_text("arial.ttf", 28, "Free Birds", window_width / 25 * 4 + window_width / 5 * 7 / 2, window_height * 5 / 20 + 10, (focus == 4? 210 : 170), (focus == 4? 170 : 180), 100);
    put_image(choosemap_img, 636, 400, window_width / 2, 500);
    put_text("arial.ttf", 28, "Choose Map", window_width / 2, 500, 225, 170, 75);
    render();
    if (just_draw)
        return;
    SDL_Event *e = new SDL_Event();
    while(!exit_pressed) {
        if (e->type == SDL_MOUSEMOTION) {
            if (window_height * 2 / 10 <= e->motion.y && e->motion.y <= window_height * 3 / 10 + 20 && (
                window_width / 25 * 1 + window_width / 5 * 0 <= e->motion.x && e->motion.x <= window_width / 25 * 1 + window_width / 5 * 1 ||
                window_width / 25 * 2 + window_width / 5 * 1 <= e->motion.x && e->motion.x <= window_width / 25 * 2 + window_width / 5 * 2 ||
                window_width / 25 * 3 + window_width / 5 * 2 <= e->motion.x && e->motion.x <= window_width / 25 * 3 + window_width / 5 * 3 ||
                window_width / 25 * 4 + window_width / 5 * 3 <= e->motion.x && e->motion.x <= window_width / 25 * 4 + window_width / 5 * 4))
                SDL_SetCursor(cursor_hand);
            else if (100 <= e->motion.x && e->motion.x <= 200 && window_height / 10 - 20 <= e->motion.y && e->motion.y <= window_height / 10 + 20)
                SDL_SetCursor(cursor_hand);
            else if (window_width / 2 - 117 <= e->motion.x && e->motion.x <= window_width / 2 + 117 && 448 <= e->motion.y && e->motion.y <= 552)
                SDL_SetCursor(cursor_hand);
            else
                SDL_SetCursor(cursor_arrow);
        }
        else if (click_wait && e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
            click_wait = false;
            if (window_width / 2 - 117 <= e->button.x && e->button.x <= window_width / 2 + 117 && 448 <= e->button.y && e->button.y <= 552 && focus != 0) {
                window_map = SDL_CreateWindow("Choose map", 760, 200, 400, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
                renderer_map = SDL_CreateRenderer(window_map, 1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                SDL_RaiseWindow(window_map);
                boxRGBA(renderer_map, 0, 0, 400, 600, 15, 60, 80, 255);
                boxRGBA(renderer_map, 16, 16, 384, 146, 7, 30, 40, 255);
                boxRGBA(renderer_map, 21, 21, 379, 141, 90, 15, 15, 255);
                boxRGBA(renderer_map, 16, 162, 384, 292, 7, 30, 40, 255);
                boxRGBA(renderer_map, 21, 167, 379, 287, 175, 150, 0, 255);
                boxRGBA(renderer_map, 16, 308, 384, 438, 7, 30, 40, 255);
                boxRGBA(renderer_map, 21, 312, 379, 433, 110, 100, 40, 255);
                boxRGBA(renderer_map, 16, 454, 384, 584, 170, 180, 50, 255);
                boxRGBA(renderer_map, 21, 459, 379, 579, 7, 40, 7, 255);
                put_text("dollarbill.ttf", 32, "KHAJ", 200, 75, 150, 150, 150, 1);
                put_text("dollarbill.ttf", 32, "EGYPTIAN", 200, 225, 50, 50, 50, 1);
                put_text("dollarbill.ttf", 32, "RADIOACTIVE", 200, 375, 220, 220, 0, 1);
                put_text("dollarbill.ttf", 32, "RANDOM", 200, 525, 15, 80, 15, 1);
                SDL_RenderPresent(renderer_map);
                SDL_Event *ee = new SDL_Event();
                while (true) {
                    if (ee->type == SDL_MOUSEMOTION) {
                        if (10 <= ee->motion.x && ee->motion.x <= 390 && (10 <= ee->motion.y && ee->motion.y <= 140 ||
                            160 <= ee->motion.y && ee->motion.y <= 290 || 310 <= ee->motion.y && ee->motion.y <= 440 ||
                            460 <= ee->motion.y && ee->motion.y <= 590))
                            SDL_SetCursor(cursor_hand);
                        else
                            SDL_SetCursor(cursor_arrow);
                    }
                    else if (click_wait && ee->type == SDL_MOUSEBUTTONDOWN && ee->button.button == SDL_BUTTON_LEFT) {
                        click_wait = false;
                        if (10 <= ee->motion.x && ee->motion.x <= 390) {
                            if (10 <= ee->motion.y && ee->motion.y <= 140) {
                                SDL_DestroyRenderer(renderer_map);
                                SDL_DestroyWindow(window_map);
                                game(0, focus);
                                return;
                            }
                            else if (160 <= ee->motion.y && ee->motion.y <= 290) {
                                SDL_DestroyRenderer(renderer_map);
                                SDL_DestroyWindow(window_map);
                                game(1, focus);
                                return;
                            }
                            else if (310 <= ee->motion.y && ee->motion.y <= 440) {
                                SDL_DestroyRenderer(renderer_map);
                                SDL_DestroyWindow(window_map);
                                game(2, focus);
                                return;
                            }
                            else if (460 <= ee->motion.y && ee->motion.y <= 590) {
                                SDL_DestroyRenderer(renderer_map);
                                SDL_DestroyWindow(window_map);
                                path_box.clear();
                                while (path_box.size() < 12) {
                                    path_box.clear();
                                    full.reset();
                                    full[0] = true, full[6] = true, full[7] = true, full[11] = true, full[12] = true, full[19] = true, full[20] = true;
                                    int start_box = rand() % 32;
                                    while (full[start_box])
                                        start_box = rand() % 32;
                                    make_path(start_box);
                                }
                                path_point.clear();
                                for (int i : path_box)
                                    path_point.emplace_back(rand() % 90 + (i % 8) * 160 + 35, rand() % 130 + (i / 8) * 200 + 35);
                                for (int i = 1; i < path_point.size(); i++)
                                    while (path_point[i].first == path_point[i - 1].first || path_point[i].second == path_point[i - 1].second)
                                        path_point[i] = (make_pair(rand() % 90 + (path_box[i] % 8) * 160 + 35, rand() % 130 + (path_box[i] / 8) * 200 + 35));
                                float arc_length, m1, m2, m, theta, d;
                                pair<float, float> o, point_1, point_2;
                                path.clear();
                                for (int i = 1; i < path_point.size() - 1; i++) {
                                    m1 = (float)(path_point[i].second - path_point[i - 1].second) / (float)(path_point[i].first - path_point[i - 1].first);
                                    m2 = (float)(path_point[i + 1].second - path_point[i].second) / (float)(path_point[i + 1].first - path_point[i].first);
                                    theta = atan(abs((m2 - m1) / (1.0 + m2 * m1)));
                                    if (distance_of(path_point[i - 1], path_point[i]) * distance_of(path_point[i - 1], path_point[i]) + distance_of(path_point[i + 1], path_point[i]) * distance_of(path_point[i + 1], path_point[i]) < distance_of(path_point[i - 1], path_point[i + 1]) * distance_of(path_point[i - 1], path_point[i + 1]))
                                        theta = PI - theta;
                                    d = 50.0 / tan(theta / 2.0);
                                    point_1.first = path_point[i].first - sqrt(d * d / (1.0 + m1 * m1)) * (path_point[i].first - path_point[i - 1].first < 0? -1.0 : 1.0);
                                    point_1.second = path_point[i].second - sqrt(d * d / (1.0 + 1.0 / m1 / m1)) * (path_point[i].second - path_point[i - 1].second < 0? -1.0 : 1.0);
                                    arc_length = 100.0 * cos(theta / 2.0);
                                    float points_distance = distance_of((i == 1? path_point[0] : point_2), point_1);
                                    if (i == 1)
                                        for (float j = 0; j < points_distance; j++)
                                            path.emplace_back(path_point[0].first + j / points_distance * (point_1.first - path_point[0].first),
                                                              path_point[0].second + j / points_distance * (point_1.second - path_point[0].second));
                                    else
                                        for (float j = 0; j < points_distance; j++)
                                            path.emplace_back(point_2.first + j / points_distance * (point_1.first - point_2.first),
                                                              point_2.second + j / points_distance * (point_1.second - point_2.second));
                                    point_2.first = path_point[i].first + sqrt(d * d / (1.0 + m2 * m2)) * (path_point[i + 1].first - path_point[i].first < 0? -1.0 : 1.0);
                                    point_2.second = path_point[i].second + sqrt(d * d / (1.0 + 1.0 / m2 / m2)) * (path_point[i + 1].second - path_point[i].second < 0? -1.0 : 1.0);
                                    o.first = (point_2.second + point_2.first / m2 - point_1.second - point_1.first / m1) / (1.0 / m2 - 1.0 / m1);
                                    o.second = -o.first / m1 + point_1.second + point_1.first / m1;
                                    float theta_1, theta_2, t1_temporary, t2_temporary, difference = 1000;
                                    for (int k1 = 0; k1 < 2; k1++)
                                        for (int k2 = 0; k2 < 2; k2++)
                                            for (int k3 = 0; k3 < 2; k3++)
                                                for (int k4 = 0; k4 < 2; k4++) {
                                                    t1_temporary = atan(m1) * (k1? 1.0 : -1.0) + (PI / 2.0) * (k2? 1.0 : -1.0);
                                                    t2_temporary = atan(m2) * (k3? 1.0 : -1.0) + (PI / 2.0) * (k4? 1.0 : -1.0);
                                                    if (abs(o.first + 50.0 * cos(t1_temporary) - point_1.first) <= 1 && abs(o.second + 50.0 * sin(t1_temporary) - point_1.second) <= 1 && abs(o.first + 50.0 * cos(t2_temporary) - point_2.first) <= 1 && abs(o.second + 50.0 * sin(t2_temporary) - point_2.second) <= 1) {
                                                        theta_1 = t1_temporary;
                                                        theta_2 = t2_temporary;
                                                    }
                                                }
                                    if (abs(theta_2 - theta_1) > PI)
                                        if (theta_2 < theta_1)
                                            theta_2 += 2.0 * PI;
                                        else
                                            theta_1 += 2.0 * PI;
                                    for (float j = 0; j < arc_length; j++)
                                        path.emplace_back(o.first + cos((theta_2 - theta_1) * j / arc_length + theta_1) * 50.0,
                                                          o.second + sin((theta_2 - theta_1) * j / arc_length + theta_1) * 50.0);
                                    if (i == path_point.size() - 2) {
                                        float points_distance = distance_of(point_2, path_point.back());
                                        for (float j = 0; j < points_distance; j++)
                                            path.emplace_back(point_2.first + j / points_distance * (path_point.back().first - point_2.first),
                                                              point_2.second + j / points_distance * (path_point.back().second - point_2.second));
                                    }
                                }
                                map_size[3] = path.size();
                                game(3, focus);
                                return;
                            }
                        }
                    }
                    else if (ee->type == SDL_MOUSEBUTTONUP && ee->button.button == SDL_BUTTON_LEFT)
                        click_wait = true;
                    else if (ee->type == SDL_WINDOWEVENT && ee->window.event == SDL_WINDOWEVENT_CLOSE)
                        break;
                    SDL_PollEvent(ee);
                }
                SDL_DestroyRenderer(renderer_map);
                SDL_DestroyWindow(window_map);
            }
            else if (window_height * 2 / 10 <= e->button.y && e->button.y <= window_height * 3 / 10 + 20) {
                if (window_width / 25 * 1 + window_width / 5 * 0 <= e->button.x && e->button.x <= window_width / 25 * 1 + window_width / 5 * 1)
                    focus = 1;
                else if (window_width / 25 * 2 + window_width / 5 * 1 <= e->button.x && e->button.x <= window_width / 25 * 2 + window_width / 5 * 2)
                    focus = 2;
                else if (window_width / 25 * 3 + window_width / 5 * 2 <= e->button.x && e->button.x <= window_width / 25 * 3 + window_width / 5 * 3)
                    focus = 3;
                else if (window_width / 25 * 4 + window_width / 5 * 3 <= e->button.x && e->button.x <= window_width / 25 * 4 + window_width / 5 * 4)
                    focus = 4;
                else
                    focus = 0;
            }
            else if (100 <= e->button.x && e->button.x <= 200 && window_height / 10 - 20 <= e->button.y && e->button.y <= window_height / 10 + 20)
                return;
            else
                focus = 0;
            start_menu(true, focus);
        }
        else if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT)
            click_wait = true;
        else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_CLOSE)
            exit_pressed = true;
        SDL_PollEvent(e);
    }
}

void help_menu() {
    clean();
    boxRGBA(renderer, 0, 0, window_width, window_height, 0, 35, 29, 255);
    put_text("arial.ttf", 36, "←", 85, 50, 170, 180, 100);
    put_image(help_img1, window_width / 2, window_height * 6 / 10, window_width / 4, window_height / 2);
    put_image(help_img2, window_width / 2, window_height * 6 / 10, window_width * 3 / 4, window_height / 2 + 30);
    render();
    SDL_Event *e = new SDL_Event();
    while(!exit_pressed) {
        if (e->type == SDL_MOUSEMOTION) {
            if (35 <= e->motion.x && e->motion.x <= 135 && 30 <= e->motion.y && e->motion.y <= 70)
                SDL_SetCursor(cursor_hand);
            else
                SDL_SetCursor(cursor_arrow);
        }
        else if (click_wait && e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
            click_wait = false;
            if (35 <= e->button.x && e->button.x <= 135 && 30 <= e->button.y && e->button.y <= 70)
                return;
        }
        else if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT)
            click_wait = true;
        else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_CLOSE)
            exit_pressed = true;
        SDL_PollEvent(e);
    }
}

void game_menu(bool just_draw = false) {
    clean();
    boxRGBA(renderer, 0, 0, window_width, window_height, 0, 35, 29, 255);
    for (int i = 0; i < 5; i++) {
        rectangleRGBA(renderer, 50 - i, 100 - i, 350 + i, 700 + i, 210, 170, 100, 255);
        rectangleRGBA(renderer, 400 - i, 100 - i, 880 + i, 700 + i, 210, 170, 100, 255);
        rectangleRGBA(renderer, 930 - i, 100 - i, 1230 + i, 700 + i, 210, 170, 100, 255);
    }
    rectangleRGBA(renderer, 60, 110, 340, 690, 210, 170, 100, 255);
    rectangleRGBA(renderer, 410, 110, 870, 690, 210, 170, 100, 255);
    rectangleRGBA(renderer, 940, 110, 1220, 690, 210, 170, 100, 255);
    rectangleRGBA(renderer, 80, 155, 320, 255, 170, 180, 100, 255);
    rectangleRGBA(renderer, 80, 285, 320, 385, 170, 180, 100, 255);
    rectangleRGBA(renderer, 80, 415, 320, 515, 170, 180, 100, 255);
    rectangleRGBA(renderer, 80, 545, 320, 645, 170, 180, 100, 255);
    put_text("arial.ttf", 36, "Sound", 200, 205, 210, 170, 100);
    put_text("arial.ttf", 36, "Music", 200, 335, 210, 170, 100);
    put_text("arial.ttf", 36, "Help", 200, 465, 210, 170, 100);
    put_text("arial.ttf", 36, "Return", 200, 595, 210, 170, 100);
    put_text("dollarbill.ttf", 48, "Start", window_width / 2, window_height / 2, 210, 170, 100);
    aacircleRGBA(renderer, window_width / 2, window_height / 2, 170, 170, 180, 100, 255);
    aacircleRGBA(renderer, window_width / 2, window_height / 2, 160, 170, 180, 100, 255);
    put_image(leaderboard_img, 94, 128, 1080, 100);
    lineRGBA(renderer, 1080, 165, 1080, 680, 210, 170, 100, 255);
    lineRGBA(renderer, 1075, 165, 1075, 680, 210, 170, 100, 255);
    lineRGBA(renderer, 1085, 165, 1085, 680, 210, 170, 100, 255);
    lineRGBA(renderer, 950, 205, 1210, 205, 210, 170, 100, 255);
    lineRGBA(renderer, 950, 200, 1210, 200, 210, 170, 100, 255);
    put_text("arial.ttf", 20, "Username", 1013, 180, 210, 170, 100);
    put_text("arial.ttf", 20, "Score", 1147, 180, 210, 170, 100);
    ifstream file;
    file.open("accounts.txt");
    vector<pair<int, string>> users;
    string s, previous, username, password;
    int score;
    while (true) {
        file >> username;
        if (username == previous)
            break;
        file >> password >> score >> s >> s >> s >> s >> s;
        users.push_back(make_pair(score, username));
        previous = username;
    }
    file.close();
    sort(users.begin(), users.end(), comparison);
    for (int i = 0; i < users.size(); i++) {
        put_text("arial.ttf", 16, users[i].second.c_str(), 1013, 225 + i * 30, 170, 180, 100);
        put_text("arial.ttf", 16, to_string(users[i].first).c_str(), 1147, 225 + i * 30, 170, 180, 100);
    }
    render();
    if (just_draw)
        return;
    SDL_Event *e = new SDL_Event();
    while(!exit_pressed) {
        if (e->type == SDL_MOUSEMOTION) {
            if ((e->motion.x - window_width / 2) * (e->motion.x - window_width / 2) + (e->motion.y - window_height / 2) * (e->motion.y - window_height / 2) <= 170 * 170)
                SDL_SetCursor(cursor_hand);
            else if (80 <= e->motion.x && e->motion.x <= 320 && (155 <= e->motion.y && e->motion.y <= 255 ||
                                                                 285 <= e->motion.y && e->motion.y <= 385 ||
                                                                 415 <= e->motion.y && e->motion.y <= 515 ||
                                                                 545 <= e->motion.y && e->motion.y <= 645))
                SDL_SetCursor(cursor_hand);
            else
                SDL_SetCursor(cursor_arrow);
        }
        else if (click_wait && e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
            click_wait = false;
            if ((e->button.x - window_width / 2) * (e->button.x - window_width / 2) + (e->button.y - window_height / 2) * (e->button.y - window_height / 2) <= 170 * 170) {
                start_menu();
                game_menu(true);
            }
            else if (80 <= e->button.x && e->button.x <= 320) {
                if (155 <= e->button.y && e->button.y <= 255)
                    sound_on = !sound_on;
                else if (285 <= e->button.y && e->button.y <= 385) {
                    music_on = !music_on;
                    if (music_on)
                        Mix_ResumeMusic();
                    else
                        Mix_PauseMusic();
                }
                else if (415 <= e->button.y && e->button.y <= 515) {
                    help_menu();
                    game_menu(true);
                }
                else if (545 <= e->button.y && e->button.y <= 645)
                    return;
            }
        }
        else if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT)
            click_wait = true;
        else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_CLOSE)
            exit_pressed = true;
        SDL_PollEvent(e);
    }
}

bool submit(bool is_login, string username, string password) {
    int line = check_username(username);
    if ((is_login && line != 0 && check_password(line + 1, password)) || (!is_login && line == 0 && 3 <= username.size() && username.size() <= 13)) {
        user_line = (is_login? line : add_account(username, password));
        get_powers();
        game_menu();
        update_powers();
        return true;
    }
    return false;
}

void register_login(bool is_login, bool just_draw = false, string username = "", string password = "", int focus = 0) {
    clean();
    SDL_RenderCopy(renderer, background2, NULL, &background_rect);
    put_text("dollarbill.ttf", 48, (is_login? "Login" : "Register"), window_width / 2, window_height * 3 / 20, 255, 255, 255);
    put_text("arial.ttf", 36, "←", window_width * 3 / 20 + 10, window_height * 2 / 20, 255, 255, 255);
    boxRGBA(renderer, window_width / 3, window_height * 3 / 10, window_width * 2 / 3, window_height * 7 / 10 + 50, 170, 180, 100, 255);
    rectangleRGBA(renderer, window_width / 3 + 10, window_height * 3 / 10 + 10, window_width * 2 / 3 - 10, window_height * 7 / 10 + 40, 255, 255, 255, 255);
    boxRGBA(renderer, window_width / 3 + 50, window_height * 3 / 10 + 50, window_width * 2 / 3 - 50, window_height / 2 - 15, (focus == 1? 200 : 255), (focus == 1? 200 : 255), (focus == 1? 200 : 255), 255);
    boxRGBA(renderer, window_width / 3 + 50, window_height / 2 + 15, window_width * 2 / 3 - 50, window_height * 7 / 10 - 50, (focus == 2? 200 : 255), (focus == 2? 200 : 255), (focus == 2? 200 : 255), 255);
    boxRGBA(renderer, window_width / 3 + 150, window_height * 7 / 10 - 30, window_width * 2 / 3 - 150, window_height * 7 / 10 + 20, 15, 60, 80, 255);
    if (username.empty())
        put_text("arial.ttf", 24, "Username", window_width / 2, window_height * 2 / 5 + 35 / 2, 150, 150, 150);
    else
        put_text("arial.ttf", 24, username.c_str(), window_width / 2, window_height * 2 / 5 + 35 / 2, 0, 0, 0);
    if (password.empty())
        put_text("arial.ttf", 24, "Password", window_width / 2, window_height * 3 / 5 - 35 / 2, 150, 150, 150);
    else
        put_text("arial.ttf", 24, string(password.size(), '*').c_str(), window_width / 2, window_height * 3 / 5 - 35 / 2, 0, 0, 0);
    put_text("arial.ttf", 24, "Submit", window_width / 2, window_height * 7 / 10 - 5, 255, 255, 255);
    render();
    if (just_draw)
        return;
    SDL_Event *e = new SDL_Event();
    while(!exit_pressed) {
        if (e->type == SDL_MOUSEMOTION) {
            if (window_width / 3 + 50 <= e->motion.x && e->motion.x <= window_width * 2 / 3 - 50 && window_height * 3 / 10 + 50 <= e->motion.y && e->motion.y <= window_height / 2 - 15)
                SDL_SetCursor(cursor_ibeam);
            else if (window_width / 3 + 50 <= e->motion.x && e->motion.x <= window_width * 2 / 3 - 50 && window_height / 2 + 15 <= e->motion.y && e->motion.y <= window_height * 7 / 10 - 50)
                SDL_SetCursor(cursor_ibeam);
            else if (window_width / 3 + 150 <= e->motion.x && e->motion.x <= window_width * 2 / 3 - 150 && window_height * 7 / 10 - 30 <= e->motion.y && e->motion.y <= window_height * 7 / 10 + 20)
                SDL_SetCursor(cursor_hand);
            else if (window_width * 3 / 20 - 40 <= e->motion.x && e->motion.x <= window_width * 3 / 20 + 60 && window_height * 2 / 20 - 20 <= e->motion.y && e->motion.y <= window_height * 2 / 20 + 20)
                SDL_SetCursor(cursor_hand);
            else
                SDL_SetCursor(cursor_arrow);
        }
        else if (click_wait && e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
            click_wait = false;
            if (window_width / 3 + 50 <= e->button.x && e->button.x <= window_width * 2 / 3 - 50 && window_height * 3 / 10 + 50 <= e->button.y && e->button.y <= window_height / 2 - 15) {
                focus = 1;
                register_login(is_login, true, username, password, focus);
            }
            else if (window_width / 3 + 50 <= e->button.x && e->button.x <= window_width * 2 / 3 - 50 && window_height / 2 + 15 <= e->button.y && e->button.y <= window_height * 7 / 10 - 50) {
                focus = 2;
                register_login(is_login, true, username, password, focus);
            }
            else if (window_width * 3 / 20 - 40 <= e->button.x && e->button.x <= window_width * 3 / 20 + 60 && window_height * 2 / 20 - 20 <= e->button.y && e->button.y <= window_height * 2 / 20 + 20)
                return;
            else if (window_width / 3 + 150 <= e->motion.x && e->motion.x <= window_width * 2 / 3 - 150 && window_height * 7 / 10 - 30 <= e->motion.y && e->motion.y <= window_height * 7 / 10 + 20) {
                if (!username.empty() && !password.empty() && submit(is_login, username, password))
                    return;
            }
            else {
                focus = 0;
                register_login(is_login, true, username, password, focus);
            }
        }
        else if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT)
            click_wait = true;
        else if (e->type == SDL_KEYDOWN && key_wait) {
            if (focus == 1) {
                if ((SDLK_0 <= e->key.keysym.sym && e->key.keysym.sym <= SDLK_9) || (SDLK_a <= e->key.keysym.sym && e->key.keysym.sym <= SDLK_z))
                    username += e->key.keysym.sym;
                else if (e->key.keysym.sym == SDLK_BACKSPACE && !username.empty()) {
                    username.pop_back();
                    key_wait = false;
                }
                register_login(is_login, true, username, password, focus);
            }
            else if (focus == 2) {
                if ((SDLK_0 <= e->key.keysym.sym && e->key.keysym.sym <= SDLK_9) || (SDLK_a <= e->key.keysym.sym && e->key.keysym.sym <= SDLK_z))
                    password += e->key.keysym.sym;
                else if (e->key.keysym.sym == SDLK_BACKSPACE && !password.empty()) {
                    password.pop_back();
                    key_wait = false;
                }
                register_login(is_login, true, username, password, focus);
            }
            if (e->key.keysym.sym == SDLK_KP_ENTER || e->key.keysym.sym == SDLK_RETURN) {
                key_wait = false;
                if (!username.empty() && !password.empty() && submit(is_login, username, password))
                    return;
            }
        }
        else if (e->type == SDL_KEYUP)
            key_wait = true;
        else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_CLOSE)
            exit_pressed = true;
        SDL_PollEvent(e);
    }
}

void first_menu(bool just_draw = false) {
    clean();
    SDL_RenderCopy(renderer, background2, NULL, &background_rect);
    put_text("dollarbill.ttf", 72, "ARSINA", window_width / 2, window_height * 3 / 20, 255, 255, 255);
    boxRGBA(renderer, window_width / 3, window_height * 3 / 10, window_width * 2 / 3, window_height * 7 / 10, 170, 180, 100, 255);
    rectangleRGBA(renderer, window_width / 3 + 10, window_height * 3 / 10 + 10, window_width * 2 / 3 - 10, window_height * 7 / 10 - 10, 255, 255, 255, 255);
    boxRGBA(renderer, window_width / 3 + 50, window_height * 3 / 10 + 50, window_width * 2 / 3 - 50, window_height / 2 - 15, 15, 60, 80, 255);
    boxRGBA(renderer, window_width / 3 + 50, window_height / 2 + 15, window_width * 2 / 3 - 50, window_height * 7 / 10 - 50, 15, 60, 80, 255);
    put_text("arial.ttf", 36, "Login", window_width / 2, window_height * 2 / 5 + 35 / 2, 255, 255, 255);
    put_text("arial.ttf", 36, "Register", window_width / 2, window_height * 3 / 5 - 35 / 2, 255, 255, 255);
    render();
    if (just_draw)
        return;
    SDL_Event *e = new SDL_Event();
    while(!exit_pressed) {
        if (e->type == SDL_MOUSEMOTION) {
            if (window_width / 3 + 50 <= e->motion.x && e->motion.x <= window_width * 2 / 3 - 50 && window_height * 3 / 10 + 50 <= e->motion.y && e->motion.y <= window_height / 2 - 15)
                SDL_SetCursor(cursor_hand);
            else if (window_width / 3 + 50 <= e->motion.x && e->motion.x <= window_width * 2 / 3 - 50 && window_height / 2 + 15 <= e->motion.y && e->motion.y <= window_height * 7 / 10 - 50)
                SDL_SetCursor(cursor_hand);
            else
                SDL_SetCursor(cursor_arrow);
        }
        else if (click_wait && e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
            click_wait = false;
            if (window_width / 3 + 50 <= e->button.x && e->button.x <= window_width * 2 / 3 - 50 && window_height * 3 / 10 + 50 <= e->button.y && e->button.y <= window_height / 2 - 15) {
                register_login(true);
                first_menu(true);
            }
            else if (window_width / 3 + 50 <= e->button.x && e->button.x <= window_width * 2 / 3 - 50 && window_height / 2 + 15 <= e->button.y && e->button.y <= window_height * 7 / 10 - 50) {
                register_login(false);
                first_menu(true);
            }
        }
        else if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT)
            click_wait = true;
        else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_CLOSE)
            exit_pressed = true;
        SDL_PollEvent(e);
    }
}

void initialize() {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    window = SDL_CreateWindow("ARSINA", 320, 100, window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RaiseWindow(window);
    cursor_hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    cursor_ibeam = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    cursor_crosshair = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    cursor_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    background1 = IMG_LoadTexture(renderer, "background1.png");
    background2 = IMG_LoadTexture(renderer, "background2.png");
    background_rect.w = window_width;
    background_rect.h = window_height;
    background_rect.x = 0;
    background_rect.y = 0;
    shooter_img = IMG_LoadTexture(renderer, "shooter.png");
    shooter_rect.w = 200;
    shooter_rect.h = 240;
    shooter_rect.x = window_width / 2 - 100;
    shooter_rect.y = window_height / 2 - 120;
    spades_img = IMG_LoadTexture(renderer, "spades.png");
    hearts_img = IMG_LoadTexture(renderer, "hearts.png");
    clubs_img = IMG_LoadTexture(renderer, "clubs.png");
    diamonds_img = IMG_LoadTexture(renderer, "diamonds.png");
    spades2_img = IMG_LoadTexture(renderer, "spades2.png");
    hearts2_img = IMG_LoadTexture(renderer, "hearts2.png");
    clubs2_img = IMG_LoadTexture(renderer, "clubs2.png");
    diamonds2_img = IMG_LoadTexture(renderer, "diamonds2.png");
    stone_img = IMG_LoadTexture(renderer, "stone.png");
    bird_img = IMG_LoadTexture(renderer, "bird.png");
    joker_img = IMG_LoadTexture(renderer, "joker.png");
    leaderboard_img = IMG_LoadTexture(renderer, "leaderboard.png");
    choosemap_img = IMG_LoadTexture(renderer, "choosemap.png");
    titap_img = IMG_LoadTexture(renderer, "titap.png");
    questionmark_img = IMG_LoadTexture(renderer, "questionmark.png");
    pause_img = IMG_LoadTexture(renderer, "pause.png");
    backwards_img = IMG_LoadTexture(renderer, "backwards.png");
    slowmotion_img = IMG_LoadTexture(renderer, "slowmotion.png");
    filter_img = IMG_LoadTexture(renderer, "filter.png");
    thunder_img = IMG_LoadTexture(renderer, "thunder.png");
    bomb_img = IMG_LoadTexture(renderer, "bomb.png");
    bomb_all_img = IMG_LoadTexture(renderer, "bomb_all.png");
    fire_img = IMG_LoadTexture(renderer, "fire.png");
    filter_img2 = IMG_LoadTexture(renderer, "filter2.png");
    bomb_img2 = IMG_LoadTexture(renderer, "bomb2.png");
    fire_img2 = IMG_LoadTexture(renderer, "fire2.png");
    help_img1 = IMG_LoadTexture(renderer, "help1.png");
    help_img2 = IMG_LoadTexture(renderer, "help2.png");
    random_generator.seed(time(NULL));
}

int main(int argc, char *argv[]) {
    initialize();
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
    music[0] = Mix_LoadMUS("Sing, Sing, Sing.wav");
    music[1] = Mix_LoadMUS("Victory.wav");
    music[2] = Mix_LoadMUS("In The Mood.wav");
    sound[0] = Mix_LoadWAV("eagle.wav");
    sound[1] = Mix_LoadWAV("impact.wav");
    sound[2] = Mix_LoadWAV("shoot.wav");
    sound[3] = Mix_LoadWAV("clown.wav");
    sound[4] = Mix_LoadWAV("thunder.wav");
    sound[5] = Mix_LoadWAV("explosion.wav");
    Mix_PlayMusic(music[0], 0);
    Mix_HookMusicFinished(change_music);
    first_menu();
    return 0;
}
