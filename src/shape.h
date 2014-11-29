#ifndef SHAPE_H
#define SHAPE_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Attack-decay-sustain-release structure.
 *   @attack, decay, release: Attack, decay, and release time in samples.
 *   @sustian: Sustain gain.
 */

struct dsp_adsr_t {
	unsigned int attack, decay, release;
	double sustain;
};


/**
 * Generate an ADSR envelope.
 *   @attack: The attack in seconds.
 *   @decay: The decay in seconds.
 *   @sustain: The sustain volume in decibels.
 *   @release: The release in seconds.
 *   @rate: The rate.
 *   &returns: The ADSR envelope.
 */

static inline struct dsp_adsr_t dsp_adsr_init(double attack, double decay, double sustain, double release, unsigned int rate)
{
	struct dsp_adsr_t adsr;

	adsr.attack = rate * attack;
	adsr.decay = rate * decay + adsr.attack;
	adsr.release = rate *release;
	adsr.sustain = sustain;

	return adsr;
}

/**
 * Process an ADSR envelope.
 *   @adsr: The ADSR envelope.
 *   @t: The time.
 *   @sus: The sustain length.
 *   &returns: The volume level in decibels.
 */

static inline double dsp_adsr_proc(struct dsp_adsr_t adsr, unsigned int t, unsigned int sus)
{
	if(sus < adsr.decay)
		sus = adsr.decay;

	if(t < adsr.attack)
		return -60.0 + (60.0 / adsr.attack) * t;
	else if(t < adsr.decay)
		return adsr.sustain / (adsr.decay - adsr.attack) * (t - adsr.attack);
	else if(t < sus)
		return adsr.sustain;
	else if(t < (sus + adsr.release))
		return adsr.sustain + (-60.0 - adsr.sustain) / adsr.release * (t - sus);
	else
		return -INFINITY;
}

/**
 * Check if the ADSR envelope is still active.
 *   @adsr: The ADSR envelope.
 *   @t: The time.
 *   @sus: The sustain length.
 *   &returns: True if active false otherwise.
 */

static inline bool dsp_adsr_active(struct dsp_adsr_t adsr, unsigned int t, unsigned int sus)
{
	return t < (sus + adsr.release);
}


/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
