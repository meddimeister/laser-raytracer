#pragma once

#include "glm.hpp"
#include "gtc/random.hpp"
#include <random>
#include <array>
#include <chrono>
#include "vecn.h"

using namespace std;
using namespace glm;

namespace RNG
{
	template <typename T>
	class Sampler
	{
	protected:
		unsigned int count;
		unsigned int idx;

	public:
		virtual void init(unsigned int count) = 0;
		virtual T next() = 0;
	};

	template <typename T>
	class UniformSampler : public Sampler<T>
	{
	protected:
		unsigned int seed;
		default_random_engine generator;
		uniform_real_distribution<float> uniformDistribution;

	public:
		void init(unsigned int _count)
		{
			this->count = _count;
			this->idx = 0;
			seed = chrono::system_clock::now().time_since_epoch().count();
			generator.seed(seed);
			uniformDistribution = uniform_real_distribution<float>(0.0f, 1.0f);
		}

		virtual T next() = 0;
	};

	template <typename T>
	class NormalSampler : public Sampler<T>
	{
	protected:
		unsigned int seed;
		default_random_engine generator;
		normal_distribution<float> normalDistribution;

	public:
		void init(unsigned int _count)
		{
			this->count = _count;
			this->idx = 0;
			seed = chrono::system_clock::now().time_since_epoch().count();
			generator.seed(seed);
			normalDistribution = normal_distribution<float>(0.0f, 1.0f);
		}

		virtual T next() = 0;
	};

	class UniformSampler1D : public UniformSampler<float>
	{
	public:
		float next()
		{
			float sample = this->uniformDistribution(this->generator);
			return sample;
		}
	};

	class StratifiedSampler1D : public UniformSampler<float>
	{
	public:
		float next()
		{
			float sample = this->uniformDistribution(this->generator);
			return (idx++ + sample) / count;
		}
	};

	class NormalSampler1D : public NormalSampler<float>
	{
	public:
		float next(){
			float sample = this->normalDistribution(this->generator);
			return sample;
		}
	};

	template<size_t N> 
	class UniformSamplerND : public Sampler<vecn<float, N>>
	{
	protected:
		array<UniformSampler1D, N> samplers;
	public:
		void init(unsigned int _count){
			this->idx = 0;
			this->count = _count;
			for(auto &sampler: samplers){
				sampler.init(_count);
			}
		}

		vecn<float, N> next()
		{
			vecn<float, N> sampleVec;
			for(size_t i = 0; i < N; ++i){
				sampleVec[i] = samplers[i].next();
			}
			return sampleVec;
		}
	};

	template<size_t N> 
	class NormalSamplerND : public Sampler<vecn<float, N>>
	{
	protected:
		array<NormalSampler1D, N> samplers;
	public:
		void init(unsigned int _count){
			this->idx = 0;
			this->count = _count;
			for(auto &sampler: samplers){
				sampler.init(_count);
			}
		}

		vecn<float, N> next()
		{
			vecn<float, N> sampleVec;
			for(size_t i = 0; i < N; ++i){
				sampleVec[i] = samplers[i].next();
			}
			return sampleVec;
		}
	};

	template<size_t N>
	class UniformBallSampler : public Sampler<vecn<float,N>>
	{
	public:
		vecn<float, N> next()
		{
			vecn<float, N> u = UniformSamplerND<N>::next();
			float norm = length(u);
			float r = pow(this->uniformDistribution(this->generator), 1.0f/N);

			return (u * r)/norm;
		}
	};
} // namespace RNG
