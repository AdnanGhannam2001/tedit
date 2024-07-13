#include "includes/Editor.hpp"

#pragma region tedit::Editor::Position
std::pair<tedit::Editor::Position, tedit::Editor::Position>
tedit::Editor::Position::minmax(const Position& a, const Position& b)
{
    return
    {
        (a.row < b.row ? a : b),
        (a.row < b.row ? b : a)
    };
}
#pragma endregion // tedit::Editor::Position

#pragma region tedit::Editor::Line
tedit::Editor::Line::Line(const std::string& content)
    : m_content(content),
      m_sf_text(content, Editor::s_default_font.font, Editor::s_default_font.size)
{
    m_selected.setFillColor(sf::Color(120, 120, 120, 200));
}

void
tedit::Editor::Line::insertChar(const std::size_t index, const char c)
{
    m_content.insert(m_content.begin() + index, c);
    m_sf_text.setString(m_content);
}

void
tedit::Editor::Line::insertString(const std::size_t index, const std::string& str)
{
    for (auto s = str.rbegin(); s < str.rend(); s++)
    {
        m_content.insert(m_content.begin() + index, *s);
    }
    m_sf_text.setString(m_content);
}

void
tedit::Editor::Line::eraseChar(const std::size_t index)
{
    m_content.erase(m_content.begin() + index - 1);
    m_sf_text.setString(m_content);
}

void
tedit::Editor::Line::combine(Line&& line)
{
    m_content += line.m_content;
    m_sf_text.setString(m_content);
}

void
tedit::Editor::Line::setIndex(const std::size_t index)
{
    m_index = index;
    m_shape.setPosition(0, Editor::s_default_font.size * m_index);
    m_sf_text.setPosition(0, Editor::s_default_font.size * m_index);
}

void
tedit::Editor::Line::select(const std::size_t start, const std::size_t length)
{
    auto char_width = s_default_font.glyph;
    m_selected.setPosition(char_width * start, Editor::s_default_font.size * m_index);
    m_selected.setSize(
        sf::Vector2f(char_width * (length - start),
            s_default_font.size * 1.1));
}

std::string const&
tedit::Editor::Line::content()
const
{
    return m_content;
}

void
tedit::Editor::Line::unselect()
{
    m_selected.setSize(sf::Vector2f(0, 0));
}

std::string
tedit::Editor::Line::substr(const std::size_t start, const std::size_t length, const bool erase)
{
    std::string s = m_content.substr(start, length);
    if (erase) m_content.erase(start, length);
    m_sf_text.setString(m_content);

    return s;
}

bool
tedit::Editor::Line::empty()
const noexcept
{
    return m_content.empty();
}

std::size_t
tedit::Editor::Line::size()
const noexcept
{
    return m_content.size();
}

void
tedit::Editor::Line::draw(sf::RenderTarget& target, sf::RenderStates states)
const
{
    target.draw(m_shape, states);
    target.draw(m_selected, states);
    target.draw(m_sf_text, states);
}
#pragma endregion // tedit::Editor::Line

#pragma region tedit::Editor::Cursor
sf::Color tedit::Editor::Cursor::s_color = sf::Color(175, 175, 175);

tedit::Editor::Cursor::Cursor(const sf::Vector2f& size)
    : m_position({ .row = 0, .column = 0 })
{
    setSize(size);
    m_shape.setFillColor(s_color);
}

tedit::Editor::Position
tedit::Editor::Cursor::getPosition()
const noexcept
{
    return m_position;
}

void
tedit::Editor::Cursor::setPosition(const std::size_t column, const std::size_t row)
{
    float x = column * m_char_width;
    float y = row * m_char_height;
    m_shape.setPosition(column * m_char_width, row * m_char_height);

    m_position =
    {
        .row    = static_cast<std::size_t>(y / m_char_height),
        .column = static_cast<std::size_t>(x / m_char_width),
    };
}

sf::Vector2f
tedit::Editor::Cursor::getSize()
const noexcept
{
    return m_shape.getSize();
}

