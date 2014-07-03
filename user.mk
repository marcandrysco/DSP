HEADERS = \
	src/defs.h \
	src/filter/defs.h \
	src/flow/defs.h \
	src/reverb/defs.h \
	\
	src/algo.h \
	src/calc.h \
	src/conv.h \
	src/osc.h \
	src/shape.h \
	\
	src/filter/butter.h \
	src/filter/rc.h \
	\
	src/flow/flow.h \
	\
	src/reverb/allpass.h \
	src/reverb/comb.h \
	\
	src/tools/gate.h \
	\
	src/types/array.h \
	src/types/lock.h \
	src/types/sync.h \

all: dsp.h

dsp.h: $(HEADERS) user.mk
	printf '#ifndef LIBDSP_H\n#define LIBDSP_H\n' > dsp.h
	for file in $(HEADERS) ; do sed -e '1,/%dsp.h%/d' -e '/%~dsp.h%/,$$d' $$file >> dsp.h ; done
	printf '#endif\n' >> dsp.h

install: dsp_h_install

dsp_h_install: dsp.h
	install --mode 0644 -D dsp.h "$(PREFIX)/include/dsp.h"

clean: dsp_h_clean

dsp_h_clean:
	rm -f dsp.h

.PHONY: dsp_h_clean
