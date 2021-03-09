#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>
#include <inttypes.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>

#define SUBSCRIBER "subscriber"
#define PUBLISHER "publisher"
#define DEBUG

void fatal(const char *func)
{
	fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
}

char *date(void)
{
	time_t now = time(&now);
	struct tm *info = localtime(&now);
	char *text = asctime(info);
	text[strlen(text) - 1] = '\0'; // remove '\n'
	return (text);
}

static char *rand_string(char *str, size_t size)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (size)
	{
		--size;
		for (size_t n = 0; n < size; n++)
		{
			int key = rand() % (int)(sizeof charset - 1);
			str[n] = charset[key];
		}
		str[size] = '\0';
	}
	return str;
}

char *rand_string_alloc(size_t size)
{
	char *s = malloc(size + 1);
	if (s)
	{
		rand_string(s, size);
	}
	return s;
}

int pub_client(const char *url, const char *client_name, char *msg, int reps)
{
	struct timespec start, end;
	int sock;

	if ((sock = nn_socket(AF_SP, NN_PUB)) < 0)
	{
		fatal("nn_socket");
	}
	if (nn_bind(sock, url) < 0)
	{
		fatal("[pub_client] nn_bind");
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	for (int request_nbr = 0; request_nbr != reps; request_nbr++)
	{
		char *msg = "123456";
		printf("SERVER: PUBLISHING MSG\n");
		int bytes = nn_send(sock, msg, strlen(msg) + 1, 0);
		if (bytes < 0)
		{
			fatal("nn_send");
		}
		// sleep(1);
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;

	printf("%" PRIu64 "\n", delta_us);
}

int sub_client(const char *url, const char *client_name)
{
	int sock;

	if ((sock = nn_socket(AF_SP, NN_SUB)) < 0)
	{
		fatal("nn_socket");
	}

	// subscribe to everything ("" means all topics)
	if (nn_setsockopt(sock, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) < 0)
	{
		fatal("nn_setsockopt");
	}
	if (nn_connect(sock, url) < 0)
	{
		fatal("nn_connet");
	}
	for (;;)
	{
		char *buf = NULL;
		int bytes = nn_recv(sock, &buf, NN_MSG, 0);
		if (bytes < 0)
		{
			fatal("nn_recv");
		}
		printf("SUB (%s): RECEIVED %s\n", client_name, buf);
		nn_freemsg(buf);
	}
}

int bench_pub_client(const char *conn, const char *client_name, int reps, int exp)
{
	printf("Repetitions, Message Size in characters, protocoll used, Elapsed time in us\n");
	char *msg;
	for (int i = 0; i < exp; i++)
	{
		double size = pow(10, i);
		printf("%d,%.0f,%s,", reps, size, conn);
		msg = rand_string_alloc(size + 1);
		pub_client(conn, client_name, msg, reps);
	}
	return (0);
}

int main(const int argc, const char **argv)
{

	int max_msg_power = 7; // 1=[10], 2=[10,100] 3=[10,100,1000]...
	int reps = 10000;

#ifdef DEBUG
	max_msg_power = 2;
	reps = 2;
#endif

	if (argc != 4)
	{
		fprintf(stderr, "Incorrect usage. Supply 3 arguments: '[%s/%s] [connection string] [client name]'\n",
				SUBSCRIBER, PUBLISHER);
	}
	else
	{
		const char *uri = argv[2];
		const char *client_name = argv[3];

		if (strcmp(SUBSCRIBER, argv[1]) == 0)
			return (sub_client(uri, client_name));

		if (strcmp(PUBLISHER, argv[1]) == 0)
			return (bench_pub_client(uri, client_name, reps, max_msg_power));
	}

	return 1;
}