#ifndef CPL_COLOURVALUE_H
#define CPL_COLOURVALUE_H

#include "ValueBase.h"

namespace cpl
{
	class ColourValue : public ValueGroup
	{
	public:
		enum Index
		{
			R, G, B, A, ColourChannels
		};

#ifdef CPL_JUCE
		juce::Colour getAsJuceColour() {
			auto index = [&](Index i) { return static_cast<float>(getValueIndex(i).getNormalizedValue()); };
			return juce::Colour::fromFloatRGBA(index(R), index(G), index(B), index(A));
		}

		void setFromJuceColour(juce::Colour colour)
		{
			getValueIndex(R).setNormalizedValue(colour.getFloatRed());
			getValueIndex(G).setNormalizedValue(colour.getFloatGreen());
			getValueIndex(B).setNormalizedValue(colour.getFloatBlue());
			getValueIndex(A).setNormalizedValue(colour.getFloatAlpha());
		}
#endif
		template<std::size_t bits, typename ret = std::uint32_t>
		typename std::enable_if<bits < 32, ret>::type getIntValueFor(Index i)
		{
			auto cap = (1 << bits);
			auto fpoint = getValueIndex(i).getNormalizedValue();
			return fpoint == (ValueT)1.0 ? static_cast<ret>(cap - 1) : static_cast<ret>(fpoint * cap);
		}

		virtual std::size_t getNumValues() const noexcept override { return ColourChannels; }

		virtual std::string getContextualName() { return{}; }
	};

	class CompleteColour 
		: private HexFormatter<ValueT>
		, public ColourValue
	{
	public:

		CompleteColour()
			: range(0, 0xFF)
			, values{
				{ &range, this },
				{ &range, this },
				{ &range, this },
				{ &range, this },
			}
		{

		}

		virtual ValueEntityBase & getValueIndex(std::size_t i) override { return values[i]; }

		SelfcontainedValue<> values[4];

	private:
		LinearRange<ValueT> range;
	};


	template<typename UIParameterView>
	class ParameterColourValue 
		: public ColourValue
		, public Parameters::BundleUpdate<UIParameterView>
	{

	public:
		typedef typename UIParameterView::ValueType ValueType;
		typedef typename UIParameterView::ParameterType ParameterType;
		typedef typename Parameters::BundleUpdate<UIParameterView>::Record Entry;

		class SharedBehaviour : private HexFormatter<ValueType>
		{
		public:
			SharedBehaviour() : range(0, 0xFF), context("CC") {}

			VirtualFormatter<ValueType> & getFormatter() { return *this; }
			VirtualTransformer<ValueType> & getTransformer() { return range; }
			const std::string & getContext() { return context; }
		private:
			LinearRange<ValueType> range;
			std::string context;
		};

		ParameterColourValue(SharedBehaviour & b, const std::string & name = "")
			: r("R", b.getTransformer(), &b.getFormatter())
			, g("G", b.getTransformer(), &b.getFormatter())
			, b("B", b.getTransformer(), &b.getFormatter())
			, a("A", b.getTransformer(), &b.getFormatter())
			, behaviour(b)
			, contextName(name)
		{

		}

		virtual std::string getContextualName() override { return contextName; }

		virtual const std::string & getBundleContext() const noexcept override
		{
			return behaviour.getContext();
		}

		virtual std::vector<Entry> & queryParameters() override
		{
			return *parameters.get();
		}

		virtual void parametersInstalled() override
		{
			for (std::size_t i = 0; i < values.size(); ++i)
				values[i].setParameterReference(parameters->at(i).uiParameterView);

			parameters = nullptr;
		}

		void generateInfo() override
		{
			parameters = std::make_unique<std::vector<Entry>>();
			auto add = [&](ParameterType & p) { parameters->push_back(Entry{ &p, true, false }); };
			add(r); add(g); add(b); add(a);
		}

		ValueEntityBase & getValueIndex(std::size_t i) override
		{
			return values[i];
		}

		ParameterType r, g, b, a;
		std::array<ParameterValueWrapper<UIParameterView>, 4> values;

	private:
		std::string contextName;
		std::unique_ptr<std::vector<Entry>> parameters;
		SharedBehaviour & behaviour;
	};
};

#endif