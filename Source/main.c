#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//I don't know existly why these aren't actually necessary, but my guess is netinet includes them.
//#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>

void send_html(int sockfd, const char *filename) {
	FILE *html_file = fopen(filename, "rb");
	if (!html_file) {
		const char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
		write(sockfd, response, strlen(response));
		return;
	}
	
	fseek(html_file, 0, SEEK_END);
	long file_size = ftell(html_file);
	rewind(html_file);
	
	char header[256];
	sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n", file_size);
	write(sockfd, header, strlen(header));
	
	char buffer[4096];
	int bytes_read;
	while((bytes_read = fread(buffer, 1, sizeof(buffer), html_file)) > 0) {
		write(sockfd, buffer, bytes_read);
	}
	
	fclose(html_file);
}

int main(int argc, char **argv) {
	int sockfd, newsockfd, port = 9999; //default port and socket stuff
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		perror(":: error opening socket.\n");
		return 1;
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	if(argc == 2) port = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror(":: error binding socket.\n");
		return 1;
	}
	
	printf("Listening on port %d...\n", port);
	
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	
	beginning: {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //accept new connections
		send_html(newsockfd, "index.html");
	} goto beginning;
	
	close(newsockfd);
	close(sockfd);
	return 0;
}
