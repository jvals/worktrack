FLAGS = -L../clogger -I../clogger -llogger -lcmocka -Wl,--wrap=safe_new_entry -Wl,--wrap=time -Wl,--wrap=patch_latest -Wl,--wrap=get_total_diff -Wl,--wrap=logger -g

time_service.o: ../src/time_service.c
	cc -c -o time_service.o ../src/time_service.c $(FLAGS)

test_time_service.o: test_time_service.c
	cc -c -o test_time_service.o test_time_service.c $(FLAGS)

test_time_service: test_time_service.o time_service.o
	cc -o test_time_service $^ $(FLAGS)
