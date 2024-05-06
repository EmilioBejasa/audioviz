#pragma once

#include <optional>
#include <deque>
#include <list>

#include <SFML/Graphics.hpp>
#include <portaudio.hpp>
#include <av.hpp>

#include "StereoSpectrum.hpp"
#include "MyRenderTexture.hpp"
#include "ParticleSystem.hpp"

// will eventually create an `audioviz` namespace,
// move this class there and call it `stereo_spectrum`.
class audioviz
{
private:
	using SD = SpectrumDrawable;
	using FS = FrequencySpectrum;

	static inline const sf::Color zero_alpha{0, 0, 0, 0};

	struct EffectOptions
	{
		struct
		{
			float hrad = 10, vrad = 10;
			int n_passes = 20;
		} blur;
		float mult = 0;
	};

	const sf::Vector2u size;
	int sample_size = 3000;
	std::vector<float> audio_buffer;
	av::Frame rs_frame;

	av::MediaReader _format;

	av::Stream _astream = _format.find_best_stream(AVMEDIA_TYPE_AUDIO);
	av::Decoder _adecoder = _astream.create_decoder();
	av::Resampler _resampler = av::Resampler(
		&_astream->codecpar->ch_layout, AV_SAMPLE_FMT_FLT, _astream.sample_rate(),
		&_astream->codecpar->ch_layout, (AVSampleFormat)_astream->codecpar->format, _astream.sample_rate());

	std::optional<av::Stream> _vstream;
	std::optional<av::Decoder> _vdecoder;
	std::optional<av::Scaler> _scaler;
	std::optional<av::Frame> _scaled_frame;
	std::optional<std::list<sf::Texture>> _frame_queue;

	// framerate
	int framerate = 60;

	// audio frames per video frame
	int afpvf = _astream.sample_rate() / framerate;

	// stereo spectrum!
	StereoSpectrum ss = sample_size;

	// particle system
	ParticleSystem ps;

	// metadata-related fields
	sf::Font font;
	sf::Text title_text = sf::Text(font, ""), artist_text = sf::Text(font, "");
	struct _texture_sprite
	{
		sf::Texture texture;
		sf::Sprite sprite;
		_texture_sprite() : sprite(texture) {}
	} album_cover;

	// container to hold render-textures
	struct _rt
	{
		struct _blur
		{
			MyRenderTexture original, blurred;
			_blur(const sf::Vector2u size, const int antialiasing);
		} spectrum, particles;

		MyRenderTexture bg;
		_rt(const sf::Vector2u size, const int antialiasing);
	} rt;

	// clock to time the particle system
	sf::Clock ps_clock;

	std::optional<pa::PortAudio> pa_init;
	std::optional<pa::Stream> pa_stream;

public:
	/**
	 * @param size size of the output; recommended to match your `sf::RenderTarget`'s size
	 * @param media_url url to media source. must contain an audio stream
	 * @param antialiasing antialiasing level to use for round shapes
	 */
	audioviz(sf::Vector2u size, const std::string &media_url, int antialiasing = 4);

	/**
	 * @return whether the end of the audio buffer has been reached and another frame cannot be produced.
	 */
	bool draw_frame(sf::RenderTarget &target);

	/**
	 * @return the chunk of audio used to produce the last frame
	 */
	const std::span<float> &current_audio() const;

	// setters

	/**
	 * @param enabled whether to play the audio used to render the spectrum using PortAudio
	 */
	void set_audio_playback_enabled(bool enabled);

	/**
	 * important if you are capturing frames for video encoding!
	 * the framerate defaults to 60fps, so if you are on a high-refresh rate display,
	 * it may be setting a higher framerate.
	 */
	void set_framerate(int framerate);

	// set background image with optional effects: blur and color-multiply
	void set_background(const std::filesystem::path &image_path, EffectOptions options = {{10, 10, 25}, 0});
	void set_background(const sf::Texture &texture, EffectOptions options = {{10, 10, 25}, 0});

	// set margins around the output size for the spectrum to respect
	void set_margin(int margin);

	void set_title_text(const std::string &text);
	void set_artist_text(const std::string &text);
	void set_album_cover(const std::filesystem::path &image_path, sf::Vector2f scale_to = {150, 150});

	// you **must** call this method in order to see text metadata!
	void set_text_font(const std::filesystem::path &path);

	// set the top-left-most position for the metadata (album cover sprite, title/artist text) to be drawn from
	void set_metadata_position(const sf::Vector2f &pos);

	// passthrough setters

	void set_bar_width(int width);
	void set_bar_spacing(int spacing);
	void set_color_mode(SD::ColorMode mode);
	void set_solid_color(sf::Color color);
	void set_color_wheel_rate(float rate);
	void set_color_wheel_hsv(sf::Vector3f hsv);
	void set_multiplier(float multiplier);
	// void set_fft_size(int fft_size); // will handle this later
	void set_interp_type(FS::InterpolationType interp_type);
	void set_scale(FS::Scale scale);
	void set_nth_root(int nth_root);
	void set_accum_method(FS::AccumulationMethod method);
	void set_window_func(FS::WindowFunction wf);

private:
	// void decoder_thread_func();
	bool decoder_thread_finished();
	void set_text_defaults();
	void draw_spectrum();
	void draw_particles();
	void blur_spectrum();
	void blur_particles();
	void actually_draw_on_target(sf::RenderTarget &target);
	void _set_album_cover(sf::Vector2f scale_to = {150, 150});
	void decode_media();
	void play_audio();
};