void
tedit::Editor::Cursor::setSize(const sf::Vector2f& size)
{
    m_shape.setSize(size);
    m_char_width = Editor::s_default_font.glyph;
    m_char_height = getSize().y;
}

void
tedit::Editor::Cursor::draw(sf::RenderTarget & target, sf::RenderStates states)
const
{
    target.draw(m_shape, states);
}
#pragma endregion // tedit::Editor::Cursor

#pragma region tedit::Editor
tedit::Editor::Size     tedit::Editor::s_default_size             = { .width = 900, .height = 500 };
tedit::Editor::Position tedit::Editor::s_default_position         = { .row = 0, .column = 0 };
tedit::Editor::Position tedit::Editor::s_default_cursor_position  = { .row = 0, .column = 0 };
tedit::Editor::Font     tedit::Editor::s_default_font             =
    {
        .font   = sf::Font(),
        .family = "assets/monospace.ttf",
        .size   = 30,
        .glyph  = 0.0f,
        .bold   = false,
    };
tedit::Editor::Color    tedit::Editor::s_default_background_color = { .red = 31, .green = 31, .blue = 31, .alpha = 255 };

tedit::Editor::Editor(const std::size_t width, const std::size_t height)
    : m_size(sf::Vector2f(width, height)),
      m_shape(m_size),
      m_cursor(sf::Vector2f(2, s_default_font.size)),
      m_current_mode(Mode::Insert),
      m_saved(false),
      m_vscroller(Scroller::Vertical, height),
      m_vscrolled(0),
      m_hscroller(Scroller::Horizontal, width),
      m_hscrolled(0),
      m_hmax(0)
{
    m_shape.setFillColor(
        sf::Color(s_default_background_color.red,
            s_default_background_color.green,
            s_default_background_color.blue,
            s_default_background_color.alpha));
    m_vscroller.setPosition(width - TEDIT_SCROLL_SIZE, 0);
    m_hscroller.setPosition(0 , height - TEDIT_SCROLL_SIZE);

    m_lines.push_back(std::shared_ptr<Line>(new Line("")));
}

tedit::Editor::Editor(const std::vector<std::shared_ptr<Line>>& lines, const std::size_t width, const std::size_t height)
    : Editor(width, height)
{
    m_lines = lines;
}

tedit::Editor::Editor(std::vector<std::shared_ptr<Line>>&& lines, const std::size_t width, const std::size_t height)
    : Editor(width, height)
{
    m_lines = std::move(lines);
}

tedit::Editor::Editor::~Editor()
{
    if (m_file)
    {
        m_file->close();
    }
}

void
tedit::Editor::draw(sf::RenderTarget& target, sf::RenderStates states)
const
{
    target.draw(m_shape, states);

    std::size_t index = 0;
    sf::Transform old = states.transform;
    states.transform *= sf::Transform(
                                    1, 0, -1.0f * m_hscrolled,
                                    0, 1, -1.0f * m_vscrolled,
                                    0, 0, 1);
    for (auto& line : m_lines)
    {
        line->setIndex(index++);
        target.draw(*line, states);
    }

    target.draw(m_cursor, states);
    states.transform = old;

    target.draw(m_vscroller, states);
    target.draw(m_hscroller, states);
}

std::shared_ptr<tedit::Editor::Line>&
tedit::Editor::at(const std::size_t index)
{
    return m_lines.at(index);
}

std::shared_ptr<tedit::Editor::Line>&
tedit::Editor::operator[](const std::size_t index)
{
    return m_lines[index];
}

void
tedit::Editor::insertLine(const std::size_t index, tedit::Editor::Line* line)
{
    m_lines.insert(m_lines.begin() + index, std::shared_ptr<Line>(line));
}

void
tedit::Editor::eraseLine(const std::size_t index)
{
    m_lines.erase(m_lines.begin() + index);
}

