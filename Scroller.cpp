#include "includes/Scroller.hpp"

tedit::Scroller::Scroller::Scroller(const Direction& direction, const size_t& max_size)
    : m_direction(direction),
      m_max_size(max_size)
{
    setFillColor(sf::Color(241, 241, 241));
}

void
tedit::Scroller::startScrolling(const sf::Event::MouseButtonEvent& event)
{
    if (event.button == sf::Mouse::Left
        && m_shape.getGlobalBounds().contains(sf::Vector2f(event.x, event.y)))
    {
        m_hold = m_direction == Direction::Vertical ? event.y : event.x;
        auto position = m_shape.getPosition();
        m_release = m_direction == Direction::Vertical ? position.y : position.x;
    }
}

void
tedit::Scroller::endScrolling(const sf::Event::MouseButtonEvent& event)
{
    m_release = m_direction == Direction::Vertical ? event.y : event.x;
    m_hold = std::nullopt;
}

std::optional<std::size_t>
tedit::Scroller::mouseScroll(const int mouseX, const int mouseY)
{
    if (!m_hold) return std::nullopt;

    if (m_direction == Direction::Vertical)
    {
        int diff = mouseY - m_hold.value();
        setPosition(m_shape.getPosition().x, m_release + diff);
    }
    else
    {
        int diff = mouseX - m_hold.value();
        setPosition(m_release + diff, m_shape.getPosition().y);
    }

    std::size_t total = m_max_size - getSize();
    std::size_t scrolled = total - (total - getPosition());

    return scrolled * 100 / total;
}

void
tedit::Scroller::scrollTo(const int prec)
{
    std::size_t total = m_max_size - getSize();

    int value = total * prec / 100;

    if (m_direction == Direction::Vertical)
    {
        setPosition(m_shape.getPosition().x, value);
    }
    else
    {
        setPosition(value, m_shape.getPosition().y);
    }
}

void
tedit::Scroller::draw(sf::RenderTarget& target, sf::RenderStates states)
const
{
    target.draw(m_shape, states);
}

float
tedit::Scroller::getPosition()
const noexcept
{
    return m_direction == Direction::Vertical
        ? m_shape.getPosition().y
        : m_shape.getPosition().x;
}

void
tedit::Scroller::setPosition(const float x, const float y)
{
    auto current_size = m_shape.getSize();
    
    if (m_direction == Direction::Vertical)
    {
        m_shape.setPosition(x, std::clamp(y, 0.0f, m_max_size - current_size.y));
    }
    else if (m_direction == Direction::Horizontal)
    {
        m_shape.setPosition(std::clamp(x, 0.0f, m_max_size - current_size.x), y);
    }
}

void
tedit::Scroller::setX(const float x)
{
    m_shape.setPosition(x, m_shape.getPosition().y);
}

void
tedit::Scroller::setY(const float y)
{
    m_shape.setPosition(m_shape.getPosition().x, y);
}

std::size_t
tedit::Scroller::getSize()
const noexcept
{
    return m_direction == Direction::Vertical
        ? m_shape.getSize().y
        : m_shape.getSize().x;
}

void
tedit::Scroller::setSize(const std::size_t& max_size, const std::size_t& width, const std::size_t& height)
{
    m_max_size = max_size;
    m_shape.setSize(sf::Vector2f(width, height));
}

void
tedit::Scroller::setFillColor(const sf::Color& color)
{
    m_shape.setFillColor(color);
}