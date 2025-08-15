#include <stdlib.h>

int main()
{
	int i;
	char *addr;

	i = 0;
	while (i < 1024)
	{
		i++;
	}
	return (0); 
}

// chaisneau@chaisneau:~/Documents/42/projects/secondaire/42malloc$ cc -o test0 test0.c
// chaisneau@chaisneau:~/Documents/42/projects/secondaire/42malloc$ /usr/bin/time -v ./test0
//         Command being timed: "./test0"
//         User time (seconds): 0.00
//         System time (seconds): 0.00
//         Percent of CPU this job got: 100%
//         Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.00
//         Average shared text size (kbytes): 0
//         Average unshared data size (kbytes): 0
//         Average stack size (kbytes): 0
//         Average total size (kbytes): 0
//         Maximum resident set size (kbytes): 1176
//         Average resident set size (kbytes): 0
//         Major (requiring I/O) page faults: 0
//         Minor (reclaiming a frame) page faults: 68
//         Voluntary context switches: 1
//         Involuntary context switches: 1
//         Swaps: 0
//         File system inputs: 0
//         File system outputs: 0
//         Socket messages sent: 0
//         Socket messages received: 0
//         Signals delivered: 0
//         Page size (bytes): 4096
//         Exit status: 0


// chaisneau@chaisneau:~/Documents/42/projects/secondaire/42malloc$ ./run.sh /usr/bin/time -v ./test0
//         Command being timed: "./test0"
//         User time (seconds): 0.00
//         System time (seconds): 0.00
//         Percent of CPU this job got: 100%
//         Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.00
//         Average shared text size (kbytes): 0
//         Average unshared data size (kbytes): 0
//         Average stack size (kbytes): 0
//         Average total size (kbytes): 0
//         Maximum resident set size (kbytes): 1444
//         Average resident set size (kbytes): 0
//         Major (requiring I/O) page faults: 0
//         Minor (reclaiming a frame) page faults: 80
//         Voluntary context switches: 1
//         Involuntary context switches: 0
//         Swaps: 0
//         File system inputs: 0
//         File system outputs: 0
//         Socket messages sent: 0
//         Socket messages received: 0
//         Signals delivered: 0
//         Page size (bytes): 4096
//         Exit status: 0