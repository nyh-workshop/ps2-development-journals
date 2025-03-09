
EE_BIN=test.elf
EE_OBJS=draw_3D.o main.o

EE_LIBS=-ldma -lgraph -ldraw -lkernel -ldebug -lc -lpacket -ldma -lpacket2
EE_DVP = dvp-as
EE_VCL = openvcl

EE_CFLAGS += -Wall
EE_LDFLAGS = -L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib

%.vsm: %.vcl
	$(EE_VCL) $< >> $@

%.o: %.vsm
	$(EE_DVP) $< -o $@

PS2SDK=/usr/local/ps2dev/ps2sdk

include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref

.SECONDARY: $(%.vsm)

.PHONY: docker-build
docker-build:
	docker run -v $(shell pwd):/src ps2build make


.PHONY: clean
clean:
	rm -rf $(ISO_TGT) $(EE_BIN) $(EE_OBJS) *.vsm