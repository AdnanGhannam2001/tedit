#ifndef TEDIT_SCROLLER_HPP
#define TEDIT_SCROLLER_HPP

#include <iostream>
#include <optional>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Window/Event.hpp>

namespace tedit
{
    class Scroller : public sf::Drawable
    {
    public:
        enum Direction
        {
            Vertical,
            Horizontal,
        };

    private:
        sf::RectangleShape m_shape;
        Direction          m_direction;
        size_t             m_max_size;

        std::optional<float> m_hold;
        float                m_release;

    public:
        Scroller(const Direction& , const size_t& max_size);

        void
        startScrolling(const sf::Event::MouseButtonEvent&);

        void
        endScrolling(const sf::Event::MouseButtonEvent&);

        std::optional<std::size_t>
        mouseScroll(const int,
                    const int);

        void
        scrollTo(const int);

        float
        getPosition()
        const noexcept;

        void
        setPosition(const float,
                    const float);

        void
        setX(const float);

        void
        setY(const float);

        std::size_t
        getSize()
        const noexcept;

        void
        setSize(const std::size_t& max_size,
                const std::size_t&,
                const std::size_t&);
        
        void
        setFillColor(const sf::Color&);
    protected:
        void
        draw(sf::RenderTarget&,
             sf::RenderStates)
        const override;
    };
}

#endif // TEDIT_SCROLLER_HPP