void
tedit::Editor::write(const char c)
{
    Position cursor_position = m_cursor.getPosition();
    std::shared_ptr<Line> line = m_lines[cursor_position.row];

    switch (c)
    {
    case '\x0d':
    case '\n':
        {
            insertNewLine(line, cursor_position);
        }
        break;
    case '\b':
        {
            deleteBackward(line, cursor_position);
        }
        break;
    case '\x7f': // DEL
        {
            deleteForward(line, cursor_position);
        }
        break;
    case '\t':
        {
            insertTab(line, cursor_position);
        }
        break;
    default:
        line->insertChar(cursor_position.column++, c);
    }

    m_hmax = 0;
    for (auto const &line : m_lines)
    {
        if (line->size() > m_hmax)
        {
            m_hmax = line->size();
        }
    }

    m_saved = false;
    m_cursor.setPosition(cursor_position.column, cursor_position.row);
    resizeScroller();
    scrollToCursor();
}

tedit::Editor::Mode::Type
tedit::Editor::getCurrentMode()
const noexcept
{
    return m_current_mode;
}

void
tedit::Editor::setCurrentMode(const tedit::Editor::Mode::Type type)
{
    if (m_current_mode == Mode::Visual)
    {
        std::size_t max = std::max(m_selected_start.row, m_selected_end.row);
        for (std::size_t i = std::min(m_selected_start.row, m_selected_end.row); i <= max; i++)
        {
            m_lines[i]->unselect();
        }
    }

    m_current_mode = type;

    if (type == Mode::Visual)
    {
        m_selected_start = m_cursor.getPosition();
        m_selected_end = m_cursor.getPosition();
    }
}

sf::Vector2f
tedit::Editor::getSize()
const noexcept
{
    return m_shape.getSize();
}

void
tedit::Editor::setSize(const std::size_t width, const std::size_t height)
{
    m_size = sf::Vector2f(width, height);
    m_shape.setSize(sf::Vector2f(width, height));
    m_vscroller.setX(width - TEDIT_SCROLL_SIZE);
    m_hscroller.setY(height - TEDIT_SCROLL_SIZE);
    resizeScroller();
}

std::size_t
tedit::Editor::getLinesCount()
const noexcept
{
    return m_lines.size();
}

void
tedit::Editor::move(const Direction direction)
{
    Position cursor_position = m_cursor.getPosition();

    switch (direction)
    {
    case Direction::Begin:
        {
            cursor_position.column = 0;
        }
        break;
    case Direction::End:
        {
            cursor_position.column = m_lines[cursor_position.row]->size();
        }
        break;
    case Direction::Up:
        {
            if (cursor_position.row > 0)
            {
                std::size_t prev_size = m_lines[--cursor_position.row]->size();
                cursor_position.column = std::min(cursor_position.column, prev_size);
            }
        }
        break;
    case Direction::Down:
        {
            if (cursor_position.row < m_lines.size() - 1)
            {
                std::size_t next_size = m_lines[++cursor_position.row]->size();
                cursor_position.column = std::min(cursor_position.column, next_size);
            }
        }
        break;
    case Direction::Right:
        {
            if (cursor_position.column < m_lines[cursor_position.row]->size())
            {
                cursor_position.column++;
            }
        }
        break;
    case Direction::Left:
        {
            if (cursor_position.column > 0)
            {
                cursor_position.column--;
            }
        }
        break;
    default: {}
    }

    m_cursor.setPosition(cursor_position.column, cursor_position.row);
    scrollToCursor();

    if (m_current_mode == Mode::Visual)
    {
        handleSelect();
    }
}

void
tedit::Editor::handleSelect()
{
    m_selected_end = m_cursor.getPosition();

    auto [min, max] = Position::minmax(m_selected_start, m_selected_end);

    for (std::size_t i = min.row + 1; i < max.row; ++i)
    {
        m_lines[i]->select(0, m_lines[i]->size());
    }

    if (min.row > 0 && min.row - 1 > 0)
        m_lines[min.row - 1]->unselect();
    if (max.row + 1 < m_lines.size())
        m_lines[max.row + 1]->unselect();

    if (min.row != max.row)
    {
        m_lines[min.row]->select(min.column, m_lines[min.row]->size());
        m_lines[max.row]->select(0, max.column);
    }
    else
    {
        std::size_t min_column = std::min(m_selected_start.column, m_selected_end.column);
        std::size_t max_column = std::max(m_selected_start.column, m_selected_end.column);
        m_lines[max.row]->select(min_column, max_column);
    }
}

