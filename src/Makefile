
all: armhf_g armhf 

#armel

.PHONY: armhf_g armhf armel armel_g

armhf:
	$(MAKE) -f arch.mk ARCH="armhf" ROOT=$(ROOT) OPT="-O2" all

armhf_g:
	$(MAKE) -f arch.mk ARCH="armhf" ROOT=$(ROOT) SUFFIX=_g OPT="-O0 -ggdb3" all

armel:
	$(MAKE) -f arch.mk ARCH="armel" ROOT=$(ROOT) OPT="-O2" all

armel_g:
	$(MAKE) -f arch.mk ARCH="armel" ROOT=$(ROOT) SUFFIX=_g OPT="-O0 -ggdb3" all

#	$(MAKE) -f arch.mk ARCH="armel" ROOT=$(ROOT)


clean:
	$(MAKE) -f arch.mk ARCH="armhf" ROOT=$(ROOT) clean
	$(MAKE) -f arch.mk ARCH="armhf" ROOT=$(ROOT) SUFFIX=_g clean	
	$(MAKE) -f arch.mk ARCH="armel" ROOT=$(ROOT) clean
	$(MAKE) -f arch.mk ARCH="armel" ROOT=$(ROOT) SUFFIX=_g clean

