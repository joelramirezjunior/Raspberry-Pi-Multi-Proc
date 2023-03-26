#include "myprogram.h"
#include <errno.h>
#define MAX_FILENAME_LEN 100
#define NUM_PIS 3 // ONLY EDIT HERE IF WANT TO CHANGE NUMBER OF PIS
#define SPACE " "
int trace_p = 0; 
pid_t pi_pids[NUM_PIS];


int main(int argc, char *argv[]) {

    if(argc < 2){
        printf("Sorry, please make sure to feed in a file with dependancies");
        return 0;
    }

    for (int i = 0; i < NUM_PIS; i++) {
        pi_pids[i] = -1;
    }
    char *filename = argv[1];

    Graph *graph = convert_txt_dependency_graph(filename);

    setUpGraph(graph);
    printEntireJobList(graph);
    int arr_pis[NUM_PIS];

    for (int i = 0; i < NUM_PIS; i++) {
        arr_pis[i] = -1;
    }

    char **dev_names = find_ttyusb_last_n(NUM_PIS);

    uint32_t num_programs_to_run = graph->numVertices; 
    printf("num vertices:%d\n", num_programs_to_run);
    uint32_t num_programs_sent = 0;
    uint32_t num_programs_done = 0;
    printf("3\n");

    time_t start, end;
    double seconds;
    start = time(NULL);

    while (num_programs_done < num_programs_to_run) {
        for (int i = 0; i < NUM_PIS; i++) {
            // printf("i=%d\n", i);
            if(check_pi_done(i) || pi_pids[i] == -1) {
                int vertex = -1;
                vertex = arr_pis[i];
                if (vertex != -1) {
                    printf("Marking job as finished: %d!\n", vertex);
                    markNodeAsFinished(graph, vertex);
                    num_programs_done++;
                    printf("num_programs_done=%d\n", num_programs_done);
                }
                if(isJobAvailable(graph)){ //do a check here to see if there is a program available to send over. 
                    Node *job = grabAvailableJob(graph);
                    send_to_pi(i, job->fileName, dev_names, job); //put a check here to see if there is even any jobs to do. 
                    markNodeInProgress(graph, job->vertex);
                    num_programs_sent++;
                    printf("num_programs_sent=%d\n", num_programs_sent);
                    arr_pis[i] = job->vertex;
                }
            } 
        } 
    }  
    end = time(NULL);
    seconds = difftime(end, start);
    printf("seconds to complete: %f\n", seconds);
    return 0;  
}

