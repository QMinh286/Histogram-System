#
# This makefile will compile and link the entire HISTO-SYSTEM
# 
# =======================================================
#                  HISTO-SYSTEM
# =======================================================
#
all: dp1 dp2 dc

dp1:
	$(MAKE) -C DP-1

dp2:
	$(MAKE) -C DP-2

# Build DC
dc:
	$(MAKE) -C DC
clean:
	$(MAKE) -C DP-1 clean
	$(MAKE) -C DP-2 clean
	$(MAKE) -C DC clean
	rm -f common/obj/*.o