#include "includes/EditorWindow.hpp"

const std::size_t window_width  = 900;
const std::size_t window_height = 500;

int main()
{
    tedit::EditorWindow window(window_width, window_height);
    return window.open();
}