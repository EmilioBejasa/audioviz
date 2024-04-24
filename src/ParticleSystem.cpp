#include "ParticleSystem.hpp"
#include <random>

template <typename _Tp>
static _Tp random(const _Tp min, const _Tp max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return std::uniform_real_distribution<>(min, max)(gen);
}

ParticleSystem::ParticleSystem(sf::Vector2u target_size, unsigned particle_count, bool debug)
	: target_size(target_size),
	  particles(particle_count),
	  max_height(target_size.y * 0.15),
	  debug(debug)
{
	for (auto &p : particles)
	{
		p.setRadius(random<float>(2, 5));
		p.setVelocity({random<float>(-0.5, 0.5), random<float>(0, -2)});
		// start some particles offscreen, so the beginning sequence feels less "sudden"
		// otherwise all of them come out at once and it looks bad
		p.setPosition({random<float>(0, target_size.x), target_size.y * random<float>(1, 1.5)});
	}

	if (debug)
	{
		particle_to_trace_idx = random(0u, particle_count);
		particles[particle_to_trace_idx].setFillColor(sf::Color::Red);
		max_height_line[0] = {{0, max_height}};
		max_height_line[1] = {{target_size.x, max_height}};
	}
}

void ParticleSystem::set_max_height(unsigned max_height)
{
	this->max_height = max_height;
}

void ParticleSystem::draw(sf::RenderTarget &target, sf::Vector2f additional_displacement)
{
	assert(target.getSize() == target_size);

	for (auto &p : particles)
	{
		p.move();
		p.setPosition(p.getPosition() + additional_displacement);

		const auto dist_to_max_height = p.getPosition().y - max_height;

		// sqrt is very nice, but you can lower the root further
		// a root of 1.5 would be nicer: pow(x, 1 / 1.5)
		const auto alpha_scale = sqrtf(dist_to_max_height / (target_size.y - max_height));

		// decrease alpha with distance to max_height
		const auto [r, g, b, _] = p.getFillColor();
		p.setFillColor({r, g, b, alpha_scale * 255});

		// reset position to bottom of target once max_height is reached
		if (dist_to_max_height <= 0)
			p.setPosition({random<float>(0, target_size.x), target_size.y});

		target.draw(p);
	}

	if (debug)
		target.draw(max_height_line);
}