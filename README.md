#For Compile
	gcc -g -o test buffer_test.c ringbuffer.c -lpthread
	
#For discription
	无锁环形队列的实现，利用了自旋锁实现。
