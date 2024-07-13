#include "includes/EditorWindow.hpp"

tedit::EditorWindow::EditorWindow(const std::size_t& width, const std::size_t& height)
    : m_window(sf::VideoMode(width, height), WINDOW_TITLE)
{
    tedit::Editor::setFont("assets/monospace.ttf");
}

int
tedit::EditorWindow::open()
{
    auto [width, height] = m_window.getSize();
    tedit::Editor editor(width, height);

    while (m_window.isOpen())
    {
        handleEvents(editor);

        m_window.clear();

        m_window.draw(editor);
        m_window.display();
    }

    return 0;
}

void
tedit::EditorWindow::handleEvents(Editor& editor)
{
    sf::Event event;

    if (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::EventType::Closed)
        {
            m_window.close();
        }
        else if (event.type == sf::Event::EventType::Resized)
        {
            sf::View fixedView(sf::FloatRect(0, 0, event.size.width, event.size.height));
            m_window.setView(fixedView);
            editor.setSize(event.size.width, event.size.height);
        }

        editor.handleEvent(event);
    }
}