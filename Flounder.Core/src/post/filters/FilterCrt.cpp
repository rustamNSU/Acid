#include "filtercrt.hpp"

namespace Flounder
{
	filtercrt::filtercrt(const Colour &screenColour, const float &curveAmountX, const float &curveAmountY, const float &scanLineSize, const float &scanIntensity) :
		ipostfilter("filterCrt", "res/shaders/filters/crt.frag.spv"),
		m_screenColour(new Colour(screenColour)),
		m_curveAmountX(curveAmountX),
		m_curveAmountY(curveAmountY),
		m_scanLineSize(scanLineSize),
		m_scanIntensity(scanIntensity)
	{
	}

	filtercrt::~filtercrt()
	{
		delete m_screenColour;
	}

	void filtercrt::storeValues()
	{
#if 0
		m_shader->loadUniform3f("screenColour", *m_screenColour);
		m_shader->loadUniform1f("curveAmountX", m_curveAmountX * static_cast<float>(Display::Get()->getAspectRatio()));
		m_shader->loadUniform1f("curveAmountY", m_curveAmountY);
		m_shader->loadUniform1f("scanLineSize", m_scanLineSize);
		m_shader->loadUniform1f("scanIntensity", m_scanIntensity);

		m_shader->loadUniform1f("moveTime", static_cast<float>(Engine::Get()->getTimeSec()) / 100.0f);
#endif
	}
}