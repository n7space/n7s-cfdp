#include "test_transport.h"

static const char IP_ADDRESS[] = "127.0.0.1";
static int sockfd;

void *test_transport_receiver_thread(void *arg)
{
	uint8_t buffer[BUFFER_SIZE];
	memset(buffer, 0x0, (size_t)BUFFER_SIZE);
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	while (1) {
		int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
				 (struct sockaddr *)&client_addr, &addr_len);
		if (n > 0) {
			buffer[n] = '\0';
		} else {
			perror("recvfrom failed\n");
			continue;
		}

		struct cfdp_core *core = arg;
		cfdp_core_received_pdu(core, buffer, n);
	}

	return NULL;
}

void test_transport_init_and_bind(struct cfdp_core *core)
{
	struct sockaddr_in server_addr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("socket creation failed\n");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	server_addr.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
	    0) {
		perror("bind failed\n");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, test_transport_receiver_thread,
			   (void *)core) != 0) {
		perror("pthread_create failed\n");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
}

void test_transport_close() { close(sockfd); }

void test_transport_send_pdu(void *user_data, const byte pdu[], const int size)
{
	struct sockaddr_in receiver_addr;
	memset(&receiver_addr, 0, sizeof(receiver_addr));
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	receiver_addr.sin_port = htons(RECEIVER_PORT);

	int bytes_sent =
	    sendto(sockfd, pdu, size, 0, (struct sockaddr *)&receiver_addr,
		   sizeof(receiver_addr));
	if (bytes_sent == -1) {
		int errsv = errno;
		printf("socket send error %d\n", errsv);
	}
	printf("socket bytes sent %d\n", bytes_sent);
}

bool test_transport_is_ready(void *user_data) { return true; }