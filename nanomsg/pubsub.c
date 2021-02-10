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

#define SERVER "server"
#define CLIENT "client"
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

int send_msgs(const char *url, char *msg, int reps)
{
	int sock;

	if ((sock = nn_socket(AF_SP, NN_PUB)) < 0)
	{
		fatal("nn_socket");
	}
	if (nn_bind(sock, url) < 0)
	{
		fatal("nn_bind");
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	for (int request_nbr = 0; request_nbr != reps; request_nbr++)
	{
		char *msg = date();
		printf("SERVER: PUBLISHING DATE %s\n", d);
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

int client(const char *url, const char *name)
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
		printf("CLIENT (%s): RECEIVED %s\n", name, buf);
		nn_freemsg(buf);
	}
}

int bench_nanomsg(int reps, const char *conn, int exp)
{
	printf("Repetitions, Message Size in characters, protocoll used, Elapsed time in us\n");
	char *msg;
	for (int i = 0; i < exp; i++)
	{
		double size = pow(10, i);
		printf("%d,%.0f,%s,", reps, size, conn);
		msg = rand_string_alloc(size + 1);
		send_msgs(conn, msg, reps);
	}
	return (0);
}

int main(const int argc, const char **argv)
{
	int exp = 7;
	int reps = 10000;

#ifdef DEBUG
	exp = 2;
	reps = 2;
#endif

	if ((argc >= 2) && (strcmp(SERVER, argv[1]) == 0))
		return (server(reps, argv[2]), exp);

	if ((argc >= 3) && (strcmp(CLIENT, argv[1]) == 0))
		return (client(argv[2], argv[3]));

	fprintf(stderr, "Usage: pubsub %s|%s <URL> <ARG> ...\n",
		SERVER, CLIENT);
	return 1;
}