void
tedit::Editor::handleEvent(const sf::Event event)
{
    switch (event.type)
    {
    case sf::Event::EventType::TextEntered:
        {
            if (getCurrentMode() == tedit::Editor::Mode::Insert)
            {
                write(static_cast<char>(event.text.unicode));
            }
        }
        break;
    case sf::Event::Event::KeyPressed:
        {
            handleKeyPress(event.key);
        }
        break;
    case sf::Event::EventType::KeyReleased:
        {
            if (!event.key.control && getCurrentMode() != tedit::Editor::Mode::Visual)
            {
                setCurrentMode(tedit::Editor::Mode::Insert);
            }
        }
        break;
    case sf::Event::EventType::MouseButtonPressed:
        {
            m_vscroller.startScrolling(event.mouseButton);
            m_hscroller.startScrolling(event.mouseButton);
        }
        break;
    case sf::Event::EventType::MouseButtonReleased:
        {
            m_vscroller.endScrolling(event.mouseButton);
            m_hscroller.endScrolling(event.mouseButton);
        }
        break;
    case sf::Event::EventType::MouseMoved:
        {
            handleMouseScrolling(event.mouseMove.x, event.mouseMove.y);
        }
        break;
    default: {}
    }
}
bool
tedit::Editor::isSaved()
const noexcept
{
    return m_saved;
}

void
tedit::Editor::handleKeyPress(const sf::Event::KeyEvent key)
{
    if (!key.control && getCurrentMode() != tedit::Editor::Mode::Visual)
    {
        setCurrentMode(tedit::Editor::Mode::Insert);
    }
    else
    {
        if (getCurrentMode() != tedit::Editor::Mode::Visual)
        {
            setCurrentMode(tedit::Editor::Mode::Normal);
        }

        Position cursor_position = m_cursor.getPosition();
        std::shared_ptr<Line> line = m_lines[cursor_position.row];

        switch (key.code)
        {
        case sf::Keyboard::A:
            {
                move(tedit::Editor::Direction::Begin);
            }
            break;
        case sf::Keyboard::E:
            {
                move(tedit::Editor::Direction::End);
            }
            break;
        case sf::Keyboard::P:
            {
                move(tedit::Editor::Direction::Up);
            }
            break;
        case sf::Keyboard::N:
            {
                move(tedit::Editor::Direction::Down);
            }
            break;
        case sf::Keyboard::B:
            {
                move(tedit::Editor::Direction::Left);
            }
            break;
        case sf::Keyboard::F:
            {
                move(tedit::Editor::Direction::Right);
            }
            break;
        case sf::Keyboard::D:
            {
                deleteForward(line, cursor_position);
                m_cursor.setPosition(cursor_position.column, cursor_position.row);
            }
            break;
        case sf::Keyboard::H:
            {
                deleteBackward(line, cursor_position);
                m_cursor.setPosition(cursor_position.column, cursor_position.row);
            }
            break;
        case sf::Keyboard::Space:
            {
                if (getCurrentMode() != tedit::Editor::Mode::Visual)
                {
                    setCurrentMode(tedit::Editor::Mode::Visual);
                }
                else if (getCurrentMode() == tedit::Editor::Mode::Visual)
                {
                    setCurrentMode(tedit::Editor::Mode::Normal);
                }
            }
            break;
        case sf::Keyboard::Y:
            {
                paste();
            }
            break;
        case sf::Keyboard::W:
            {
                if (getCurrentMode() == tedit::Editor::Mode::Visual)
                {
                    copy();
                    setCurrentMode(tedit::Editor::Mode::Normal);
                }
            }
            break;
        case sf::Keyboard::X:
            {
                if (getCurrentMode() == tedit::Editor::Mode::Visual)
                {
                    copy(true);
                    setCurrentMode(tedit::Editor::Mode::Normal);
                }
            }
            break;
        case sf::Keyboard::S:
            {
                save();
            }
            break;
        case sf::Keyboard::O:
            {
                open();
            }
            break;
        default: {}
        }

        resizeScroller();
        scrollToCursor();
    }
}

