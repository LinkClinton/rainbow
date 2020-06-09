#pragma once

#include "../distribution.hpp"

#include <algorithm>

namespace rainbow::cpus::shared::distributions {

	inline distribution_sample_t<1>::distribution_sample_t(size_t offset, real value, real pdf) :
		offset(offset), value(value), pdf(pdf)
	{
	}

	inline distribution_sample_t<2>::distribution_sample_t(const vector2& value, real pdf) :
		value(value), pdf(pdf)
	{
	}

	inline distribution_t<1>::distribution_t(const std::vector<real>& value) :
		mValues(value), mCDFs(value.size() + 1, 0)
	{
		const auto n = mValues.size();
		// value is the function with domain of definition [0, 1], we divide the function into n-parts
		// value must not be negative.
		// each part i has 1 / n length with value[i]
		// the the CDF = integral p(x)dx from x0 to xn
		// p(x) = f(x) / c, c = integral f(x)dx from 0 to 1

		// we compute the integral f(x)dx from x0 to xn = sigma f(x) / n
		// so the mCDFs[n] is c = integral f(x)dx from x0 to xn
		for (size_t index = 1; index < mCDFs.size(); index++) {
			assert(mValues[index - 1] >= 0);

			mCDFs[index] = mCDFs[index - 1] + mValues[index - 1] / n;
		}

		mIntegral = mCDFs[n];

		// compute the CDF = integral p(x)dx from x0 to xn
		// mCDFs = integral f(x)dx from x0 to xn, so CDF[i] = mCDFs[i] / c
		for (size_t index = 1; index < mCDFs.size(); index++) {
			// if c = 0, we use 1 as the f(x) so c = 1, p(x) = 1, CDF[i] = i / n.
			if (mIntegral != 0) mCDFs[index] = mCDFs[index] / mIntegral;
			else mCDFs[index] = static_cast<real>(index) / n;
		}
	}

	inline distribution_sample_t<1> distribution_t<1>::sample(const vector_t<1, real>& sample) const
	{
		const auto u = sample.x;
		const auto offset = find_interval(u);

		auto du = u - mCDFs[offset];

		// if the segment of CDFs is less than or equal to 0, we can not divide the segment
		if (offset < mCDFs.size() - 1 && mCDFs[offset + 1] - mCDFs[offset] > 0)
			du = du / (mCDFs[offset + 1] - mCDFs[offset]);

		// compute p(x) = f(x) / c
		const auto pdf = mIntegral > 0 ? mValues[offset] / mIntegral : 0;
		const auto value = (offset + du) / mValues.size();

		return distribution_sample_t<1>(offset, value, pdf);
	}

	inline distribution_sample_t<1> distribution_t<1>::sample_discrete(const vector_t<1, real>& sample) const
	{
		const auto offset = find_interval(sample.x);
		const auto pdf = (mIntegral > 0) ? mValues[offset] / (mIntegral * count()) : 0;

		return distribution_sample_t<1>(offset, 0, pdf);
	}

	inline real distribution_t<1>::integral() const noexcept
	{
		return mIntegral;
	}

	inline real distribution_t<1>::value(size_t index) const noexcept
	{
		return mValues[index];
	}

	inline size_t distribution_t<1>::count() const noexcept
	{
		return mValues.size();
	}

	inline size_t distribution_t<1>::find_interval(real x) const
	{
		assert(x >= 0);

		// because x >= 0 and mCDFs[0] = 0, the value upper_bound() - begin() >= 1
		return (std::upper_bound(mCDFs.begin(), mCDFs.end(), x) - mCDFs.begin()) - 1;
	}

	inline distribution_t<2>::distribution_t(const std::vector<real>& value, size_t width, size_t height) :
		mWidth(width), mHeight(height)
	{
		mCDFs = std::vector<std::shared_ptr<distribution_t<1>>>(height);

		auto integrals = std::vector<real>(height);

		for (size_t y = 0; y < height; y++) {
			// compute the row begin position and end position
			const auto row_begin = value.begin() + y * width;
			const auto row_end = value.begin() + (y + 1) * width;

			// mCDFs[y]->integral = integral f(x, y)dx
			mCDFs[y] = std::make_shared<distribution_t<1>>(std::vector<real>(row_begin, row_end));

			integrals[y] = mCDFs[y]->integral();
		}

		// mMDFs is the marginal density of f(y)
		// p(y) = integral p(x, y)dx = (1 / width * integral f(x, y)dx) / integral f(x, y)dx dy
		// integral f(x, y)dx dy = integral mCDFs[y]->integral dy
		mMDFs = std::make_shared<distribution_t<1>>(integrals);
	}

	inline distribution_sample_t<2> distribution_t<2>::sample(const vector_t<2, real>& sample) const
	{
		// first, we sample the marginal density to get which row we need sample
		// second, sample the row(sample1.offset) to get result
		const auto sample1 = mMDFs->sample(vector_t<1, real>(sample.y));
		const auto sample0 = mCDFs[sample1.offset]->sample(vector_t<1, real>(sample.x));

		// the p(x, y) = (p(x | y) = sample0.pdf) * (p(y) = sample1.pdf)
		return distribution_sample_t<2>(
			vector2(sample0.value, sample1.value),
			sample0.pdf * sample1.pdf);
	}

	inline real distribution_t<2>::pdf(const vector_t<2, real>& point) const
	{
		const auto x = math::clamp(static_cast<size_t>(point.x * mWidth), static_cast<size_t>(0), mWidth - 1);
		const auto y = math::clamp(static_cast<size_t>(point.y * mHeight), static_cast<size_t>(0), mHeight - 1);

		return mCDFs[y]->value(x) / mMDFs->integral();
	}

}