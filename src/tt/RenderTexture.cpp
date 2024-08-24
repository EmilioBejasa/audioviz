#include "tt/RenderTexture.hpp"

namespace tt
{

RenderTexture::RenderTexture(const sf::Vector2u size, const sf::ContextSettings &ctx)
	: sf::RenderTexture(size, ctx),
	  sprite(getTexture(), {{}, (sf::Vector2i)size})
{
}

RenderTexture::RenderTexture(const sf::Vector2u size, int antialiasing)
	: RenderTexture(size, sf::ContextSettings{0, 0, antialiasing})
{
}

// Copy the contents of `other` to this render-texture.
void RenderTexture::copy(const RenderTexture &other)
{
	draw(other.sprite);
	display();
}

} // namespace tt
