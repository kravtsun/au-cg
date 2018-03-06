#ifndef SLOT_MACHINE_WINDOW_MANAGER_H
#define SLOT_MACHINE_WINDOW_MANAGER_H

#include <string>
#include <functional>

struct WindowManager {
    WindowManager(const std::string &win_name, int win_width, int win_height);
    
    std::string win_name() const { return win_name_; }
    int win_width() const { return win_width_; }
    int win_height() const { return win_height_; }
    
    virtual void main_loop(std::function<void()> inside_action) const = 0;
    
    virtual void resize(int new_width, int new_height);
    
    virtual ~WindowManager();
private:
    std::string win_name_;
    int win_width_, win_height_;
};

#endif //SLOT_MACHINE_WINDOW_MANAGER_H
