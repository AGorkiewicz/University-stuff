// Adam Górkiewicz
// 321989
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <bits/stdc++.h>
using namespace std;

const time_t turn_time_sec = 15;
const uint8_t turns_before_deletion = 3;
const uint32_t inf = -1;

uint32_t pack_bytes(uint8_t* bytes)
{
    uint32_t result = 0;
    result |= bytes[0] << 24;
    result |= bytes[1] << 16;
    result |= bytes[2] << 8;
    result |= bytes[3];
    return result;
}

void unpack_bytes(uint8_t* bytes, uint32_t& pack)
{
	bytes[0] = uint8_t(pack >> 24);
	bytes[1] = uint8_t(pack >> 16);
	bytes[2] = uint8_t(pack >> 8);
	bytes[3] = uint8_t(pack);
}

struct network
{
	in_addr_t address;
	uint8_t prefix_len;

	network(in_addr_t address = 0, uint8_t prefix_len = 0) : address(address), prefix_len(prefix_len) {}

	bool contains(in_addr_t member) const
	{
		const uint32_t mask = htonl(0xffffffff << (32 - prefix_len));
		return ((member & mask) == address);
	}
};

bool operator<(network a, network b){
	if (a.prefix_len != b.prefix_len) return a.prefix_len > b.prefix_len;
	return a.address < b.address;
}

string as_str(const in_addr_t& addr)
{
	static char str[20];
	inet_ntop(AF_INET, &addr, str, sizeof(str));
	return string(str);
}

string as_str(const network& net)
{
	return as_str(net.address) + "/" + to_string(net.prefix_len);
}

struct entry
{
	uint32_t distance;
	in_addr_t via;
	uint8_t life;

	bool direct() { return via == 0; }

	entry(uint32_t distance = 0, in_addr_t via = 0) : distance(distance), via(via), life(turns_before_deletion) {}
};

struct router
{
	int sockfd;
	map <network, entry> table;

	struct direct_connections
	{
		map <network, uint32_t> distance;
		set <network> all;
		set <network> connected;

		void insert(network net, uint32_t dis)
		{
			all.insert(net);
			distance[net] = dis;
		}

		network get_network(in_addr_t address)
		{
			for (network net : all){
				if (net.contains(address)){
					return net;
				}
			}
			fprintf(stderr, "received package from an address not in config\n");
			exit(EXIT_FAILURE);
		}
	} direct;

	void disconnect(network net)
	{
		if (direct.connected.count(net)){
			direct.connected.erase(net);
			table[net] = entry(inf);
		}
	}

	void connect(network net)
	{
		direct.connected.insert(net);
		table[net] = entry(direct.distance[net]);
	}

	void read_config()
	{
		int n;

		if (scanf("%d", &n) != 1){
			fprintf(stderr, "Invalid configuration format\n");
			exit(EXIT_FAILURE);
		}
		
		while (n--)
		{
			static char ip_addr_str[25];
			static char ip_addr_only_str[25];
			int prefix_len, distance;
			in_addr_t address;
			if (scanf("%s distance %d", ip_addr_str, &distance) != 2){
				fprintf(stderr, "Invalid configuration format\n");
				exit(EXIT_FAILURE);
			}
			if (sscanf(ip_addr_str, "%15[^/]/%d", ip_addr_only_str, &prefix_len) != 2){
				fprintf(stderr, "Invalid IP address format\n");
				exit(EXIT_FAILURE);
			}
			if (inet_pton(AF_INET, ip_addr_only_str, &address) != 1) {
				fprintf(stderr, "Invalid IP address\n");
				exit(EXIT_FAILURE);
			}
			address &= htonl(0xffffffff << (32 - prefix_len));
			network net(address, prefix_len);
			direct.insert(net, distance);
			connect(net);
			disconnect(net);
		}
	}

	router() : sockfd(socket(AF_INET, SOCK_DGRAM, 0))
	{
		if (sockfd < 0){
			fprintf(stderr, "socket error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		sockaddr_in server_address;
		bzero(&server_address, sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(54321);
		server_address.sin_addr.s_addr = htonl(INADDR_ANY);
		int broadcast_permission = 1;
	
		if (bind(sockfd, (sockaddr*)&server_address, sizeof(server_address)) < 0){
			fprintf(stderr, "bind error: %s\n", strerror(errno)); 
			exit(EXIT_FAILURE);
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void*)&broadcast_permission, sizeof(broadcast_permission)) < 0){
			fprintf(stderr, "setsockopt error: %s\n", strerror(errno)); 
			exit(EXIT_FAILURE);
		}

		read_config();
	}

	~router()
	{
		close(sockfd);
	}

	void expire()
	{
		vector <network> vec;
		for (auto&[key, value] : table){
			if (--value.life == 0){
				vec.push_back(key);
			}
		}
		for (auto key : vec){
			if (table[key].distance == inf){
				table.erase(key);
			}else{
				table[key].distance = inf;
				table[key].life = turns_before_deletion;
			}
		}
	}

	void listen()
	{
		timeval tv;
		tv.tv_sec = turn_time_sec;
		tv.tv_usec = 0;

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
			socklen_t sender_len = sizeof(sender);
			ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (sockaddr*)&sender, &sender_len);
		
			if (packet_len < 0){
				fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			network destination(pack_bytes(buffer), buffer[4]);
			in_addr_t via = sender.sin_addr.s_addr;
			auto via_network = direct.get_network(via);

			if (direct.connected.count(via_network) == 0){
				continue;
			}

			uint32_t distance = pack_bytes(buffer + 5);

			if (distance != inf){
				distance += direct.distance[via_network];
			}

			if (table.find(destination) == table.end() || table[destination].distance > distance || (table[destination].via == via && table[destination].distance != inf)){
				table[destination] = entry(distance, via);
			}
		}

		expire();
	}

	bool send(network destination, network about, uint32_t distance)
	{
		sockaddr_in server_address;
		bzero(&server_address, sizeof(server_address));
		
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(54321);
		server_address.sin_addr.s_addr = destination.address;

		uint8_t buffer[9];
		unpack_bytes(buffer, about.address);
		buffer[4] = about.prefix_len;
		unpack_bytes(buffer + 5, distance);

		if (sendto(sockfd, buffer, sizeof(buffer), 0, (sockaddr*) &server_address, sizeof(server_address)) != sizeof(buffer)) {
			return false;
		}

		return true;
	}

	void send()
	{
		for (auto[about, info] : table){
			for (network destination : direct.all){
				if (send(destination, about, info.distance)){
					connect(destination);
				}else{
					disconnect(destination);
				}
			}
		}
	}

	void show()
	{
		printf("\n");
		for (auto[net, info] : table){
			printf("%s ", as_str(net).c_str());
			if (info.distance == inf){
				printf("unreachable%sconnected directly\n", direct.all.count(net) ? " " : " not ");
			}else{
				printf("distance %u ", (unsigned int)(info.distance));
				if (info.direct()){
					assert(direct.all.count(net));
					printf("connected directly\n");
				}else{
					printf("via %s\n", as_str(info.via).c_str());
				}
			}
		}
		fflush(stdout);
	}

	void run()
	{
		while (true)
		{
			show();
			send();
			listen();
		}
	}
};

int main()
{
	router r;
	r.run();
}
