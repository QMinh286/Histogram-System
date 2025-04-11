#
# this makefile will compile and link the DP-1 application
# 
# =======================================================
#                  DP-1
# =======================================================
#
#
# FINAL BINARY Target
./bin/dp1 : ./obj/main.o ./obj/dp1_function.o ../common/obj/circular_buffer.o ../common/obj/ipc_utils.o
	cc ./obj/main.o ./obj/dp1_function.o ../common/obj/circular_buffer.o ../common/obj/ipc_utils.o -o ./bin/dp1
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/main.o : ./src/main.c ./inc/dp1.h
	cc -c ./src/main.c -I./inc -I../common/inc -o ./obj/main.o

./obj/dp1_function.o : ./src/dp1_function.c ./inc/dp1.h ../common/inc/circular_buffer.h ../common/inc/ipc_utils.h ../common/inc/constants.h
	cc -c ./src/dp1_function.c -I./inc -I../common/inc -o ./obj/dp1_function.o

../common/obj/circular_buffer.o : ../common/src/circular_buffer.c ../common/inc/circular_buffer.h ../common/inc/constants.h
	cc -c ../common/src/circular_buffer.c -I../common/inc -o ../common/obj/circular_buffer.o

../common/obj/ipc_utils.o : ../common/src/ipc_utils.c ../common/inc/ipc_utils.h
	cc -c ../common/src/ipc_utils.c -I../common/inc -o ../common/obj/ipc_utils.o
#
# =======================================================
# Other targets
# =======================================================                     
clean:
	rm -f ./bin/dp1
	rm -f ./obj/*.o
	rm -f ../common/obj/*.o
