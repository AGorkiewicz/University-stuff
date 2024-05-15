// Adam Górkiewicz
// 321989
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <bits/stdc++.h>
using namespace std;


uint16_t compute_icmp_checksum(const void *buff, int length)
{
	uint32_t sum;
	const uint16_t* ptr = (const uint16_t*)(buff);
	assert(length % 2 == 0);
	for (sum = 0; length > 0; length -= 2){
		sum += *ptr++;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	return (uint16_t)(~(sum + (sum >> 16)));
}


struct pinger
{
	int sockfd;
	uint16_t program_id;
	string recipient_ip;

	pinger(const string& recipient_ip) : sockfd(socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)), program_id(htons(getpid() & 0xFFFF)), recipient_ip(recipient_ip)
	{
		if (sockfd < 0){
			fprintf(stderr, "socket error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	~pinger()
	{
		close(sockfd);
	}

	bool listen(set <uint16_t> seqs)
	{
		static uint8_t buffer[IP_MAXPACKET];
		set <string> respondents;

		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 1'000'000;

		int delay_sum = 0;
		int delay_num = 0;

		while (!seqs.empty())
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
			
			sockaddr_in sender;	
			socklen_t sender_len = sizeof(sender);
			ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (sockaddr*)&sender, &sender_len);
			
			if (packet_len < 0){
				fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
				exit(EXIT_FAILURE);
			}

			icmp* icmp_header = (icmp*)(buffer + sizeof(ip));
			uint8_t type = icmp_header->icmp_type;
			uint8_t code = icmp_header->icmp_code;
			uint16_t id = icmp_header->icmp_id;
			uint16_t seq = icmp_header->icmp_seq;

			if (type == ICMP_TIME_EXCEEDED){
				uint8_t* data = buffer + sizeof(ip) + sizeof(icmp) + 4;
				id  = data[0] + (uint16_t(data[1]) << 8);
				seq = data[2] + (uint16_t(data[3]) << 8);
			}

			if (type != ICMP_ECHOREPLY && type != ICMP_TIME_EXCEEDED) continue;
			if (code != 0) continue;
			if (id != program_id) continue;
			if (seqs.find(seq) == seqs.end()) continue;

			// ip* ip_header = (ip*)(buffer);
			// u_int32_t saddr = ip_header->ip_src.s_addr;
			// u_int32_t daddr = ip_header->ip_dst.s_addr;

			seqs.erase(seq);

			delay_sum += 1'000'000 - tv.tv_usec;
			delay_num += 1;

			char sender_ip[20];
			
			inet_ntop(AF_INET, &(sender.sin_addr), sender_ip, sizeof(sender_ip));
			
			if (respondents.count(string(sender_ip)) == 0){
				printf("%s ", sender_ip);
				respondents.insert(string(sender_ip));
			}
		}

		if (delay_num == 0){
			printf("*");
		}else if (delay_num == 3){
				printf("%dms", delay_sum / delay_num / 1000);
		}else{
			printf("???");
		}

		printf("\n");

		return (respondents.count(recipient_ip) || respondents.count("127.0.0.1"));
	}

	uint16_t send(int ttl)
	{
		static uint16_t cur_seq = 0;

		setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

		icmp header;
		header.icmp_type = ICMP_ECHO;
		header.icmp_code = 0;
		header.icmp_id = program_id;
		header.icmp_seq = ++cur_seq;
		header.icmp_cksum = 0;
		header.icmp_cksum = compute_icmp_checksum((uint16_t*)&header, sizeof(header));

		sockaddr_in recipient;
		bzero(&recipient, sizeof(recipient));
		recipient.sin_family = AF_INET;
		inet_pton(AF_INET, recipient_ip.c_str(), &recipient.sin_addr);

		ssize_t bytes_sent = sendto(sockfd, &header, sizeof(header), 0, (sockaddr*)&recipient, sizeof(recipient));

		if (bytes_sent != sizeof(icmp)){
			fprintf(stderr, "sendto error: %s\n", strerror(errno)); 
			exit(EXIT_FAILURE);
		}

		return cur_seq;
	}

	void traceroute()
	{
		for (int ttl = 1; ttl <= 30; ttl++){
			set <uint16_t> ids = {send(ttl), send(ttl), send(ttl)};
			printf("%d. ", ttl);
			if (listen(ids) == true) break;
		}
	}
};


string parse_ip(int argc, char ** argv)
{
	if (argc != 2){
		fprintf(stderr, "wrong number of parameters: expected 1, got %d\n", argc - 1);
		exit(EXIT_FAILURE);
	}

	string arg = argv[1];

	auto fail = [&] (string msg = "") {
		fprintf(stderr, "the parameter %s is not a valid ip address%s\n", arg.c_str(), (msg == "" ? "" : " (" + msg + ")").c_str());
		exit(EXIT_FAILURE);
	};

	vector <string> nums = {""};

	for (char c : arg){
		if (c == '.'){
			nums.push_back("");
			continue;
		}
		if (c >= '0' && c <= '9'){
			nums.back().push_back(c);
			continue;
		}
		fail("invalid characters");
	}

	if (nums.size() != 4) fail("too many dots");

	for (string num : nums){
		if (num.size() > 3) fail("number too long");
		if (num.size() == 0) fail("consecutive dots");
		if (num.size() != 1 && num.front() == '0') fail("leading zeros");
		int val = atoi(num.c_str());
		if (val < 0 || val > 255) fail("number out of range [0, 255]");
	}

	return arg;
}


int main(int argc, char ** argv)
{
	pinger p(parse_ip(argc, argv));
	p.traceroute();
}