#
# this makefile will compile and link the DC application
# 
# =======================================================
#                  DC
# =======================================================
#
#
# FINAL BINARY Target
./bin/dc : ./obj/main.o ./obj/dc.o ../common/obj/circular_buffer.o ../common/obj/ipc_utils.o
	cc ./obj/main.o ./obj/dc.o ../common/obj/circular_buffer.o ../common/obj/ipc_utils.o -o ./bin/dc
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/main.o : ./src/main.c ./inc/dc.h
	cc -c ./src/main.c -I./inc -I../common/inc -o ./obj/main.o

./obj/dc.o : ./src/dc.c ./inc/dc.h ../common/inc/circular_buffer.h ../common/inc/ipc_utils.h ../common/inc/constants.h
	cc -c ./src/dc.c -I./inc -I../common/inc -o ./obj/dc.o

../common/obj/circular_buffer.o : ../common/src/circular_buffer.c ../common/inc/circular_buffer.h ../common/inc/constants.h
	cc -c ../common/src/circular_buffer.c -I../common/inc -o ../common/obj/circular_buffer.o

../common/obj/ipc_utils.o : ../common/src/ipc_utils.c ../common/include/ipc_utils.h
	cc -c ../common/src/ipc_utils.c -I../common/include -o ../common/obj/ipc_utils.o
#
# =======================================================
# Other targets
# =======================================================                     
clean:
	rm -f ./bin/dc
	rm -f ./obj/*.o
	rm -f ../common/obj/*.o

