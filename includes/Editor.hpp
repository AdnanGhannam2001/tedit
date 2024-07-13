#ifndef TEDIT_EDITOR_HPP
#define TEDIT_EDITOR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <numeric>
#include <cstring>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include "Scroller.hpp"

#define TEDIT_SCROLL_SIZE 7

namespace tedit
{
    class Editor;

    class Editor : public sf::Drawable
    {
    public:
        struct Font
        {
            sf::Font    font;
            std::string family;
            std::size_t size;
            float       glyph;
            bool        bold;
        };

        struct Position
        {
            std::size_t row;
            std::size_t column;

            static std::pair<Position, Position>
            minmax(const Position&, const Position&);
        };

        struct Size
        {
            std::size_t width;
            std::size_t height;
        };

        struct Color
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };

        enum class Direction
        {
            Begin,
            End,
            Up,
            Down,
            Right,
            Left,
        };

        class Line : public sf::Drawable
        {
        private:
            sf::RectangleShape m_shape; // ! Not Used Yet
            std::size_t        m_index;

            std::string m_content;
            sf::Text    m_sf_text;

            sf::RectangleShape m_selected;

        public:
            Line(const std::string& content);

            void
            insertChar(const std::size_t,
                       const char);

            void
            insertString(const std::size_t,
                         const std::string&);

            void
            eraseChar(const std::size_t);

            void
            combine(Line&&);

            void
            setIndex(const std::size_t);

            void
            unselect();

            void
            select(const std::size_t start,
                   const std::size_t n);

            std::string const&
            content()
            const;

            std::string
            substr(const std::size_t,
                   const std::size_t,
                   const bool earse = false);

            bool
            empty()
            const noexcept;

            std::size_t
            size()
            const noexcept;

        protected:
            void
            draw(sf::RenderTarget&,
                 sf::RenderStates)
            const override;
        };

        class Cursor : public sf::Drawable
        {
        public: static sf::Color s_color;

        private: sf::RectangleShape m_shape;
                 float              m_char_width;
                 float              m_char_height;
                 Position           m_position;

        public:
            Cursor(const sf::Vector2f& size);

            Position
            getPosition()
            const noexcept;

            void
            setPosition(const std::size_t,
                        const std::size_t);

            sf::Vector2f
            getSize()
            const noexcept;

            void
            setSize(const sf::Vector2f&);

        protected:
            void
            draw(sf::RenderTarget&,
                 sf::RenderStates)
                const override;
        };

        class Mode
        {
        public:
            enum Type
            {
                Insert,
                Normal,
                Visual,
            };

            friend std::ostream&
            operator<<(std::ostream& os,
                       const Mode::Type& type)
            {
                using Mode = tedit::Editor::Mode;

                switch (type)
                {
                case Mode::Insert:
                {
                    os << "Insert";
                }
                break;
                case Mode::Normal:
                {
                    os << "Normal";
                }
                break;
                case Mode::Visual:
                {
                    os << "Visual";
                }
                break;
                }
                return os;
            }
        };

    private:
        static Size     s_default_size;
        static Position s_default_position;
        static Position s_default_cursor_position;
        static Font     s_default_font;
        static Color    s_default_background_color;

    private:
        sf::Vector2f       m_size;
        sf::RectangleShape m_shape;
        Cursor             m_cursor;
        Mode::Type         m_current_mode;

        std::vector<std::shared_ptr<Line>> m_lines;

        Position m_selected_start;
        Position m_selected_end;

        std::unique_ptr<std::fstream> m_file;
        std::optional<std::string>    m_filename;
        bool                          m_saved;

        std::string m_clipboard;

        Scroller    m_vscroller;
        std::size_t m_vscrolled;

        Scroller    m_hscroller;
        std::size_t m_hscrolled;
        std::size_t m_hmax;

    public:
        Editor(const std::size_t width = s_default_size.width,
               const std::size_t height = s_default_size.height);

        Editor(const std::vector<std::shared_ptr<Line>>&,
               const std::size_t width = s_default_size.width,
               const std::size_t height = s_default_size.height);

        Editor(std::vector<std::shared_ptr<Line>>&&,
               const std::size_t width = s_default_size.width,
               const std::size_t height = s_default_size.height);

        ~Editor();

        std::shared_ptr<Line>&
        at(const std::size_t);

        std::shared_ptr<Line>&
        operator[](const std::size_t);

        void
        insertLine(const std::size_t,
                   Line*);

        void
        eraseLine(const std::size_t);

        void
        write(const char);

        Mode::Type
        getCurrentMode()
        const noexcept;

        void
        setCurrentMode(const Mode::Type);

        sf::Vector2f
        getSize()
        const noexcept;

        void
        setSize(const std::size_t,
                const std::size_t);

        std::size_t
        getLinesCount()
        const noexcept;

        void
        move(const Direction);

        void
        handleEvent(const sf::Event);

        bool
        isSaved()
        const noexcept;

    private:
        void
        handleSelect();

        void
        handleKeyPress(const sf::Event::KeyEvent);

        void
        deleteForward(std::shared_ptr<Line>& current_line,
                       Position& cursor_position);

        void
        deleteBackward(std::shared_ptr<Line>& current_line,
                       Position& cursor_position);

        void
        insertNewLine(std::shared_ptr<Line>& current_line,
                      Position& cursor_position);

        void
        insertTab(std::shared_ptr<Line>& current_line,
                      Position& cursor_position);

        void
        copy(bool erase = false);

        void
        paste();

        void
        save();

        void
        open();

        void
        handleMouseScrolling(const int,
                             const int);

        void
        scrollToCursor();

        void
        resizeScroller();

    public:
        static void
        setFont(const std::string& font_path);

    protected:
        void
        draw(sf::RenderTarget&,
             sf::RenderStates)
        const override;
    };
}

#endif // TEDIT_EDITOR_HPP