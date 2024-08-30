#include "tt/AudioAnalyzer.hpp"

namespace tt
{

AudioAnalyzer::AudioAnalyzer(const int num_channels)
	: _num_channels(num_channels),
	  _spectrum_data_per_channel(num_channels)
{
}

void AudioAnalyzer::resize(const int size)
{
	for (int i = 0; i < _num_channels; ++i)
		_spectrum_data_per_channel[i].resize(size);
}

void AudioAnalyzer::analyze(tt::FrequencyAnalyzer &fa, const float *const audio, const bool interleaved)
{
	for (int i = 0; i < _num_channels; ++i)
	{
		fa.copy_channel_to_input(audio, _num_channels, i, interleaved);
		fa.render(_spectrum_data_per_channel[i]);
	}
}

int AudioAnalyzer::get_num_channels() const
{
	return _num_channels;
}

const std::vector<float> &AudioAnalyzer::get_spectrum_data(const int channel_index) const
{
	if (channel_index < 0 || channel_index >= _num_channels)
		throw std::invalid_argument("channel index out of bounds!");
	return _spectrum_data_per_channel[channel_index];
}

} // namespace tt
