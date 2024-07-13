#ifndef TEDIT_EDITOR_WINDOW_HPP
#define TEDIT_EDITOR_WINDOW_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include "Editor.hpp"

#define WINDOW_TITLE "tedit"

namespace tedit
{
    class EditorWindow
    {
    private: sf::RenderWindow m_window;
        
    public:
        EditorWindow(const std::size_t&,
                     const std::size_t&);

        int
        open();

    private:
        void
        handleEvents(Editor&);
    };
}

#endif // TEDIT_EDITOR_WINDOW_HPP