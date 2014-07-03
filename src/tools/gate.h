#ifndef TOOLS_GATE_H
#define TOOLS_GATE_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Gate structure.
 *   @attack, hold, release: Attack, hold, and release parameters.
 *   @high, low: The high and low thresholds.
 */

struct dsp_gate_t {
	double attack, hold, release;
	double high, low;
};

/**
 * Gate data.
 *   @held: The held time.
 *   @level: The current level.
 */

struct dsp_gate_data_t {
	double held;
	double level;
};


/**
 * Create a new gate.
 *   @rate: The sample rate.
 *   @attack: The attack time in seconds.
 *   @hold: The hold time in seconds.
 *   @release: The release time in seconds.
 *   @high: The high threshold.
 *   @low The low threshold.
 */

static inline struct dsp_gate_t dsp_gate_new(unsigned int rate, float attack, float hold, float release, double high, double low)
{
	struct dsp_gate_t gate;

	gate.attack = 1.0 / (rate * attack);
	gate.hold = 1.0 / (rate * hold);
	gate.release = 1.0 / (rate * release);
	gate.high = high;
	gate.low = low;

	return gate;
}

/**
 * Create a gate data structure.
 *   &returns: The initialized gate data.
 */

static inline struct dsp_gate_data_t dsp_gate_data()
{
	return (struct dsp_gate_data_t){ 1.0, 0.0 };
}

/**
 * Process a gate, obtaining the current gate level.
 *   @x: The input value.
 *   @gate: The gate.
 *   @data: The gate data.
 *   &returns: The current level, between zero and one.
 */

static inline double dsp_gate_proc(double x, struct dsp_gate_t gate, struct dsp_gate_data_t *data)
{
	if((data->held >= 1.0) && (x < gate.high)) {
		data->level = m_dblmax(data->level - gate.release, 0.0);
	}
	else {
		data->level = m_dblmin(data->level + gate.attack, 1.0);
		if((x >= gate.low) && (data->level == 1.0))
			data->held = 0.0;
		else
			data->held += gate.hold;
	}

	return data->level;
}

/*
 * gate function declarations
 */


/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