void
tedit::Editor::deleteForward(std::shared_ptr<Line>& current_line, Position& cursor_position)
{
    if (cursor_position.column < current_line->size())
    {
        current_line->eraseChar(cursor_position.column + 1);
    }
    else if (cursor_position.row < m_lines.size() - 1)
    {
        current_line->combine(std::move(*m_lines[cursor_position.row + 1]));
        m_lines.erase(m_lines.begin() + cursor_position.row + 1);
    }
}

void
tedit::Editor::deleteBackward(std::shared_ptr<Line>& current_line, Position& cursor_position)
{
    if (cursor_position.row != 0 && !cursor_position.column)
    {
        if (current_line->empty())
        {
            m_lines.erase(m_lines.begin() + cursor_position.row--);
            cursor_position.column = m_lines[cursor_position.row]->size();
        }
        else
        {
            std::shared_ptr<Line> prev = m_lines[cursor_position.row - 1];
            cursor_position.column = prev->size();
            prev->combine(std::move(*current_line));
            m_lines.erase(m_lines.begin() + cursor_position.row--);
        }
    }
    else if (!current_line->empty() && cursor_position.column)
    {
        current_line->eraseChar(cursor_position.column--);
    }
}

void
tedit::Editor::insertNewLine(std::shared_ptr<Line>& current_line, Position& cursor_position)
{
    std::shared_ptr<Line> newline(new Line(current_line->substr(cursor_position.column, current_line->size(), true)));
    m_lines.insert(m_lines.begin() + ++cursor_position.row, std::move(newline));
    cursor_position.column = 0;
}

void
tedit::Editor::insertTab(std::shared_ptr<Line>& current_line, Position& cursor_position)
{
    current_line->insertString(cursor_position.column, "    ");
    cursor_position.column += 4;
}

void
tedit::Editor::copy(bool erase)
{
    auto [min, max] = Position::minmax(m_selected_start, m_selected_end);

    m_clipboard = "";

    if (min.row != max.row)
    {
        for (std::size_t i = min.row; i <= max.row; ++i)
        {
            m_clipboard += m_lines[i]->substr((i == min.row ? min.column : 0), (i == max.row ? max.column : m_lines[i]->size()), erase);
            if (i != max.row) m_clipboard += '\n';
        }

        if (erase)
        {
            std::shared_ptr<Line> last = m_lines[max.row];

            for (std::size_t i = min.row + 1; i < max.row; ++i)
            {
                m_lines.erase(m_lines.begin() + i);
            }

            m_lines[min.row]->combine(std::move(*last));
            m_lines.erase(m_lines.begin() + max.row);
            m_selected_end = m_selected_start = min;
        }
    }
    else
    {
        std::size_t min_column = std::min(m_selected_start.column, m_selected_end.column);
        std::size_t max_column = std::max(m_selected_start.column, m_selected_end.column);
        m_clipboard += m_lines[min.row]->substr(min_column, max_column - min_column, erase);
    }

    m_cursor.setPosition(std::min(min.column, max.column), min.row);
    scrollToCursor();
}

void
tedit::Editor::paste()
{
    for (auto const& c : m_clipboard)
    {
        write(c);
    }
}

void
tedit::Editor::save()
{
    if (m_saved) return;

    if (!m_filename)
    {
        system("zenity --file-selection --save --confirm-overwrite > temp");

        std::ifstream file("temp");
        m_filename = "";
        std::getline(file, *m_filename);
        file.close();
        remove("temp");

        if (std::empty(*m_filename))
        {
            m_filename = std::nullopt;
            return;
        }

        m_file = std::make_unique<std::fstream>(std::fstream());
    }

    std::string strcontent = std::accumulate(m_lines.begin(), m_lines.end(),
        std::string(),
        [](std::string& acc, const std::shared_ptr<Line>& line) -> std::string
        {
            return acc + line->content() + "\n";
        });
    
    m_file->open(m_filename.value(), std::ios::out | std::ios::trunc);
    *m_file << strcontent;
    m_file->close();
    m_saved = true;
}

