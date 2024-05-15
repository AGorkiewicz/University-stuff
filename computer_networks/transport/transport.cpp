// Adam Górkiewicz
// 321989
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <iterator>
#include <cassert>
#include <cstring>
#include <queue>
using namespace std;

const int window_length = 1337;
const timeval request_renewal_time = {0, 213769};

timeval get_time()
{
	timeval result;
	gettimeofday(&result, NULL);
	return result;
}

struct downloader
{
	struct entry
	{
		uint8_t data[1000];
		bool loaded = false;
		timeval last_requested = {0, 0};
	};

	int sockfd = 0;
	int filefd = 0;
	in_addr_t server_ip = 0;
	uint16_t server_port = 0;
	uint32_t total_data_length = 0;
	entry window[window_length];
	int window_position = 0;
	queue <int> events;

	downloader(int argc, char ** argv)
	{
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);

		if (sockfd < 0){
			fprintf(stderr, "socket error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		sockaddr_in address;
		bzero(&address, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(INADDR_ANY);
	
		if (bind(sockfd, (sockaddr*)&address, sizeof(address)) < 0){
			fprintf(stderr, "bind error: %s\n", strerror(errno)); 
			exit(EXIT_FAILURE);
		}

		auto fail = [&](string msg){
			fprintf(stderr, "configuration error: %s\n", msg.c_str()); 
			exit(EXIT_FAILURE);
		};
		
		if (argc != 5) fail("wrong number of arguments");

		if (inet_pton(AF_INET, argv[1], &server_ip) != 1) fail("invalid IP address");

		auto parse = [&](string arg, int limit, string error){
			int result = 0;
			if (arg.empty()) fail(error);
			if (arg[0] == '0') fail(error);
			for (char c : arg){
				if (c < '0' || c > '9') fail(error);
				if (((result *= 10) += (c - '0')) >= limit) fail(error);
			}
			return result;
		};

		server_port = parse(argv[2], 1 << 16, "invalid port");
		total_data_length = parse(argv[4], 1 << 25, "invalid datagram size");

		filefd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);

		if (filefd == -1){
			fprintf(stderr, "file opening error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	~downloader()
	{
		if (sockfd != 0) close(sockfd);
		if (filefd != 0) close(filefd);
	}

	int position_length(int position)
	{
		return max(0, min((int)(total_data_length) - position * 1000, 1000));
	}

	void send_request(int position)
	{
		int start = position * 1000;
		int length = position_length(position);
		entry& info = window[position % window_length];
		info.last_requested = get_time();
		events.push(position);

		assert(length > 0);
		assert(info.loaded == false);

		static char buffer[1005];
		sprintf(buffer, "GET %d %d\n", start, length);
		ssize_t buffer_len = strlen(buffer);

		sockaddr_in server_address;
		bzero(&server_address, sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(server_port);
		server_address.sin_addr.s_addr = server_ip;

		if (sendto(sockfd, buffer, buffer_len, 0, (sockaddr*)&server_address, sizeof(server_address)) != buffer_len) {
			fprintf(stderr, "sendto error: %s\n", strerror(errno)); 
			exit(EXIT_FAILURE);
		}
	}

	void download()
	{
		for (int position = 0; position < window_length; position++){
			if (position_length(position) > 0){
				send_request(position);
			}
		}

		while (!events.empty()){
			int position = events.front();
			events.pop();
			assert(position < window_position + window_length);
			if (position < window_position) continue;
			entry& info = window[position % window_length];
			if (info.loaded) continue;
			timeval pause = info.last_requested;
			timeval current_time = get_time();
			timersub(&pause, &current_time, &pause);
			timeradd(&pause, &request_renewal_time, &pause);
			listen(pause);
			if (position < window_position) continue;
			if (info.loaded) continue;
			send_request(position);
		}
	}

	bool dump()
	{
		int length = position_length(window_position);
		if (length == 0) return false;
		entry& info = window[window_position % window_length];
		if (info.loaded == false) return false;
		if (write(filefd, info.data, length) == -1){
			fprintf(stderr, "write error: %s\n", strerror(errno)); 
			exit(EXIT_FAILURE);
		}
		double progress = double(window_position * 1000 + length) / total_data_length;
		printf("%.3lf%% done\n", 100.0 * progress);
		info.loaded = false;
		window_position += 1;
		if (position_length(window_position + window_length - 1) > 0){
			send_request(window_position + window_length - 1);
		}
		return true;
	}

	void listen(timeval tv)
	{
		if (tv.tv_sec < 0) return;

		while (true)
		{
			fd_set descriptors;
			FD_ZERO(&descriptors);
			FD_SET(sockfd, &descriptors);
			
			int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
		
			if (ready < 0){
				fprintf(stderr, "select error: %s\n", strerror(errno)); 
				exit(EXIT_FAILURE);
			}

			if (ready == 0) break;

			static uint8_t buffer[IP_MAXPACKET];
			sockaddr_in sender;
			socklen_t sender_length = sizeof(sender);
			ssize_t packet_length = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (sockaddr*)&sender, &sender_length);
		
			if (packet_length < 0){
				fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			int data_start, data_length;

			if (sscanf((char*)(buffer), "DATA %d %d\n", &data_start, &data_length) != 2) continue;

			if (data_start % 1000 != 0) continue;

			int position = data_start / 1000;

			if (data_length == 0) continue;
			if (position < window_position) continue;
			if (position >= window_position + window_length) continue;
			if (position_length(position) != data_length) continue;

			uint8_t* data = buffer + packet_length - data_length;

			entry& info = window[position % window_length];
			info.loaded = true;
			for (int i = 0; i < data_length; i++){
				info.data[i] = data[i];
			}

			while (true){
				if (dump() == false){
					break;
				}
			}
		}
	}
};

int main(int argc, char ** argv)
{
	downloader d(argc, argv);
	d.download();
}