// basically my-install
void send_to_pi(uint32_t pi_num, char *filename, char **dev_names, Node *job) {
    char *dev_name = dev_names[pi_num];
    pid_t child_pid  = fork();
    if (child_pid != 0) {
        printf("In the Parent\n");
        pi_pids[pi_num] = child_pid;
        return;
    }
    int tty = open_tty(dev_name);
    if(tty < 0)
        panic("can't open tty <%s>\n", dev_name);
    unsigned baud_rate = B115200;
    double timeout_tenths = 2*5;
    int fd = set_tty_to_8n1(tty, baud_rate, timeout_tenths);
    if(fd < 0)
        panic("could not set tty: <%s>\n", dev_name);

    // 3. read in program [probably should just make a <file_size>
    //    call and then shard out the pieces].
    unsigned nbytes;
    uint8_t *code = read_file(&nbytes, filename);

    // 4. let's send it!
    debug_output(" tty-usb=<%s> program=<%s>: about to boot\n", 
                 dev_name, filename);

    if (child_pid == 0) {
	if (job->numParents == 0) {
	    //no dependencies
	    simple_boot(fd, code, nbytes, 0, 0, 0);
	}
	//TODO: format the stuff so there are headers between each payload
	else {
	    //we have dependencies
	    //read them continuously into a uint8_t buffer
	    //also get some metadata and pass that to simple boot too
	    unsigned data_n = 0; //number of bytes total that the pi will receive
	    unsigned n_payloads = job->numParents;
	    char *parentoutputs[n_payloads];
	    int parentsizes[n_payloads];


	    //populate those arrays
	    for (int i = 0; i < n_payloads; i++) {
		//name
		char *parentbin = (job->parents[i])->fileName;
		char *parentroot = strtok(parentbin, ".");
		char *parentoutput = strcat(parentroot, ".output");
		parentoutputs[i] = parentoutput;

		//size
		struct stat st;
		int check = stat(parentoutputs[i], &st);
		if (check == -1) {
		    printf("ERRORLSDJFDSFJ:LSDFJLSDKFJ:SDFJS:DLFKJSDFSD\n");
		    printf("stat error for %s. Errno: %d\n", parentoutputs[i], errno);
		}
		parentsizes[i] = st.st_size - 8; //subtract the \nDONE!!!
		data_n += parentsizes[i];
	    }

	    //just make sure it's 8-aligned!
	    unsigned data_n_aligned = data_n + sizeof(unsigned); //add header size 
	    data_n_aligned = (data_n_aligned + 8) - (data_n_aligned + 8) % 8;
	    //create data_buf array
	    uint8_t data_buf[data_n_aligned];

	    // Convert data_n to a byte array
	    uint8_t data_n_bytes[4];
	    data_n_bytes[0] = (data_n >> 24) & 0xFF;
	    data_n_bytes[1] = (data_n >> 16) & 0xFF;
	    data_n_bytes[2] = (data_n >> 8) & 0xFF;
	    data_n_bytes[3] = data_n & 0xFF;
	    
	    // Copy the bytes to the beginning of data_buf
	    for (int i = 0; i < 4; i++) {
	        data_buf[i] = data_n_bytes[i];
		printf("header: %d\n", data_buf[i]);
	    }
	    //unsigned header_unsigned[1] = {data_n};
	    //uint8_t header_uint8_t[sizeof(unsigned) / sizeof(uint8_t)];
	    //memmove(data_buf, header_uint8_t, sizeof(uint8_t));
	    uint8_t *tmp = data_buf + sizeof(unsigned);


	    //data_buf format:
	    //[unsigned data_n][payload][payload]...etc[possible 0s for 8-align]
	    

	    //read each depedency file into our data_buf array sequentially
	    for (int i = 0; i < n_payloads; i++) {
	        printf("reading this depedency: %s\n", parentoutputs[i]);
		
		//open the parent file
		int fd = open(parentoutputs[i], O_RDONLY);
		if (fd == -1) {
		    printf("ERRORLSDJFDSFJ:LSDFJLSDKFJ:SDFJS:DLFKJSDFSD\n");
		    printf("open error on %s, errno: %d\n", parentoutputs[i], errno);
		}

		//read the parent file
		int bytesread = read(fd, tmp, parentsizes[i]);
		if (bytesread != parentsizes[i]) {
		    printf("ERRORLSDJFDSFJ:LSDFJLSDKFJ:SDFJS:DLFKJSDFSD\n");
		    printf("read error on %s\n", parentoutputs[i]);
		}
		tmp += parentsizes[i]; //increment to end of databuf for next loop
	    }
	    //clear out the remaining bytes of databuf if any
	    for (int i = tmp - data_buf; i < data_n_aligned; i++) {
		data_buf[i] = 0;
	    }
	    
	    //print data_buf to check
	    printf("--------------------------------------------\n");
	    unsigned data_n_tmp = (data_buf[0] << 24) | (data_buf[1] << 16) | (data_buf[2] << 8) | (data_buf[3]); 
	    printf("here is the header: %d\n", data_n_tmp);
	    printf("Here is our data_buf for %s-----------------\n", filename);
	    //for (int i = 0; i < n_payloads; i++) {
		printf("%s", (char *)data_buf + sizeof(unsigned));
	    //}
	    printf("[END]\n----------------------------------------------\n");
	    printf("we read in %d bytes across %d payloads\n", data_n, n_payloads);
	    printf("and the data buf is rounded up to be %d bytes\n", data_n_aligned);

            simple_boot(fd, code, nbytes, data_buf, data_n, n_payloads);
	}


	//echo stuff from the pi into a file too, for program output to use for dependencies
	char * filename_root = strtok(filename, "."); //get the a in a.bin, b in b.bin, etc. 
	const char *filepath = strcat(filename_root, ".output"); //call them a.output, b.output, etc
	int output_fd = open(filepath, O_CREAT | O_RDWR, 777);
        if (output_fd == -1) {
	    printf("ERROR IN FILE CREATION when trying to make %s\n", filepath);
	    exit(-1);
	}
        
	pi_echo(0, fd, dev_name, output_fd);
	
        exit(0); //should never get here.
    }
}

// returns 1 if done and 0 if not done
uint32_t check_pi_done(uint32_t pi_num) {
    int status;
    waitpid(pi_pids[pi_num], &status, WNOHANG);
    if (WIFEXITED(status)) {
        return 1;
    }
    return 0;
}

Graph *convert_txt_dependency_graph(char *fileName) {
  FILE *file = fopen(fileName, "r");
  
  if (file == NULL) {
    printf("Error: File not found.\n");
    return NULL;
  }
    
  char buffer[1000];
  char *token;
  char *filename;
  char *dependency;
  char *dependencyList;
  int numDependencies;
  
  // Map to store the dependencies
  char **map[MAX_FILENAME_LEN];
  int numFiles = 0;

  Graph *graph = createGraph();
  // Read each line of the file
  while (fgets(buffer, 1000, file) != NULL) {
    // Split the line into tokens
    filename = strtok(buffer, ":");
    addNode(graph, graph->numVertices, filename);

    dependencyList = strtok(NULL, ":");
    // printf("Dependancy:%s\n", dependencyList);
    fflush(stdout);
    dependencyList++;
    int len = strlen(dependencyList);
    /* get the first token */
    dependency = strtok(dependencyList, ", \n");
    /* walk through other tokens */
    
    while(dependency != NULL) {
        if (strcmp(dependency, "[]") == 0) break;
        // printf( "Here:%s\n", dependency);
        addNode(graph, graph->numVertices, dependency);
        addEdge(graph, dependency, filename);
        dependency = strtok(NULL, ", \n");
    }
  }
  fclose(file);
  printf("1");
  return graph;
}