void
tedit::Editor::open()
{
    char filename[1024];
    FILE *f = popen("zenity --file-selection", "r");
    fgets(filename, 1024, f);
    
    if (filename[0] != '\0' && filename[1] != '\0')
    {
        std::size_t size = strlen(filename);

        if (filename[size - 1] == '\n') filename[size - 1] = '\0';

        m_file = std::make_unique<std::fstream>(std::fstream(filename, std::ios::in));

        if (!m_file->fail())
        {
            m_filename = filename;
            m_lines.clear();
            m_cursor.setPosition(0, 0);

            m_lines.push_back(std::shared_ptr<Line>(new Line("")));
            for (std::string line; std::getline(*m_file, line);)
            {
                for (std::size_t i = 0; i < line.size(); ++i)
                {
                    write(line[i]);
                }
                
                write('\n');
            }

            m_cursor.setPosition(0, 0);
        }
        
        m_file->close();
    }
}

void
tedit::Editor::handleMouseScrolling(const int mouseX, const int mouseY)
{
    // Vertical Scrolling
    {
        auto scrolled = m_vscroller.mouseScroll(mouseX, mouseY);
        if (scrolled)
        {
            std::size_t total = (m_lines.size() * s_default_font.size) - m_shape.getSize().y + (TEDIT_SCROLL_SIZE * 2);
            m_vscrolled = scrolled.value() * total / 100;
        }
    }

    // Horizontal Scrolling
    {
        auto scrolled = m_hscroller.mouseScroll(mouseX, mouseY);
        if (scrolled)
        {
            std::size_t total = (m_hmax * s_default_font.glyph) - m_shape.getSize().x + (TEDIT_SCROLL_SIZE * 2);
            m_hscrolled = scrolled.value() * total / 100;
        }
    }
}

void
tedit::Editor::scrollToCursor()
{
    auto position = m_cursor.getPosition();

    // Vertical Scrolling
    {
        if (((position.row) * s_default_font.size) < m_vscrolled)
        {
            m_vscrolled = (position.row) * s_default_font.size + (TEDIT_SCROLL_SIZE * 2);
        }
        else if (((position.row + 1) * s_default_font.size) > m_vscrolled + m_size.y)
        {
            m_vscrolled = (position.row + 1 - ((float)m_size.y / s_default_font.size))
                            * s_default_font.size
                            + (TEDIT_SCROLL_SIZE * 2);
        }

        m_vscroller.scrollTo(position.row * 100 / (getLinesCount() + 1));
    }

    // Horizontal Scrolling
    {
        auto char_width = s_default_font.glyph;
        if (((position.column) * char_width) < m_hscrolled)
        {
            m_hscrolled = (position.column) * char_width;
        }
        else if (((position.column + 1) * char_width) > m_hscrolled + m_size.x)
        {
            m_hscrolled = (position.column + 1 - ((float)m_size.x / char_width)) * char_width;
        }

        m_hscroller.scrollTo(position.column * 100 / (m_hmax + 1));
    }
}

void
tedit::Editor::resizeScroller()
{
    auto size = getSize();

    // Vertical Scrolling
    {
        auto scroll_size = (static_cast<float>(size.y) / s_default_font.size) / m_lines.size();

        if (scroll_size < 1)
        {
            m_vscroller.setSize(size.y, TEDIT_SCROLL_SIZE, scroll_size * size.y);
        }
        else
        {
            m_vscroller.setSize(size.y, 0, 0);
        }
    }

    // Horizontal Scrolling
    {
        auto scroll_size = (static_cast<float>(size.x) / s_default_font.glyph) / m_hmax;

        if (scroll_size < 1)
        {
            m_hscroller.setSize(size.x, scroll_size * size.x, TEDIT_SCROLL_SIZE);
        }
        else
        {
            m_hscroller.setSize(size.x, 0, 0);
        }
    }

    scrollToCursor();
}

void
tedit::Editor::setFont(const std::string& font_path)
{
    s_default_font.font.loadFromFile(font_path);
    s_default_font.glyph = s_default_font.font.getGlyph(' ', s_default_font.size, s_default_font.bold).advance;
}
#pragma endregion // tedit::Editor