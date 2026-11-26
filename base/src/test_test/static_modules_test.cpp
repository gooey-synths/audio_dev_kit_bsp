#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <modules/static/oscillator.hpp>
#include <modules/static/fixed_output.hpp>
#include <cmath>

static void runOscillator(modules::stat::Oscillator& o, uint16_t* buf, size_t nSamples) {
    for(size_t i = 0; i < nSamples; i++) {
        o.run();
        buf[i] = o.getOutput(0);
    }
}

// Note: This only counts the periods of simple waveforms and an actual midpoint!
static size_t countPeriods(uint16_t* buf, size_t nSamples, uint16_t midPoint) {
    size_t nHalfPeriods = 0;

    for(size_t i = 1; i < nSamples; i++) {
        uint16_t last = buf[i-1];
        uint16_t current = buf[i];
        uint16_t min = last > current? current : last;
        uint16_t max = last > current? last : current;
        if(midPoint >= min && midPoint < max && min != max) {
            nHalfPeriods++;
        }
    }

    return nHalfPeriods/2;
}

static void countSlopes(uint16_t* buf, size_t nSamples, int& nPos, int& nNeg) {
    nPos = 0;
    nNeg = 0;

    for(size_t i = 1; i < nSamples; i++) {
        uint16_t last = buf[i-1];
        uint16_t current = buf[i];
        if(current > last) {
            nPos++;
        }
        if(last > current) {
            nNeg++;
        }
    }
}

TEST(OscillatorTests, HappyPathTests) {
    const size_t nTestSamples = 1024;
    std::string waveTypes[] = {
        "saw",
        "tri",
        "sqr"
    };

    for(std::string& w : waveTypes) {
        uint16_t testBuffer[nTestSamples];

        modules::stat::Oscillator o;
        std::unordered_map<std::string, std::string> params;

        // Test center frequency
        for(float nPeriods = 1; nPeriods <= 10; nPeriods++) {
            // Calculate a center frequency such that it will give us 1 period per nTestSamples
            float cf = module_basics::scSamplerate / (float)nTestSamples;
            // Multiply frequency by how many periods we want
            cf *= nPeriods * 1.01F; // Nudge freq a bit to account for rounding stuff
            params["cf"] = std::to_string(cf);
            params["opc"] = std::to_string(0); // No octave shift
            params["wav"] = w;
            o.configure(params);
            o.setInput(o.getInputIdx("mod"), 0xFFFF/2);
            // Check that we have generated that many periods
            runOscillator(o, testBuffer, nTestSamples);
            EXPECT_EQ(countPeriods(testBuffer, nTestSamples, 0xFFFF/2), nPeriods);
        }

        // Test octave shifting
        for(float nPeriods = 1; nPeriods <= 10; nPeriods++) {
            // Calculate a center frequency such that it will give us 1 period per nTestSamples
            float cf = module_basics::scSamplerate / nTestSamples;
            cf *= 1.01; // Nudge freq a bit to account for rounding stuff
            float octavesPerCount = 1.0F/500.0F; // Make this small to reduce inaccuracy
            params["cf"] = std::to_string(cf);
            params["opc"] = std::to_string(octavesPerCount);
            params["wav"] = w;
            o.configure(params);
            // Calculate octave shift based on frequency for nPeriods
            float octaveShift = log2(nPeriods);
            float counts = ceil(octaveShift / octavesPerCount);
            o.setInput(o.getInputIdx("mod"), (0xFFFF/2)+(uint16_t)counts);
            // Check that we have generated that many periods
            runOscillator(o, testBuffer, nTestSamples);
            EXPECT_EQ(countPeriods(testBuffer, nTestSamples, 0xFFFF/2), nPeriods);
        }

        // Test waveform type
        int nPos;
        int nNeg;
        // Calculate a center frequency such that it will give us 1 period per nTestSamples
        float cf = module_basics::scSamplerate / (float)nTestSamples;
        cf *= 1.01F; // Nudge freq a bit to account for rounding stuff
        params["cf"] = std::to_string(cf);
        params["opc"] = std::to_string(0); // No octave shift
        params["wav"] = w;
        o.configure(params);
        o.setInput(o.getInputIdx("mod"), 0xFFFF/2);
        // Check that we have generated that many periods
        runOscillator(o, testBuffer, nTestSamples);
        countSlopes(testBuffer, nTestSamples, nPos, nNeg);

        if(w.compare(std::string("saw")) == 0) {
            // Saw waves only have one negative slope
            EXPECT_GT(nPos, 0);
            EXPECT_LE(nNeg, 1);
        } else if(w.compare(std::string("tri")) == 0) {
            // Triangle waves have similar amounts of postive and negative slopes
            EXPECT_LE(abs(nPos-nNeg), 10);
        } else if(w.compare(std::string("sqr")) == 0) {
            // Square waves have only highs and lows
            for(uint16_t& sample : testBuffer) {
                EXPECT_THAT(sample, ::testing::AnyOf(0, 0xFFFF));
            }
        } else {
            // Unknown waveform
            FAIL();
        }
    }
}

TEST(OscillatorTests, SadPathTests) {
    modules::stat::Oscillator o;

    // Test missing wave type
    std::unordered_map<std::string, std::string> a = {
        {"opc", "0.1"},
        {"cf", "1000"}
    };
    EXPECT_THROW(
        try {
            o.configure(a);
        } catch(const char* c) {
            EXPECT_EQ(c, "Key not found");
            throw;
        }, const char*);

    // Test missing octaves per count
    std::unordered_map<std::string, std::string> b = {
        {"wav", "saw"},
        {"cf", "1000"}
    };
    EXPECT_THROW(
        try {
            o.configure(b);
        } catch(const char* c) {
            EXPECT_EQ(c, "Key not found");
            throw;
        }, const char*);

    // Test missing center frequency
    std::unordered_map<std::string, std::string> c = {
        {"wav", "saw"},
        {"opc", "1"}
    };
    EXPECT_THROW(
        try {
            o.configure(c);
        } catch(const char* _c) {
            EXPECT_EQ(_c, "Key not found");
            throw;
        }, const char*);

    // Test invalid wave type
    std::unordered_map<std::string, std::string> d = {
        {"wav", ";)"},
        {"opc", "1"},
        {"cf", "231.23"}
    };
    EXPECT_THROW(
        try {
            o.configure(d);
        } catch(const char* c) {
            EXPECT_EQ(c, "Invalid wave type");
            throw;
        }, const char*);
}

TEST(FixedOutputTests, HappyPathTests) {
    // Test that the output is what we configure it to be
}


TEST(FixedOutputTests, SadPathTests) {
    // Test that we throw if value is not provieded.

}
